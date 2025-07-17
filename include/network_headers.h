#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <array>
#include <memory>

namespace cppscapy {

// Forward declarations
class EthernetHeader;
class IPv4Header;
class IPv6Header;
class TCPHeader;
class UDPHeader;
class ICMPHeader;

// MAC Address utility class
class MacAddress {
public:
    MacAddress() = default;
    MacAddress(const std::string& mac_str);
    MacAddress(const std::array<uint8_t, 6>& bytes);
    MacAddress(uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4, uint8_t b5, uint8_t b6);
    
    std::string to_string() const;
    std::array<uint8_t, 6> to_bytes() const;
    bool is_broadcast() const;
    bool is_multicast() const;
    
    static MacAddress broadcast();
    static MacAddress multicast_ipv4();
    static MacAddress multicast_ipv6();
    
private:
    std::array<uint8_t, 6> bytes_ = {0};
};

// IPv4 Address utility class
class IPv4Address {
public:
    IPv4Address() = default;
    IPv4Address(const std::string& ip_str);
    IPv4Address(uint32_t ip);
    IPv4Address(uint8_t a, uint8_t b, uint8_t c, uint8_t d);
    
    std::string to_string() const;
    uint32_t to_uint32() const;
    std::array<uint8_t, 4> to_bytes() const;
    
    static IPv4Address localhost();
    static IPv4Address broadcast();
    static IPv4Address any();
    
private:
    uint32_t ip_ = 0;
};

// IPv6 Address utility class
class IPv6Address {
public:
    IPv6Address() = default;
    IPv6Address(const std::string& ip_str);
    IPv6Address(const std::array<uint8_t, 16>& bytes);
    
    std::string to_string() const;
    std::array<uint8_t, 16> to_bytes() const;
    
    static IPv6Address localhost();
    static IPv6Address any();
    
private:
    std::array<uint8_t, 16> bytes_ = {0};
};

// Ethernet Header
class EthernetHeader {
public:
    static constexpr size_t SIZE = 14;
    
    EthernetHeader() = default;
    EthernetHeader(const MacAddress& dst, const MacAddress& src, uint16_t ethertype);
    
    EthernetHeader& dst(const MacAddress& mac);
    EthernetHeader& src(const MacAddress& mac);
    EthernetHeader& ethertype(uint16_t type);
    
    MacAddress dst() const { return dst_; }
    MacAddress src() const { return src_; }
    uint16_t ethertype() const { return ethertype_; }
    
    std::vector<uint8_t> to_bytes() const;
    
    // Common ethertypes
    static constexpr uint16_t ETHERTYPE_IPV4 = 0x0800;
    static constexpr uint16_t ETHERTYPE_IPV6 = 0x86DD;
    static constexpr uint16_t ETHERTYPE_ARP = 0x0806;
    
private:
    MacAddress dst_;
    MacAddress src_;
    uint16_t ethertype_ = 0;
};

// IPv4 Header
class IPv4Header {
public:
    static constexpr size_t MIN_SIZE = 20;
    
    IPv4Header() = default;
    IPv4Header(const IPv4Address& src, const IPv4Address& dst, uint8_t protocol);
    
    IPv4Header& version(uint8_t ver) { version_ = ver; return *this; }
    IPv4Header& ihl(uint8_t ihl) { ihl_ = ihl; return *this; }
    IPv4Header& tos(uint8_t tos) { tos_ = tos; return *this; }
    IPv4Header& length(uint16_t len) { length_ = len; return *this; }
    IPv4Header& id(uint16_t id) { id_ = id; return *this; }
    IPv4Header& flags(uint8_t flags) { flags_ = flags; return *this; }
    IPv4Header& fragment_offset(uint16_t offset) { fragment_offset_ = offset; return *this; }
    IPv4Header& ttl(uint8_t ttl) { ttl_ = ttl; return *this; }
    IPv4Header& protocol(uint8_t proto) { protocol_ = proto; return *this; }
    IPv4Header& src(const IPv4Address& addr) { src_ = addr; return *this; }
    IPv4Header& dst(const IPv4Address& addr) { dst_ = addr; return *this; }
    
    uint8_t version() const { return version_; }
    uint8_t ihl() const { return ihl_; }
    uint8_t tos() const { return tos_; }
    uint16_t length() const { return length_; }
    uint16_t id() const { return id_; }
    uint8_t flags() const { return flags_; }
    uint16_t fragment_offset() const { return fragment_offset_; }
    uint8_t ttl() const { return ttl_; }
    uint8_t protocol() const { return protocol_; }
    IPv4Address src() const { return src_; }
    IPv4Address dst() const { return dst_; }
    
    std::vector<uint8_t> to_bytes() const;
    
    // Common protocols
    static constexpr uint8_t PROTOCOL_ICMP = 1;
    static constexpr uint8_t PROTOCOL_TCP = 6;
    static constexpr uint8_t PROTOCOL_UDP = 17;
    
    // Common flags
    static constexpr uint8_t FLAG_DONT_FRAGMENT = 0x40;
    static constexpr uint8_t FLAG_MORE_FRAGMENTS = 0x20;
    
private:
    uint8_t version_ = 4;
    uint8_t ihl_ = 5;
    uint8_t tos_ = 0;
    uint16_t length_ = 0;
    uint16_t id_ = 0;
    uint8_t flags_ = 0;
    uint16_t fragment_offset_ = 0;
    uint8_t ttl_ = 64;
    uint8_t protocol_ = 0;
    uint16_t checksum_ = 0;
    IPv4Address src_;
    IPv4Address dst_;
};

// IPv6 Header
class IPv6Header {
public:
    static constexpr size_t SIZE = 40;
    
    IPv6Header() = default;
    IPv6Header(const IPv6Address& src, const IPv6Address& dst, uint8_t next_header);
    
    IPv6Header& version(uint8_t ver) { version_ = ver; return *this; }
    IPv6Header& traffic_class(uint8_t tc) { traffic_class_ = tc; return *this; }
    IPv6Header& flow_label(uint32_t fl) { flow_label_ = fl; return *this; }
    IPv6Header& payload_length(uint16_t len) { payload_length_ = len; return *this; }
    IPv6Header& next_header(uint8_t nh) { next_header_ = nh; return *this; }
    IPv6Header& hop_limit(uint8_t hl) { hop_limit_ = hl; return *this; }
    IPv6Header& src(const IPv6Address& addr) { src_ = addr; return *this; }
    IPv6Header& dst(const IPv6Address& addr) { dst_ = addr; return *this; }
    
    uint8_t version() const { return version_; }
    uint8_t traffic_class() const { return traffic_class_; }
    uint32_t flow_label() const { return flow_label_; }
    uint16_t payload_length() const { return payload_length_; }
    uint8_t next_header() const { return next_header_; }
    uint8_t hop_limit() const { return hop_limit_; }
    IPv6Address src() const { return src_; }
    IPv6Address dst() const { return dst_; }
    
    std::vector<uint8_t> to_bytes() const;
    
    // Common next headers (same as IPv4 protocols)
    static constexpr uint8_t NEXT_HEADER_TCP = 6;
    static constexpr uint8_t NEXT_HEADER_UDP = 17;
    static constexpr uint8_t NEXT_HEADER_ICMPV6 = 58;
    
private:
    uint8_t version_ = 6;
    uint8_t traffic_class_ = 0;
    uint32_t flow_label_ = 0;
    uint16_t payload_length_ = 0;
    uint8_t next_header_ = 0;
    uint8_t hop_limit_ = 64;
    IPv6Address src_;
    IPv6Address dst_;
};

// TCP Header
class TCPHeader {
public:
    static constexpr size_t MIN_SIZE = 20;
    
    TCPHeader() = default;
    TCPHeader(uint16_t src_port, uint16_t dst_port);
    
    TCPHeader& src_port(uint16_t port) { src_port_ = port; return *this; }
    TCPHeader& dst_port(uint16_t port) { dst_port_ = port; return *this; }
    TCPHeader& seq_num(uint32_t seq) { seq_num_ = seq; return *this; }
    TCPHeader& ack_num(uint32_t ack) { ack_num_ = ack; return *this; }
    TCPHeader& data_offset(uint8_t offset) { data_offset_ = offset; return *this; }
    TCPHeader& flags(uint8_t flags) { flags_ = flags; return *this; }
    TCPHeader& window_size(uint16_t size) { window_size_ = size; return *this; }
    TCPHeader& urgent_ptr(uint16_t ptr) { urgent_ptr_ = ptr; return *this; }
    
    uint16_t src_port() const { return src_port_; }
    uint16_t dst_port() const { return dst_port_; }
    uint32_t seq_num() const { return seq_num_; }
    uint32_t ack_num() const { return ack_num_; }
    uint8_t data_offset() const { return data_offset_; }
    uint8_t flags() const { return flags_; }
    uint16_t window_size() const { return window_size_; }
    uint16_t urgent_ptr() const { return urgent_ptr_; }
    
    std::vector<uint8_t> to_bytes() const;
    
    // TCP flags
    static constexpr uint8_t FLAG_FIN = 0x01;
    static constexpr uint8_t FLAG_SYN = 0x02;
    static constexpr uint8_t FLAG_RST = 0x04;
    static constexpr uint8_t FLAG_PSH = 0x08;
    static constexpr uint8_t FLAG_ACK = 0x10;
    static constexpr uint8_t FLAG_URG = 0x20;
    static constexpr uint8_t FLAG_ECE = 0x40;
    static constexpr uint8_t FLAG_CWR = 0x80;
    
private:
    uint16_t src_port_ = 0;
    uint16_t dst_port_ = 0;
    uint32_t seq_num_ = 0;
    uint32_t ack_num_ = 0;
    uint8_t data_offset_ = 5;
    uint8_t flags_ = 0;
    uint16_t window_size_ = 8192;
    uint16_t checksum_ = 0;
    uint16_t urgent_ptr_ = 0;
};

// UDP Header
class UDPHeader {
public:
    static constexpr size_t SIZE = 8;
    
    UDPHeader() = default;
    UDPHeader(uint16_t src_port, uint16_t dst_port, uint16_t length = 0);
    
    UDPHeader& src_port(uint16_t port) { src_port_ = port; return *this; }
    UDPHeader& dst_port(uint16_t port) { dst_port_ = port; return *this; }
    UDPHeader& length(uint16_t len) { length_ = len; return *this; }
    
    uint16_t src_port() const { return src_port_; }
    uint16_t dst_port() const { return dst_port_; }
    uint16_t length() const { return length_; }
    
    std::vector<uint8_t> to_bytes() const;
    
private:
    uint16_t src_port_ = 0;
    uint16_t dst_port_ = 0;
    uint16_t length_ = 0;
    uint16_t checksum_ = 0;
};

// ICMP Header
class ICMPHeader {
public:
    static constexpr size_t MIN_SIZE = 8;
    
    ICMPHeader() = default;
    ICMPHeader(uint8_t type, uint8_t code);
    
    ICMPHeader& type(uint8_t type) { type_ = type; return *this; }
    ICMPHeader& code(uint8_t code) { code_ = code; return *this; }
    ICMPHeader& identifier(uint16_t id) { identifier_ = id; return *this; }
    ICMPHeader& sequence(uint16_t seq) { sequence_ = seq; return *this; }
    
    uint8_t type() const { return type_; }
    uint8_t code() const { return code_; }
    uint16_t identifier() const { return identifier_; }
    uint16_t sequence() const { return sequence_; }
    
    std::vector<uint8_t> to_bytes() const;
    
    // ICMP types
    static constexpr uint8_t TYPE_ECHO_REPLY = 0;
    static constexpr uint8_t TYPE_ECHO_REQUEST = 8;
    static constexpr uint8_t TYPE_DEST_UNREACHABLE = 3;
    static constexpr uint8_t TYPE_TIME_EXCEEDED = 11;
    
private:
    uint8_t type_ = 0;
    uint8_t code_ = 0;
    uint16_t checksum_ = 0;
    uint16_t identifier_ = 0;
    uint16_t sequence_ = 0;
};

// Packet builder for combining headers
class PacketBuilder {
public:
    PacketBuilder() = default;
    
    PacketBuilder& ethernet(const EthernetHeader& eth);
    PacketBuilder& ipv4(const IPv4Header& ip);
    PacketBuilder& ipv6(const IPv6Header& ip);
    PacketBuilder& tcp(const TCPHeader& tcp);
    PacketBuilder& udp(const UDPHeader& udp);
    PacketBuilder& icmp(const ICMPHeader& icmp);
    PacketBuilder& payload(const std::vector<uint8_t>& data);
    PacketBuilder& payload(const std::string& data);
    
    std::vector<uint8_t> build() const;
    
private:
    std::vector<uint8_t> packet_;
};

// Utility functions for common patterns
namespace patterns {
    // Create a simple IPv4 packet
    std::vector<uint8_t> ipv4_packet(
        const IPv4Address& src, const IPv4Address& dst, 
        uint8_t protocol, const std::vector<uint8_t>& payload = {});
    
    // Create a simple IPv6 packet
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

} // namespace cppscapy
