#include "network_headers.h"
#include <algorithm>
#include <numeric>

namespace cppscapy {

namespace {
    // Helper function to calculate 16-bit checksum using one's complement
    uint16_t calculate_checksum_16(const std::vector<uint8_t>& data) {
        uint32_t sum = 0;
        
        // Sum all 16-bit words
        for (size_t i = 0; i < data.size(); i += 2) {
            if (i + 1 < data.size()) {
                // Network byte order (big-endian)
                uint16_t word = (static_cast<uint16_t>(data[i]) << 8) | data[i + 1];
                sum += word;
            } else {
                // Odd byte - pad with zero
                uint16_t word = static_cast<uint16_t>(data[i]) << 8;
                sum += word;
            }
        }
        
        // Add carry bits
        while (sum >> 16) {
            sum = (sum & 0xFFFF) + (sum >> 16);
        }
        
        // One's complement
        return static_cast<uint16_t>(~sum);
    }
    
    // Create IPv4 pseudo-header for UDP checksum
    std::vector<uint8_t> create_ipv4_pseudo_header(const IPv4Address& src_ip, 
                                                   const IPv4Address& dst_ip, 
                                                   uint16_t udp_length) {
        std::vector<uint8_t> pseudo_header;
        pseudo_header.reserve(12);
        
        // Source IP (4 bytes)
        auto src_bytes = src_ip.to_bytes();
        pseudo_header.insert(pseudo_header.end(), src_bytes.begin(), src_bytes.end());
        
        // Destination IP (4 bytes)
        auto dst_bytes = dst_ip.to_bytes();
        pseudo_header.insert(pseudo_header.end(), dst_bytes.begin(), dst_bytes.end());
        
        // Zero byte
        pseudo_header.push_back(0);
        
        // Protocol (UDP = 17)
        pseudo_header.push_back(17);
        
        // UDP length (2 bytes, network byte order)
        pseudo_header.push_back((udp_length >> 8) & 0xFF);
        pseudo_header.push_back(udp_length & 0xFF);
        
        return pseudo_header;
    }
    
    // Create IPv6 pseudo-header for UDP checksum
    std::vector<uint8_t> create_ipv6_pseudo_header(const IPv6Address& src_ip, 
                                                   const IPv6Address& dst_ip, 
                                                   uint16_t udp_length) {
        std::vector<uint8_t> pseudo_header;
        pseudo_header.reserve(40);
        
        // Source IP (16 bytes)
        auto src_bytes = src_ip.to_bytes();
        pseudo_header.insert(pseudo_header.end(), src_bytes.begin(), src_bytes.end());
        
        // Destination IP (16 bytes)
        auto dst_bytes = dst_ip.to_bytes();
        pseudo_header.insert(pseudo_header.end(), dst_bytes.begin(), dst_bytes.end());
        
        // UDP length (4 bytes, network byte order, upper 2 bytes are zero)
        pseudo_header.push_back(0);
        pseudo_header.push_back(0);
        pseudo_header.push_back((udp_length >> 8) & 0xFF);
        pseudo_header.push_back(udp_length & 0xFF);
        
        // Zeros (3 bytes)
        pseudo_header.push_back(0);
        pseudo_header.push_back(0);
        pseudo_header.push_back(0);
        
        // Next Header (UDP = 17)
        pseudo_header.push_back(17);
        
        return pseudo_header;
    }
}

uint16_t UDPHeader::calculate_checksum(const IPv4Address& src_ip, const IPv4Address& dst_ip, 
                                      const std::vector<uint8_t>& payload) const {
    // Calculate total UDP length (header + payload)
    uint16_t udp_length = static_cast<uint16_t>(SIZE + payload.size());
    
    // Create pseudo-header
    auto pseudo_header = create_ipv4_pseudo_header(src_ip, dst_ip, udp_length);
    
    // Create UDP header with checksum field set to zero
    std::vector<uint8_t> udp_data;
    udp_data.reserve(SIZE + payload.size());
    
    // UDP header (with checksum = 0)
    udp_data.push_back((src_port_ >> 8) & 0xFF);
    udp_data.push_back(src_port_ & 0xFF);
    udp_data.push_back((dst_port_ >> 8) & 0xFF);
    udp_data.push_back(dst_port_ & 0xFF);
    udp_data.push_back((udp_length >> 8) & 0xFF);
    udp_data.push_back(udp_length & 0xFF);
    udp_data.push_back(0); // Checksum = 0 for calculation
    udp_data.push_back(0);
    
    // Add payload
    udp_data.insert(udp_data.end(), payload.begin(), payload.end());
    
    // Combine pseudo-header + UDP header + payload
    std::vector<uint8_t> checksum_data;
    checksum_data.reserve(pseudo_header.size() + udp_data.size());
    checksum_data.insert(checksum_data.end(), pseudo_header.begin(), pseudo_header.end());
    checksum_data.insert(checksum_data.end(), udp_data.begin(), udp_data.end());
    
    // Calculate checksum
    uint16_t checksum = calculate_checksum_16(checksum_data);
    
    // Special case: if checksum is 0, use 0xFFFF (all 1s)
    // This is because UDP uses 0 to indicate "no checksum computed"
    return (checksum == 0) ? 0xFFFF : checksum;
}

uint16_t UDPHeader::calculate_checksum(const IPv6Address& src_ip, const IPv6Address& dst_ip, 
                                      const std::vector<uint8_t>& payload) const {
    // Calculate total UDP length (header + payload)
    uint16_t udp_length = static_cast<uint16_t>(SIZE + payload.size());
    
    // Create pseudo-header
    auto pseudo_header = create_ipv6_pseudo_header(src_ip, dst_ip, udp_length);
    
    // Create UDP header with checksum field set to zero
    std::vector<uint8_t> udp_data;
    udp_data.reserve(SIZE + payload.size());
    
    // UDP header (with checksum = 0)
    udp_data.push_back((src_port_ >> 8) & 0xFF);
    udp_data.push_back(src_port_ & 0xFF);
    udp_data.push_back((dst_port_ >> 8) & 0xFF);
    udp_data.push_back(dst_port_ & 0xFF);
    udp_data.push_back((udp_length >> 8) & 0xFF);
    udp_data.push_back(udp_length & 0xFF);
    udp_data.push_back(0); // Checksum = 0 for calculation
    udp_data.push_back(0);
    
    // Add payload
    udp_data.insert(udp_data.end(), payload.begin(), payload.end());
    
    // Combine pseudo-header + UDP header + payload
    std::vector<uint8_t> checksum_data;
    checksum_data.reserve(pseudo_header.size() + udp_data.size());
    checksum_data.insert(checksum_data.end(), pseudo_header.begin(), pseudo_header.end());
    checksum_data.insert(checksum_data.end(), udp_data.begin(), udp_data.end());
    
    // Calculate checksum
    uint16_t checksum = calculate_checksum_16(checksum_data);
    
    // Note: For IPv6 UDP, checksum is mandatory and cannot be 0
    // If calculated checksum is 0, use 0xFFFF
    return (checksum == 0) ? 0xFFFF : checksum;
}

UDPHeader& UDPHeader::update_checksum(const IPv4Address& src_ip, const IPv4Address& dst_ip, 
                                     const std::vector<uint8_t>& payload) {
    checksum_ = calculate_checksum(src_ip, dst_ip, payload);
    return *this;
}

UDPHeader& UDPHeader::update_checksum(const IPv6Address& src_ip, const IPv6Address& dst_ip, 
                                     const std::vector<uint8_t>& payload) {
    checksum_ = calculate_checksum(src_ip, dst_ip, payload);
    return *this;
}

} // namespace cppscapy
