# CppScapy API Reference

## Complete API Summary

### Core Classes

#### MacAddress
```cpp
// Constructors
MacAddress(const std::string& mac_str);           // "aa:bb:cc:dd:ee:ff"
MacAddress(const std::array<uint8_t, 6>& bytes);
MacAddress(uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4, uint8_t b5, uint8_t b6);

// Methods
std::string to_string() const;
std::array<uint8_t, 6> to_bytes() const;
bool is_broadcast() const;
bool is_multicast() const;

// Static methods
static MacAddress broadcast();
static MacAddress multicast_ipv4();
static MacAddress multicast_ipv6();
```

#### IPv4Address
```cpp
// Constructors
IPv4Address(const std::string& ip_str);     // "192.168.1.1"
IPv4Address(uint32_t ip);
IPv4Address(uint8_t a, uint8_t b, uint8_t c, uint8_t d);

// Methods
std::string to_string() const;
uint32_t to_uint32() const;
std::array<uint8_t, 4> to_bytes() const;

// Static methods
static IPv4Address localhost();    // 127.0.0.1
static IPv4Address broadcast();    // 255.255.255.255
static IPv4Address any();          // 0.0.0.0
```

#### IPv6Address
```cpp
// Constructors
IPv6Address(const std::string& ip_str);     // "2001:db8::1"
IPv6Address(const std::array<uint8_t, 16>& bytes);

// Methods
std::string to_string() const;
std::array<uint8_t, 16> to_bytes() const;

// Static methods
static IPv6Address localhost();    // ::1
static IPv6Address any();          // ::
```

#### EthernetHeader
```cpp
// Constructor
EthernetHeader(const MacAddress& dst, const MacAddress& src, uint16_t ethertype);

// Fluent API
EthernetHeader& dst(const MacAddress& mac);
EthernetHeader& src(const MacAddress& mac);
EthernetHeader& ethertype(uint16_t type);

// Getters
MacAddress dst() const;
MacAddress src() const;
uint16_t ethertype() const;

// Serialization
std::vector<uint8_t> to_bytes() const;

// Constants
static constexpr uint16_t ETHERTYPE_IPV4 = 0x0800;
static constexpr uint16_t ETHERTYPE_IPV6 = 0x86DD;
static constexpr uint16_t ETHERTYPE_ARP = 0x0806;
```

#### IPv4Header
```cpp
// Constructor
IPv4Header(const IPv4Address& src, const IPv4Address& dst, uint8_t protocol);

// Fluent API
IPv4Header& version(uint8_t ver);
IPv4Header& ihl(uint8_t ihl);
IPv4Header& tos(uint8_t tos);
IPv4Header& length(uint16_t len);
IPv4Header& id(uint16_t id);
IPv4Header& flags(uint8_t flags);
IPv4Header& fragment_offset(uint16_t offset);
IPv4Header& ttl(uint8_t ttl);
IPv4Header& protocol(uint8_t proto);
IPv4Header& src(const IPv4Address& addr);
IPv4Header& dst(const IPv4Address& addr);

// Serialization
std::vector<uint8_t> to_bytes() const;

// Constants
static constexpr uint8_t PROTOCOL_ICMP = 1;
static constexpr uint8_t PROTOCOL_TCP = 6;
static constexpr uint8_t PROTOCOL_UDP = 17;
static constexpr uint8_t FLAG_DONT_FRAGMENT = 0x40;
static constexpr uint8_t FLAG_MORE_FRAGMENTS = 0x20;
```

#### IPv6Header
```cpp
// Constructor
IPv6Header(const IPv6Address& src, const IPv6Address& dst, uint8_t next_header);

// Fluent API
IPv6Header& version(uint8_t ver);
IPv6Header& traffic_class(uint8_t tc);
IPv6Header& flow_label(uint32_t fl);
IPv6Header& payload_length(uint16_t len);
IPv6Header& next_header(uint8_t nh);
IPv6Header& hop_limit(uint8_t hl);
IPv6Header& src(const IPv6Address& addr);
IPv6Header& dst(const IPv6Address& addr);

// Serialization
std::vector<uint8_t> to_bytes() const;

// Constants
static constexpr uint8_t NEXT_HEADER_TCP = 6;
static constexpr uint8_t NEXT_HEADER_UDP = 17;
static constexpr uint8_t NEXT_HEADER_ICMPV6 = 58;
```

#### TCPHeader
```cpp
// Constructor
TCPHeader(uint16_t src_port, uint16_t dst_port);

// Fluent API
TCPHeader& src_port(uint16_t port);
TCPHeader& dst_port(uint16_t port);
TCPHeader& seq_num(uint32_t seq);
TCPHeader& ack_num(uint32_t ack);
TCPHeader& data_offset(uint8_t offset);
TCPHeader& flags(uint8_t flags);
TCPHeader& window_size(uint16_t size);
TCPHeader& urgent_ptr(uint16_t ptr);

// Serialization
std::vector<uint8_t> to_bytes() const;

// Constants
static constexpr uint8_t FLAG_FIN = 0x01;
static constexpr uint8_t FLAG_SYN = 0x02;
static constexpr uint8_t FLAG_RST = 0x04;
static constexpr uint8_t FLAG_PSH = 0x08;
static constexpr uint8_t FLAG_ACK = 0x10;
static constexpr uint8_t FLAG_URG = 0x20;
static constexpr uint8_t FLAG_ECE = 0x40;
static constexpr uint8_t FLAG_CWR = 0x80;
```

#### UDPHeader
```cpp
// Constructor
UDPHeader(uint16_t src_port, uint16_t dst_port, uint16_t length = 0);

// Fluent API
UDPHeader& src_port(uint16_t port);
UDPHeader& dst_port(uint16_t port);
UDPHeader& length(uint16_t len);

// Serialization
std::vector<uint8_t> to_bytes() const;
```

#### ICMPHeader
```cpp
// Constructor
ICMPHeader(uint8_t type, uint8_t code);

// Fluent API
ICMPHeader& type(uint8_t type);
ICMPHeader& code(uint8_t code);
ICMPHeader& identifier(uint16_t id);
ICMPHeader& sequence(uint16_t seq);

// Serialization
std::vector<uint8_t> to_bytes() const;

// Constants
static constexpr uint8_t TYPE_ECHO_REPLY = 0;
static constexpr uint8_t TYPE_ECHO_REQUEST = 8;
static constexpr uint8_t TYPE_DEST_UNREACHABLE = 3;
static constexpr uint8_t TYPE_TIME_EXCEEDED = 11;
```

#### PacketBuilder
```cpp
// Fluent API for building packets
PacketBuilder& ethernet(const EthernetHeader& eth);
PacketBuilder& ipv4(const IPv4Header& ip);
PacketBuilder& ipv6(const IPv6Header& ip);
PacketBuilder& tcp(const TCPHeader& tcp);
PacketBuilder& udp(const UDPHeader& udp);
PacketBuilder& icmp(const ICMPHeader& icmp);
PacketBuilder& payload(const std::vector<uint8_t>& data);
PacketBuilder& payload(const std::string& data);

// Build final packet
std::vector<uint8_t> build() const;
```

### Convenience Patterns

```cpp
namespace patterns {
    // Create IPv4 packet
    std::vector<uint8_t> ipv4_packet(
        const IPv4Address& src, const IPv4Address& dst, 
        uint8_t protocol, const std::vector<uint8_t>& payload = {});
    
    // Create IPv6 packet
    std::vector<uint8_t> ipv6_packet(
        const IPv6Address& src, const IPv6Address& dst, 
        uint8_t next_header, const std::vector<uint8_t>& payload = {});
    
    // Create TCP SYN packet
    std::vector<uint8_t> tcp_syn(
        const IPv4Address& src_ip, const IPv4Address& dst_ip,
        uint16_t src_port, uint16_t dst_port, uint32_t seq_num = 0);
    
    // Create UDP packet
    std::vector<uint8_t> udp_packet(
        const IPv4Address& src_ip, const IPv4Address& dst_ip,
        uint16_t src_port, uint16_t dst_port, 
        const std::vector<uint8_t>& payload = {});
    
    // Create ICMP ping packet
    std::vector<uint8_t> icmp_ping(
        const IPv4Address& src_ip, const IPv4Address& dst_ip,
        uint16_t identifier = 0, uint16_t sequence = 0);
    
    // Create Ethernet frame
    std::vector<uint8_t> ethernet_frame(
        const MacAddress& src_mac, const MacAddress& dst_mac,
        uint16_t ethertype, const std::vector<uint8_t>& payload = {});
}
```

### Example Usage Patterns

#### Simple TCP SYN Packet
```cpp
auto packet = patterns::tcp_syn(
    IPv4Address("192.168.1.100"),
    IPv4Address("192.168.1.1"),
    12345, 80);
```

#### Complex Packet with PacketBuilder
```cpp
PacketBuilder builder;
auto packet = builder
    .ethernet(EthernetHeader(dst_mac, src_mac, EthernetHeader::ETHERTYPE_IPV4))
    .ipv4(IPv4Header(src_ip, dst_ip, IPv4Header::PROTOCOL_TCP)
          .ttl(64).length(total_length))
    .tcp(TCPHeader(src_port, dst_port)
         .flags(TCPHeader::FLAG_SYN | TCPHeader::FLAG_ACK)
         .seq_num(1000))
    .payload("HTTP/1.1 200 OK\r\n\r\n")
    .build();
```

#### UDP DNS Query
```cpp
auto dns_packet = patterns::udp_packet(
    IPv4Address("192.168.1.100"),
    IPv4Address("8.8.8.8"),
    53001, 53,
    dns_query_payload);
```

#### ICMP Ping
```cpp
auto ping = patterns::icmp_ping(
    IPv4Address("192.168.1.100"),
    IPv4Address("8.8.8.8"),
    1, 1);
```

### Build Instructions

```bash
mkdir build && cd build
cmake ..
make

# Run examples
./demo          # Comprehensive demo
./simple_test   # Unit tests
./quick_usage   # Quick usage examples
```

This library provides a complete, type-safe, and convenient API for constructing network packets at various layers of the network stack.
