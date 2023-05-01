#include "tcp_receiver.hh"
#include <iostream>

using namespace std;

void TCPReceiver::receive( TCPSenderMessage message, Reassembler& reassembler, Writer& inbound_stream )
{
  // Your code here.
  if (message.SYN) {
    ISN = message.seqno;
  }
  if (ISN) {
    ackno += message.sequence_length();
    auto isn = *std::move(ISN);
    string msg = message.payload.release();
    reassembler.insert(message.seqno.unwrap(isn, reassembler.bytes_pending()) - str_bytes, msg, message.FIN, inbound_stream);
    re_index = reassembler.current_index() + inbound_stream.is_closed();
  }
  str_bytes += message.SYN + inbound_stream.is_closed();
}

TCPReceiverMessage TCPReceiver::send( const Writer& inbound_stream ) const
{
  // Your code here.
  uint64_t window_size = min(inbound_stream.available_capacity(), static_cast<uint64_t>(UINT16_MAX));
  if (ISN) {
    auto isn = *std::move(ISN);
    return {
      .ackno = Wrap32::wrap(re_index+1, isn),
      .window_size = static_cast<uint16_t>(window_size)
    };
  } else {
    return {
      .window_size = static_cast<uint16_t>(window_size)
    };
  }
}
