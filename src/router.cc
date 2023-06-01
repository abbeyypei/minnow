#include "router.hh"

#include <iostream>
#include <limits>
#include <cmath>

using namespace std;

// route_prefix: The "up-to-32-bit" IPv4 address prefix to match the datagram's destination address against
// prefix_length: For this route to be applicable, how many high-order (most-significant) bits of
//    the route_prefix will need to match the corresponding bits of the datagram's destination address?
// next_hop: The IP address of the next hop. Will be empty if the network is directly attached to the router (in
//    which case, the next hop address should be the datagram's final destination).
// interface_num: The index of the interface to send the datagram out on.
void Router::add_route( const uint32_t route_prefix,
                        const uint8_t prefix_length,
                        const optional<Address> next_hop,
                        const size_t interface_num )
{
  cerr << "DEBUG: adding route " << Address::from_ipv4_numeric( route_prefix ).ip() << "/"
       << static_cast<int>( prefix_length ) << " => " << ( next_hop.has_value() ? next_hop->ip() : "(direct)" )
       << " on interface " << interface_num << "\n";

  router_table_.push_back({
    route_prefix,
    prefix_length,
    next_hop,
    interface_num,
  });

}

void Router::route() {
  for(size_t i = 0; i < interfaces_.size(); i++) {
    const auto msg = interface(i).maybe_receive();
    if ( msg.has_value() ) {
      InternetDatagram datagram = msg.value();
      uint32_t dst_addr = datagram.header.dst;
      if (datagram.header.ttl > 1) {
        datagram.header.ttl --;
        datagram.header.compute_checksum();

        int8_t max_prefix_len = -1;
        struct RoutingInfo max_route = {0, 0, {}, 0};
        for (struct RoutingInfo r_info : router_table_) {
          if (r_info.prefix_length == 0 && max_prefix_len < static_cast<int8_t>(r_info.prefix_length)) {
            max_prefix_len = 0;
            max_route = r_info;
          } else {
            if (dst_addr / static_cast<uint32_t>(pow(2, (32 - r_info.prefix_length))) == (r_info.route_prefix / static_cast<uint32_t>(pow(2, (32 - r_info.prefix_length))))) {
              if (max_prefix_len < static_cast<int8_t>(r_info.prefix_length)) {
                max_prefix_len = static_cast<int8_t>(r_info.prefix_length);
                max_route = r_info;
              }
            }
          }
        }

        if (max_prefix_len >= 0) {
          if (max_route.next_hop.has_value()) {
            Address dst = max_route.next_hop.value();
            interface(max_route.interface_num).send_datagram(datagram, dst);
          } else {
            interface(max_route.interface_num).send_datagram(datagram, Address::from_ipv4_numeric(dst_addr));
          }
          
        }
      }


    }

  }
}
