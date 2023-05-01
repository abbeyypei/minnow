#pragma once

#include "byte_stream.hh"
#include "tcp_receiver_message.hh"
#include "tcp_sender_message.hh"
#include <map>
#include <queue>

typedef std::pair<Wrap32, TCPSenderMessage> ql;

class QlCompare
{
public:
  bool operator() (const ql& lhs, const ql& rhs) const
  {
    return rhs.first < lhs.first;
  }
};

class Wrap32Compare
{
public:
  bool operator() (const Wrap32& lhs, const Wrap32& rhs) const
  {
    return lhs < rhs;
  }
};

class TCPSender
{
  Wrap32 isn_;
  uint64_t initial_RTO_ms_;
  Wrap32 window_;
  uint64_t cur_time_;
  uint64_t rto_;
  int64_t retrans_timer_;
  uint64_t cons_retrans_;
  uint64_t seq_num_in_flight_;
  bool syn_;
  bool fin_;
  bool retransmit_;
  bool zero_window_;
  std::priority_queue<ql, std::vector<ql>, QlCompare> msg_queue_;
  std::priority_queue<ql, std::vector<ql>, QlCompare> retransmit_queue_;


public:
  /* Construct TCP sender with given default Retransmission Timeout and possible ISN */
  TCPSender( uint64_t initial_RTO_ms, std::optional<Wrap32> fixed_isn );

  /* Push bytes from the outbound stream */
  void push( Reader& outbound_stream );

  /* Send a TCPSenderMessage if needed (or empty optional otherwise) */
  std::optional<TCPSenderMessage> maybe_send();

  /* Generate an empty TCPSenderMessage */
  TCPSenderMessage send_empty_message();

  /* Receive an act on a TCPReceiverMessage from the peer's receiver */
  void receive( const TCPReceiverMessage& msg );

  /* Time has passed by the given # of milliseconds since the last time the tick() method was called. */
  void tick( uint64_t ms_since_last_tick );

  /* Accessors for use in testing */
  uint64_t sequence_numbers_in_flight() const;  // How many sequence numbers are outstanding?
  uint64_t consecutive_retransmissions() const; // How many consecutive *re*transmissions have happened?
};
