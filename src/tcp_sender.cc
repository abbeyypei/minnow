#include "tcp_sender.hh"
#include "tcp_config.hh"
#include "../tests/sender_test_harness.hh"

#include <random>
#include <iostream>

using namespace std;

/* TCPSender constructor (uses a random ISN if none given) */
TCPSender::TCPSender( uint64_t initial_RTO_ms, optional<Wrap32> fixed_isn )
  : isn_( fixed_isn.value_or( Wrap32 { random_device()() } ) ), initial_RTO_ms_( initial_RTO_ms ), window_( isn_ + 1 ), cur_time_( initial_RTO_ms + 1 ), rto_( initial_RTO_ms ), retrans_timer_( -1 ), cons_retrans_( 0 ), seq_num_in_flight_( 0 ), syn_( false ), fin_( false ), retransmit_( false ), zero_window_( false ), msg_queue_(), retransmit_queue_()
{}

uint64_t TCPSender::sequence_numbers_in_flight() const
{
  // Your code here.
  return seq_num_in_flight_;
}

uint64_t TCPSender::consecutive_retransmissions() const
{
  // Your code here.
  return cons_retrans_;
}

optional<TCPSenderMessage> TCPSender::maybe_send()
{
  // Your code here.
  // cout << "maybe send retrans" << retransmit_ << "\n";
  if (retransmit_ && !retransmit_queue_.empty()) {
    TCPSenderMessage msg = retransmit_queue_.top().second;
    // cout << "retrans msg " << retransmit_queue_.top().first << "\n";
    retrans_timer_ = cur_time_;
    retransmit_ = false;
    return msg;
  }
  if (!msg_queue_.empty()) {
    ql msg = msg_queue_.top();
    retransmit_queue_.push(msg);
    msg_queue_.pop();
    if (retrans_timer_ < 0) {
      retrans_timer_ = cur_time_;
    }
    cout << "sending msg " << msg.first << "\n";
    return msg.second;
  }
  return {};
}

void TCPSender::push( Reader& outbound_stream )
{
  // cout << "push called with window " << window_ << "\n";
  // Your code here.
  string payload = "";
  TCPSenderMessage msg;
  bool fin = false;

  if ((!syn_)) {
    if (outbound_stream.is_finished() && isn_+ 1 < window_) {
      fin = true;
      fin_ = true;
    }
    msg = {
      isn_,
      true,
      payload,
      fin
    };
    cout << "pushing SYN \n";
    msg_queue_.push(make_pair(isn_, msg));
    seq_num_in_flight_ += msg.sequence_length();
    isn_ = isn_ + msg.sequence_length();
    syn_ = true;
  }

  while ((outbound_stream.bytes_buffered() > 0) && isn_ < window_)
  {
    uint64_t payload_size = min(outbound_stream.bytes_buffered(), TCPConfig::MAX_PAYLOAD_SIZE);
    payload_size = min(payload_size, static_cast<uint64_t>(window_ - isn_));
    // cout << "payload size " << payload_size << "\n";
    read(outbound_stream, payload_size, payload);
    if (outbound_stream.is_finished() && isn_ + payload.size() < window_) {
      fin = true;
      fin_ = true;
    }
    msg = {
      isn_,
      false,
      payload,
      fin
    };
    // cout << "pushing " << payload << "\n";
    msg_queue_.push(make_pair(isn_, msg));
    isn_ = isn_ + msg.sequence_length();
    seq_num_in_flight_ += msg.sequence_length();
  }

  if ((!fin_) && outbound_stream.is_finished() && isn_ < window_) {
    payload = "";
     msg = {
      isn_,
      false,
      payload,
      true
    };
    // cout << "pushing FIN \n";
    msg_queue_.push(make_pair(isn_, msg));
    seq_num_in_flight_ += msg.sequence_length();
    isn_ = isn_ + msg.sequence_length();
    fin_ = true;
  }
  
}

TCPSenderMessage TCPSender::send_empty_message()
{
  // Your code here.
  // cout << "empty message \n";
  string payload = "";
  TCPSenderMessage msg = {
      isn_,
      false,
      payload,
      false
    };
  // isn_ = isn_ + 1;
  return msg;
}

void TCPSender::receive( const TCPReceiverMessage& msg )
{
  // Your code here.
  vector<Wrap32> remove;
  bool acked = false;
  if (msg.ackno) {
    auto ackno = *std::move(msg.ackno);
    if (isn_ < ackno) {
      cout << "ack failed \n";
      return;
    }
    while ((!retransmit_queue_.empty()) && retransmit_queue_.top().first + retransmit_queue_.top().second.sequence_length() < (ackno+1)) {
      seq_num_in_flight_ -= retransmit_queue_.top().second.sequence_length();
      retransmit_queue_.pop();
      acked = true;
    }
    if (msg.window_size == 0) {
      window_ = ackno + 1;
      zero_window_ = true;
    } else {
      window_ = ackno + msg.window_size;
      zero_window_ = false;
    }
  } else {
    if (msg.window_size == 0) {
      window_ = isn_ + 1;
      zero_window_ = true;
    } else {
      window_ = isn_ + msg.window_size;
      zero_window_ = false;
    }
  }

  if (acked) {
    rto_ = initial_RTO_ms_;
    retrans_timer_ = -1;
    retransmit_ = false;
    cons_retrans_ = 0;
    if (!retransmit_queue_.empty()) {
      retrans_timer_ = cur_time_;
    }
  }
  
}

void TCPSender::tick( const size_t ms_since_last_tick )
{
  // Your code here.
  // cout << "1 - retrans " << retrans_timer_ << " curtime " << cur_time_ << " rto " << rto_ << " window " << window_ << " isn " << isn_ <<  "\n";
  (void)ms_since_last_tick;
  cur_time_ += ms_since_last_tick;
  if (retrans_timer_ > 0  && retrans_timer_ + rto_ <= cur_time_) {
    retransmit_ = true;
    if (((!zero_window_) && isn_ < window_ + 1)) {
        rto_ *= 2;
        cons_retrans_ += 1;
        // retransmit_ = false;
    }
    retrans_timer_ = cur_time_;
  } else {
    retransmit_ = false;
  }
  
  // cout << "2 - retrans " << retrans_timer_ << " curtime " << cur_time_ << " rto " << rto_ << " window " << window_ << " cons retrans " << cons_retrans_ << "\n";
}
