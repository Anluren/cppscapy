#include "../include/network_headers.h"
#include <cstring>

namespace cppscapy {

// TCPHeader implementation
TCPHeader::TCPHeader(uint16_t src_port, uint16_t dst_port) 
    : src_port_(src_port), dst_port_(dst_port) {}

std::vector<uint8_t> TCPHeader::to_bytes() const {
    std::vector<uint8_t> result;
    result.reserve(MIN_SIZE);
    
    // Source Port
    result.push_back((src_port_ >> 8) & 0xFF);
    result.push_back(src_port_ & 0xFF);
    
    // Destination Port
    result.push_back((dst_port_ >> 8) & 0xFF);
    result.push_back(dst_port_ & 0xFF);
    
    // Sequence Number
    result.push_back((seq_num_ >> 24) & 0xFF);
    result.push_back((seq_num_ >> 16) & 0xFF);
    result.push_back((seq_num_ >> 8) & 0xFF);
    result.push_back(seq_num_ & 0xFF);
    
    // Ack
    result.push_back((ack_num_ >> 24) & 0xFF);
    result.push_back((ack_num_ >> 16) & 0xFF);
    result.push_back((ack_num_ >> 8) & 0xFF);
    result.push_back(ack_num_ & 0xFF);
    
    // Data Offset and Flags
    result.push_back((data_offset_ << 4) | 0); // Reserved bits are 0
    result.push_back(flags_);
    
    // Window Size
    result.push_back((window_size_ >> 8) & 0xFF);
    result.push_back(window_size_ & 0xFF);
    
    // Checksum (placeholder)
    result.push_back(0);
    result.push_back(0);
    
    // Urgent Pointer
    result.push_back((urgent_ptr_ >> 8) & 0xFF);
    result.push_back(urgent_ptr_ & 0xFF);
    
    return result;
}

// UDPHeader implementation
UDPHeader::UDPHeader(uint16_t src_port, uint16_t dst_port, uint16_t length) 
    : src_port_(src_port), dst_port_(dst_port), length_(length) {}

std::vector<uint8_t> UDPHeader::to_bytes() const {
    std::vector<uint8_t> result;
    result.reserve(SIZE);
    
    // Source Port
    result.push_back((src_port_ >> 8) & 0xFF);
    result.push_back(src_port_ & 0xFF);
    
    // Destination Port
    result.push_back((dst_port_ >> 8) & 0xFF);
    result.push_back(dst_port_ & 0xFF);
    
    // Length
    result.push_back((length_ >> 8) & 0xFF);
    result.push_back(length_ & 0xFF);
    
    // Checksum (placeholder)
    result.push_back(0);
    result.push_back(0);
    
    return result;
}

// ICMPHeader implementation
ICMPHeader::ICMPHeader(uint8_t type, uint8_t code) 
    : type_(type), code_(code) {}

std::vector<uint8_t> ICMPHeader::to_bytes() const {
    std::vector<uint8_t> result;
    result.reserve(MIN_SIZE);
    
    // Type
    result.push_back(type_);
    
    // Code
    result.push_back(code_);
    
    // Checksum (placeholder)
    result.push_back(0);
    result.push_back(0);
    
    // Identifier
    result.push_back((identifier_ >> 8) & 0xFF);
    result.push_back(identifier_ & 0xFF);
    
    // Sequence Number
    result.push_back((sequence_ >> 8) & 0xFF);
    result.push_back(sequence_ & 0xFF);
    
    return result;
}

// PacketBuilder implementation
PacketBuilder& PacketBuilder::ethernet(const EthernetHeader& eth) {
    auto bytes = eth.to_bytes();
    packet_.insert(packet_.end(), bytes.begin(), bytes.end());
    return *this;
}

PacketBuilder& PacketBuilder::ipv4(const IPv4Header& ip) {
    auto bytes = ip.to_bytes();
    packet_.insert(packet_.end(), bytes.begin(), bytes.end());
    return *this;
}

PacketBuilder& PacketBuilder::ipv6(const IPv6Header& ip) {
    auto bytes = ip.to_bytes();
    packet_.insert(packet_.end(), bytes.begin(), bytes.end());
    return *this;
}

PacketBuilder& PacketBuilder::mpls(const MPLSHeader& mpls) {
    auto bytes = mpls.to_bytes();
    packet_.insert(packet_.end(), bytes.begin(), bytes.end());
    return *this;
}

PacketBuilder& PacketBuilder::tcp(const TCPHeader& tcp) {
    auto bytes = tcp.to_bytes();
    packet_.insert(packet_.end(), bytes.begin(), bytes.end());
    return *this;
}

PacketBuilder& PacketBuilder::udp(const UDPHeader& udp) {
    auto bytes = udp.to_bytes();
    packet_.insert(packet_.end(), bytes.begin(), bytes.end());
    return *this;
}

PacketBuilder& PacketBuilder::icmp(const ICMPHeader& icmp) {
    auto bytes = icmp.to_bytes();
    packet_.insert(packet_.end(), bytes.begin(), bytes.end());
    return *this;
}

PacketBuilder& PacketBuilder::payload(const std::vector<uint8_t>& data) {
    packet_.insert(packet_.end(), data.begin(), data.end());
    return *this;
}

PacketBuilder& PacketBuilder::payload(const std::string& data) {
    packet_.insert(packet_.end(), data.begin(), data.end());
    return *this;
}

std::vector<uint8_t> PacketBuilder::build() const {
    return packet_;
}

// Utility patterns implementation
namespace patterns {

std::vector<uint8_t> ipv4_packet(
    const IPv4Address& src, const IPv4Address& dst, 
    uint8_t protocol, const std::vector<uint8_t>& payload) {
    
    IPv4Header ip(src, dst, protocol);
    ip.length(IPv4Header::MIN_SIZE + payload.size());
    
    PacketBuilder builder;
    builder.ipv4(ip).payload(payload);
    
    return builder.build();
}

std::vector<uint8_t> ipv6_packet(
    const IPv6Address& src, const IPv6Address& dst, 
    uint8_t next_header, const std::vector<uint8_t>& payload) {
    
    IPv6Header ip(src, dst, next_header);
    ip.payload_length(payload.size());
    
    PacketBuilder builder;
    builder.ipv6(ip).payload(payload);
    
    return builder.build();
}

std::vector<uint8_t> tcp_syn(
    const IPv4Address& src_ip, const IPv4Address& dst_ip,
    uint16_t src_port, uint16_t dst_port, uint32_t seq_num) {
    
    TCPHeader tcp(src_port, dst_port);
    tcp.seq_num(seq_num).flags(TCPHeader::FLAG_SYN);
    
    auto tcp_bytes = tcp.to_bytes();
    
    IPv4Header ip(src_ip, dst_ip, IPv4Header::PROTOCOL_TCP);
    ip.length(IPv4Header::MIN_SIZE + tcp_bytes.size());
    
    PacketBuilder builder;
    builder.ipv4(ip).tcp(tcp);
    
    return builder.build();
}

std::vector<uint8_t> udp_packet(
    const IPv4Address& src_ip, const IPv4Address& dst_ip,
    uint16_t src_port, uint16_t dst_port, 
    const std::vector<uint8_t>& payload) {
    
    UDPHeader udp(src_port, dst_port, UDPHeader::SIZE + payload.size());
    
    auto udp_bytes = udp.to_bytes();
    
    IPv4Header ip(src_ip, dst_ip, IPv4Header::PROTOCOL_UDP);
    ip.length(IPv4Header::MIN_SIZE + udp_bytes.size() + payload.size());
    
    PacketBuilder builder;
    builder.ipv4(ip).udp(udp).payload(payload);
    
    return builder.build();
}

std::vector<uint8_t> icmp_ping(
    const IPv4Address& src_ip, const IPv4Address& dst_ip,
    uint16_t identifier, uint16_t sequence) {
    
    ICMPHeader icmp(ICMPHeader::TYPE_ECHO_REQUEST, 0);
    icmp.identifier(identifier).sequence(sequence);
    
    auto icmp_bytes = icmp.to_bytes();
    
    IPv4Header ip(src_ip, dst_ip, IPv4Header::PROTOCOL_ICMP);
    ip.length(IPv4Header::MIN_SIZE + icmp_bytes.size());
    
    PacketBuilder builder;
    builder.ipv4(ip).icmp(icmp);
    
    return builder.build();
}

std::vector<uint8_t> ethernet_frame(
    const MacAddress& src_mac, const MacAddress& dst_mac,
    uint16_t ethertype, const std::vector<uint8_t>& payload) {
    
    EthernetHeader eth(dst_mac, src_mac, ethertype);
    
    PacketBuilder builder;
    builder.ethernet(eth).payload(payload);
    
    return builder.build();
}

std::vector<uint8_t> mpls_packet(
    uint32_t label, uint8_t ttl, uint8_t tc,
    const std::vector<uint8_t>& payload) {
    
    MPLSHeader mpls(label, tc, true, ttl);
    
    PacketBuilder builder;
    builder.mpls(mpls).payload(payload);
    
    return builder.build();
}

std::vector<uint8_t> mpls_ethernet_frame(
    const MacAddress& src_mac, const MacAddress& dst_mac,
    uint32_t label, uint8_t ttl, uint8_t tc,
    const std::vector<uint8_t>& payload) {
    
    EthernetHeader eth(dst_mac, src_mac, EthernetHeader::ETHERTYPE_MPLS);
    MPLSHeader mpls(label, tc, true, ttl);
    
    PacketBuilder builder;
    builder.ethernet(eth).mpls(mpls).payload(payload);
    
    return builder.build();
}

} // namespace patterns
} // namespace cppscapy
