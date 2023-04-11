#include <stdexcept>

#include "byte_stream.hh"

using namespace std;

ByteStream::ByteStream( uint64_t capacity )
  : capacity_( capacity ), closed_( false ), error_( false ), bytes_pushed_( 0 ), bytes_popped_( 0 ), buffer_()
{}

void Writer::push( string data )
{
  // Your code here.
  for ( size_t i = 0; i < data.length(); i++ ) {
    if ( capacity_ == 0 )
      return;
    buffer_.push( data[i] );
    capacity_--;
    bytes_pushed_++;
  }
}

void Writer::close()
{
  // Your code here.
  closed_ = true;
}

void Writer::set_error()
{
  // Your code here.
  error_ = true;
}

bool Writer::is_closed() const
{
  // Your code here.
  return closed_;
}

uint64_t Writer::available_capacity() const
{
  // Your code here.
  return capacity_;
}

uint64_t Writer::bytes_pushed() const
{
  // Your code here.
  return bytes_pushed_;
}

string_view Reader::peek() const
{
  // Your code here.
  return string_view( &( buffer_.front() ), 1 );
}

bool Reader::is_finished() const
{
  // Your code here.
  return closed_ && ( bytes_pushed_ == bytes_popped_ );
}

bool Reader::has_error() const
{
  // Your code here.
  return error_;
}

void Reader::pop( uint64_t len )
{
  // Your code here.
  for ( uint64_t i = 0; i < len; i++ ) {
    buffer_.pop();
    capacity_++;
    bytes_popped_++;
  }
}

uint64_t Reader::bytes_buffered() const
{
  // Your code here.
  return bytes_pushed_ - bytes_popped_;
}

uint64_t Reader::bytes_popped() const
{
  // Your code here.
  return bytes_popped_;
}
