#include "wrapping_integers.hh"
#include <iostream>

using namespace std;

Wrap32 Wrap32::wrap( uint64_t n, Wrap32 zero_point )
{
  // Your code here.
  return Wrap32 { static_cast<uint32_t>(static_cast<uint64_t>(zero_point.raw_value_) + static_cast<uint64_t>(n)) };
}

uint64_t Wrap32::unwrap( Wrap32 zero_point, uint64_t checkpoint ) const
{
  // Your code here.
  uint64_t raw = static_cast<uint64_t>(static_cast<uint32_t>(static_cast<uint64_t>(raw_value_) + (1UL << 32) - static_cast<uint64_t>(zero_point.raw_value_)));
  uint64_t X = (checkpoint & (0xFFFFFFFFUL << 32));
  uint64_t Y = checkpoint & 0xFFFFFFFFUL;
  uint64_t MAX32 = (1UL << 32);
  if (checkpoint < (1UL << 31)) {
    return raw;
  } else {
    uint64_t val1 = abs(static_cast<long long int>(Y) - static_cast<long long int>(raw) - static_cast<long long int>(MAX32));
    uint64_t val2 = abs(static_cast<long long int>(Y) - static_cast<long long int>(raw));
    uint64_t val3 = abs(static_cast<long long int>(Y) - static_cast<long long int>(raw) + static_cast<long long int>(MAX32));

    if (val1 <= val2 && val1 <= val3) {
      return X + raw + MAX32;
    }
    if (val2 <= val1 && val2 <= val3) {
      return X + raw;
    }
    if (val3 <= val2 && val3 <= val1) {
      return X + raw - MAX32;
    }
    return X + raw;
  } 
}
