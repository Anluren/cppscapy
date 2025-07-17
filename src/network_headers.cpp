#include "../include/network_headers.h"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <stdexcept>
#include <cstring>
#include <arpa/inet.h>

namespace cppscapy {

// MacAddress implementation
MacAddress::MacAddress(const std::string& mac_str) {
    std::stringstream ss(mac_str);
    std::string token;
    int i = 0;
    
    while (std::getline(ss, token, ':') && i < 6) {
        bytes_[i] = static_cast<uint8_t>(std::stoul(token, nullptr, 16));
        i++;
    }
    
    if (i != 6) {
        throw std::invalid_argument("Invalid MAC address format");
    }
}

MacAddress::MacAddress(const std::array<uint8_t, 6>& bytes) : bytes_(bytes) {}

MacAddress::MacAddress(uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4, uint8_t b5, uint8_t b6) {
    bytes_ = {b1, b2, b3, b4, b5, b6};
}

std::string MacAddress::to_string() const {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (int i = 0; i < 6; ++i) {
        if (i > 0) ss << ":";
        ss << std::setw(2) << static_cast<int>(bytes_[i]);
    }
    return ss.str();
}

std::array<uint8_t, 6> MacAddress::to_bytes() const {
    return bytes_;
}

bool MacAddress::is_broadcast() const {
    return std::all_of(bytes_.begin(), bytes_.end(), [](uint8_t b) { return b == 0xFF; });
}

bool MacAddress::is_multicast() const {
    return (bytes_[0] & 0x01) != 0;
}

MacAddress MacAddress::broadcast() {
    return MacAddress(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF);
}

MacAddress MacAddress::multicast_ipv4() {
    return MacAddress(0x01, 0x00, 0x5E, 0x00, 0x00, 0x00);
}

MacAddress MacAddress::multicast_ipv6() {
    return MacAddress(0x33, 0x33, 0x00, 0x00, 0x00, 0x00);
}

// IPv4Address implementation
IPv4Address::IPv4Address(const std::string& ip_str) {
    if (inet_pton(AF_INET, ip_str.c_str(), &ip_) != 1) {
        throw std::invalid_argument("Invalid IPv4 address format");
    }
}

IPv4Address::IPv4Address(uint32_t ip) : ip_(ip) {}

IPv4Address::IPv4Address(uint8_t a, uint8_t b, uint8_t c, uint8_t d) {
    ip_ = htonl((static_cast<uint32_t>(a) << 24) | 
                (static_cast<uint32_t>(b) << 16) | 
                (static_cast<uint32_t>(c) << 8) | 
                static_cast<uint32_t>(d));
}

std::string IPv4Address::to_string() const {
    char buffer[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &ip_, buffer, INET_ADDRSTRLEN);
    return std::string(buffer);
}

uint32_t IPv4Address::to_uint32() const {
    return ip_;
}

std::array<uint8_t, 4> IPv4Address::to_bytes() const {
    uint32_t host_order = ntohl(ip_);
    return {
        static_cast<uint8_t>((host_order >> 24) & 0xFF),
        static_cast<uint8_t>((host_order >> 16) & 0xFF),
        static_cast<uint8_t>((host_order >> 8) & 0xFF),
        static_cast<uint8_t>(host_order & 0xFF)
    };
}

IPv4Address IPv4Address::localhost() {
    return IPv4Address(127, 0, 0, 1);
}

IPv4Address IPv4Address::broadcast() {
    return IPv4Address(255, 255, 255, 255);
}

IPv4Address IPv4Address::any() {
    return IPv4Address(0, 0, 0, 0);
}

// IPv6Address implementation
IPv6Address::IPv6Address(const std::string& ip_str) {
    if (inet_pton(AF_INET6, ip_str.c_str(), bytes_.data()) != 1) {
        throw std::invalid_argument("Invalid IPv6 address format");
    }
}

IPv6Address::IPv6Address(const std::array<uint8_t, 16>& bytes) : bytes_(bytes) {}

std::string IPv6Address::to_string() const {
    char buffer[INET6_ADDRSTRLEN];
    inet_ntop(AF_INET6, bytes_.data(), buffer, INET6_ADDRSTRLEN);
    return std::string(buffer);
}

std::array<uint8_t, 16> IPv6Address::to_bytes() const {
    return bytes_;
}

IPv6Address IPv6Address::localhost() {
    std::array<uint8_t, 16> bytes = {0};
    bytes[15] = 1;
    return IPv6Address(bytes);
}

IPv6Address IPv6Address::any() {
    std::array<uint8_t, 16> bytes = {0};
    return IPv6Address(bytes);
}

// Helper function to calculate checksum
uint16_t calculate_checksum(const std::vector<uint8_t>& data) {
    uint32_t sum = 0;
    
    // Sum all 16-bit words
    for (size_t i = 0; i < data.size() - 1; i += 2) {
        sum += (data[i] << 8) + data[i + 1];
    }
    
    // Add odd byte if present
    if (data.size() % 2 == 1) {
        sum += data[data.size() - 1] << 8;
    }
    
    // Fold 32-bit sum to 16 bits
    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }
    
    return ~sum;
}

// EthernetHeader implementation
EthernetHeader::EthernetHeader(const MacAddress& dst, const MacAddress& src, uint16_t ethertype) 
    : dst_(dst), src_(src), ethertype_(ethertype) {}

EthernetHeader& EthernetHeader::dst(const MacAddress& mac) {
    dst_ = mac;
    return *this;
}

EthernetHeader& EthernetHeader::src(const MacAddress& mac) {
    src_ = mac;
    return *this;
}

EthernetHeader& EthernetHeader::ethertype(uint16_t type) {
    ethertype_ = type;
    return *this;
}

std::vector<uint8_t> EthernetHeader::to_bytes() const {
    std::vector<uint8_t> result;
    result.reserve(SIZE);
    
    // Destination MAC
    auto dst_bytes = dst_.to_bytes();
    result.insert(result.end(), dst_bytes.begin(), dst_bytes.end());
    
    // Source MAC
    auto src_bytes = src_.to_bytes();
    result.insert(result.end(), src_bytes.begin(), src_bytes.end());
    
    // EtherType
    result.push_back((ethertype_ >> 8) & 0xFF);
    result.push_back(ethertype_ & 0xFF);
    
    return result;
}

// IPv4Header implementation
IPv4Header::IPv4Header(const IPv4Address& src, const IPv4Address& dst, uint8_t protocol) 
    : src_(src), dst_(dst), protocol_(protocol) {}

std::vector<uint8_t> IPv4Header::to_bytes() const {
    std::vector<uint8_t> result;
    result.reserve(MIN_SIZE);
    
    // Version and IHL
    result.push_back((version_ << 4) | ihl_);
    
    // Type of Service
    result.push_back(tos_);
    
    // Total Length
    result.push_back((length_ >> 8) & 0xFF);
    result.push_back(length_ & 0xFF);
    
    // Identification
    result.push_back((id_ >> 8) & 0xFF);
    result.push_back(id_ & 0xFF);
    
    // Flags and Fragment Offset
    uint16_t flags_and_offset = (flags_ << 13) | fragment_offset_;
    result.push_back((flags_and_offset >> 8) & 0xFF);
    result.push_back(flags_and_offset & 0xFF);
    
    // TTL
    result.push_back(ttl_);
    
    // Protocol
    result.push_back(protocol_);
    
    // Checksum (will be calculated later)
    result.push_back(0);
    result.push_back(0);
    
    // Source IP
    auto src_bytes = src_.to_bytes();
    result.insert(result.end(), src_bytes.begin(), src_bytes.end());
    
    // Destination IP
    auto dst_bytes = dst_.to_bytes();
    result.insert(result.end(), dst_bytes.begin(), dst_bytes.end());
    
    // Calculate and set checksum
    uint16_t checksum = calculate_checksum(result);
    result[10] = (checksum >> 8) & 0xFF;
    result[11] = checksum & 0xFF;
    
    return result;
}

// IPv6Header implementation
IPv6Header::IPv6Header(const IPv6Address& src, const IPv6Address& dst, uint8_t next_header) 
    : src_(src), dst_(dst), next_header_(next_header) {}

std::vector<uint8_t> IPv6Header::to_bytes() const {
    std::vector<uint8_t> result;
    result.reserve(SIZE);
    
    // Version, Traffic Class, Flow Label
    uint32_t version_tc_fl = (version_ << 28) | (traffic_class_ << 20) | flow_label_;
    result.push_back((version_tc_fl >> 24) & 0xFF);
    result.push_back((version_tc_fl >> 16) & 0xFF);
    result.push_back((version_tc_fl >> 8) & 0xFF);
    result.push_back(version_tc_fl & 0xFF);
    
    // Payload Length
    result.push_back((payload_length_ >> 8) & 0xFF);
    result.push_back(payload_length_ & 0xFF);
    
    // Next Header
    result.push_back(next_header_);
    
    // Hop Limit
    result.push_back(hop_limit_);
    
    // Source Address
    auto src_bytes = src_.to_bytes();
    result.insert(result.end(), src_bytes.begin(), src_bytes.end());
    
    // Destination Address
    auto dst_bytes = dst_.to_bytes();
    result.insert(result.end(), dst_bytes.begin(), dst_bytes.end());
    
    return result;
}

} // namespace cppscapy
