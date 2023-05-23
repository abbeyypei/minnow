#include "network_interface.hh"

#include "arp_message.hh"
#include "ethernet_frame.hh"

using namespace std;

constexpr size_t MAPPING_EXPIRE_TIME = 30000;
constexpr size_t ARP_EXPIRE_TIME = 5000;

// ethernet_address: Ethernet (what ARP calls "hardware") address of the interface
// ip_address: IP (what ARP calls "protocol") address of the interface
NetworkInterface::NetworkInterface( const EthernetAddress& ethernet_address, const Address& ip_address )
  : ethernet_address_( ethernet_address ), ip_address_( ip_address ), 
    address_mapping_( {} ), mapping_queue_(), arp_mapping_( {} ), 
    arp_queue_(), send_queue_(), time_(0)
{
  cerr << "DEBUG: Network interface has Ethernet address " << to_string( ethernet_address_ ) << " and IP address "
       << ip_address.ip() << "\n";
}

// dgram: the IPv4 datagram to be sent
// next_hop: the IP address of the interface to send it to (typically a router or default gateway, but
// may also be another host if directly connected to the same network as the destination)

// Note: the Address type can be converted to a uint32_t (raw 32-bit IP address) by using the
// Address::ipv4_numeric() method.
void NetworkInterface::send_datagram( const InternetDatagram& dgram, const Address& next_hop )
{
  uint32_t ip = next_hop.ipv4_numeric();
  Serializer ser = Serializer();
  dgram.serialize(ser);

  if (address_mapping_.find(ip) == address_mapping_.end()) {
    if (arp_mapping_.find(ip) == arp_mapping_.end()) {
      // sending ARP rqst
      ARPMessage msg = {
        .opcode = ARPMessage::OPCODE_REQUEST,
        .sender_ethernet_address = ethernet_address_,
        .sender_ip_address = ip_address_.ipv4_numeric(),
        .target_ip_address = ip,
      };

      EthernetFrame arp = {
        .header = {
          .dst = ETHERNET_BROADCAST,
          .src = ethernet_address_,
          .type = EthernetHeader::TYPE_ARP,
        },
        .payload = serialize(msg),
      };

      EthernetFrame datagram = {
        .header = {
          .dst = 0,
          .src = ethernet_address_,
          .type = EthernetHeader::TYPE_IPv4,
        },
        .payload = ser.output(),
      };

      arp_mapping_[ip] = datagram;
      arp_queue_.push(make_pair(ip, -1));
      send_queue_.push(arp);
    }
  } else {
    EthernetAddress e_addr = address_mapping_[ip];
    EthernetFrame frame = {
      .header = {
        .dst = e_addr,
        .src = ethernet_address_,
        .type = EthernetHeader::TYPE_IPv4,
      },
      .payload = ser.output(),
    };
    send_queue_.push(frame);
  }

}

// frame: the incoming Ethernet frame
optional<InternetDatagram> NetworkInterface::recv_frame( const EthernetFrame& frame )
{
  // cout << frame.header.to_string() << endl;
  if (!(frame.header.dst == ETHERNET_BROADCAST || frame.header.dst == ethernet_address_)) {
    // cout << frame.header.to_string() << endl;
    // cout << "recv not valid \n";
    return {};
  }
  if (frame.header.type == EthernetHeader::TYPE_IPv4) {
    InternetDatagram datagram = InternetDatagram();
    if (parse(datagram, frame.payload)) {
      return datagram;
    }
  }
  if (frame.header.type == EthernetHeader::TYPE_ARP) {
    ARPMessage msg = ARPMessage();
    if (parse(msg, frame.payload)) {
      address_mapping_[msg.sender_ip_address] = msg.sender_ethernet_address;
      mapping_queue_.push(make_pair(msg.sender_ip_address, time_));

      EthernetFrame e_reply;
      if (msg.opcode == ARPMessage::OPCODE_REQUEST) {
        // cout << "arp msg rqst " << msg.to_string() << endl;
        if (msg.target_ip_address == ip_address_.ipv4_numeric()) {
          ARPMessage reply = {
            .opcode = ARPMessage::OPCODE_REPLY,
            .sender_ethernet_address = ethernet_address_,
            .sender_ip_address = ip_address_.ipv4_numeric(),
            .target_ethernet_address = msg.sender_ethernet_address,
            .target_ip_address = msg.sender_ip_address,
          };

          e_reply = {
            .header = {
              .dst = msg.sender_ethernet_address,
              .src = ethernet_address_,
              .type = EthernetHeader::TYPE_ARP,
            },
            .payload = serialize(reply),
          };
          send_queue_.push(e_reply);
          
        }
      } else {
        e_reply = arp_mapping_[msg.sender_ip_address];
        e_reply.header.dst = msg.sender_ethernet_address;

        arp_mapping_.erase(msg.sender_ip_address);
        send_queue_.push(e_reply);

      }

      
    }
  }
  return {};
}

// ms_since_last_tick: the number of milliseconds since the last call to this method
void NetworkInterface::tick( const size_t ms_since_last_tick )
{
  time_ += ms_since_last_tick;
  size_t map_time, arp_time;
  uint32_t ip_addr;

  while ((!mapping_queue_.empty())) {
    map_time = get<1>(mapping_queue_.front());
    if (map_time + MAPPING_EXPIRE_TIME <= time_) {
      ip_addr = get<0>(mapping_queue_.front());
      mapping_queue_.pop();
      address_mapping_.erase(ip_addr);
    } else {
      goto jmp;
    }
  }
  jmp:
  while (!arp_queue_.empty()) {
    arp_time = get<1>(arp_queue_.front());
    if (arp_time + ARP_EXPIRE_TIME <= time_) {
      ip_addr = get<0>(arp_queue_.front());
      arp_queue_.pop();
      if (arp_mapping_.find(ip_addr) != arp_mapping_.end()) {
        ARPMessage msg = {
        .opcode = ARPMessage::OPCODE_REQUEST,
        .sender_ethernet_address = ethernet_address_,
        .sender_ip_address = ip_address_.ipv4_numeric(),
        .target_ip_address = ip_addr,
      };

        EthernetFrame arp_rqst = {
          .header = {
            .dst = ETHERNET_BROADCAST,
            .src = ethernet_address_,
            .type = EthernetHeader::TYPE_ARP,
          },
          .payload = serialize(msg),
        };
        send_queue_.push(arp_rqst);
        arp_queue_.push(make_pair(ip_addr, time_));
      }
    } else {
      return;
    }
  }
}

optional<EthernetFrame> NetworkInterface::maybe_send()
{
  if (!send_queue_.empty()) {
    EthernetFrame send = send_queue_.front();
    send_queue_.pop();
    return send;
  }
  return {};
}
