#pragma once

#include "network_headers.h"
#include <string>
#include <string_view>
#include <vector>
#include <array>
#include <iostream>
#include <cctype>
#include <stdexcept>

namespace cppscapy {
namespace utils {

// Utility to print packet in hex format
void print_hex(const std::vector<uint8_t>& data, const std::string& description = "");

// Utility to print packet in both hex and ASCII
void print_hex_ascii(const std::vector<uint8_t>& data, const std::string& description = "");

// Convert packet to hex string
std::string to_hex_string(const std::vector<uint8_t>& data);

// Convert array to hex string (constexpr version)
template<size_t N>
constexpr std::array<char, N * 2 + 1> to_hex_string_array(const std::array<uint8_t, N>& data);

// Template implementation for to_hex_string_array
template<size_t N>
constexpr std::array<char, N * 2 + 1> to_hex_string_array(const std::array<uint8_t, N>& data) {
    std::array<char, N * 2 + 1> result = {};
    constexpr char hex_chars[] = "0123456789abcdef";
    
    for (size_t i = 0; i < N; ++i) {
        result[i * 2] = hex_chars[data[i] >> 4];
        result[i * 2 + 1] = hex_chars[data[i] & 0x0F];
    }
    result[N * 2] = '\0';
    
    return result;
}

// Parse hex string back to packet
std::vector<uint8_t> from_hex_string(const std::string& hex_str);

// Convert hex string to fixed-size array (e.g., MAC addresses, IP addresses)
template<size_t N>
constexpr std::array<uint8_t, N> from_hex_string_array(std::string_view hex_str);

// Convert hex string to auto-sized array (assumes only hex characters)
template<size_t N>
constexpr std::array<uint8_t, N / 2> from_hex_string_auto(const char (&hex_str)[N]) {
    static_assert(N > 1, "Hex string must not be empty");
    static_assert((N - 1) % 2 == 0, "Hex string must have even number of characters");
    
    constexpr size_t array_size = (N - 1) / 2;  // -1 for null terminator
    std::array<uint8_t, array_size> result = {};
    
    constexpr auto hex_to_byte = [](char c) -> uint8_t {
        if (c >= '0' && c <= '9') return c - '0';
        if (c >= 'a' && c <= 'f') return c - 'a' + 10;
        if (c >= 'A' && c <= 'F') return c - 'A' + 10;
        return 0;
    };
    
    for (size_t i = 0; i < array_size; ++i) {
        result[i] = (hex_to_byte(hex_str[i * 2]) << 4) | hex_to_byte(hex_str[i * 2 + 1]);
    }
    
    return result;
}

// Template implementation for from_hex_string_array
template<size_t N>
constexpr std::array<uint8_t, N> from_hex_string_array(std::string_view hex_str) {
    std::array<uint8_t, N> result = {};
    size_t clean_hex_pos = 0;
    char clean_hex[N * 2] = {};
    
    // Remove spaces, colons, and convert to lowercase
    for (size_t i = 0; i < hex_str.length() && clean_hex_pos < N * 2; ++i) {
        char c = hex_str[i];
        if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F')) {
            clean_hex[clean_hex_pos++] = (c >= 'A' && c <= 'F') ? c + 32 : c; // to lowercase
        }
    }
    
    // Check if we have enough hex characters for the array size
    if (clean_hex_pos < N * 2) {
        throw std::invalid_argument("Not enough hex data for array size");
    }
    
    // Convert pairs of hex characters to bytes
    for (size_t i = 0; i < N; ++i) {
        auto hex_to_byte = [](char c) -> uint8_t {
            if (c >= '0' && c <= '9') return c - '0';
            if (c >= 'a' && c <= 'f') return c - 'a' + 10;
            return 0;
        };
        
        result[i] = (hex_to_byte(clean_hex[i * 2]) << 4) | hex_to_byte(clean_hex[i * 2 + 1]);
    }
    
    return result;
}

// Convenience functions for common array sizes
constexpr inline std::array<uint8_t, 6> mac_from_hex_string(std::string_view hex_str) {
    return from_hex_string_array<6>(hex_str);
}

constexpr inline std::array<uint8_t, 4> ipv4_from_hex_string(std::string_view hex_str) {
    return from_hex_string_array<4>(hex_str);
}

constexpr inline std::array<uint8_t, 16> ipv6_from_hex_string(std::string_view hex_str) {
    return from_hex_string_array<16>(hex_str);
}

// Convenience functions using automatic length deduction
// Note: These create the arrays at compile time, but the address constructors are not constexpr
template<size_t N>
auto make_mac_address(const char (&hex_str)[N]) {
    static_assert(N == 13, "MAC address hex string must be exactly 12 characters plus null terminator");
    return cppscapy::MacAddress(from_hex_string_auto(hex_str));
}

template<size_t N>
auto make_ipv4_address(const char (&hex_str)[N]) {
    static_assert(N == 9, "IPv4 address hex string must be exactly 8 characters plus null terminator");
    auto bytes = from_hex_string_auto(hex_str);
    return cppscapy::IPv4Address(bytes[0], bytes[1], bytes[2], bytes[3]);
}

// Calculate and verify checksums
uint16_t calculate_ip_checksum(const std::vector<uint8_t>& header);

// Calculate IPv4 header checksum (automatically clears checksum field)
uint16_t calculate_ipv4_header_checksum(const std::vector<uint8_t>& header);

uint16_t calculate_tcp_checksum(const std::vector<uint8_t>& tcp_header, 
                               const IPv4Address& src_ip, 
                               const IPv4Address& dst_ip,
                               const std::vector<uint8_t>& payload = {});

// Verify IPv4 header checksum - raw pointer version
bool verify_ipv4_checksum(const uint8_t* data, size_t length);

// Verify IPv4 header checksum - vector version
bool verify_ipv4_checksum(const std::vector<uint8_t>& header);

// Packet analysis utilities
struct PacketInfo {
    bool has_ethernet = false;
    bool has_ipv4 = false;
    bool has_ipv6 = false;
    bool has_tcp = false;
    bool has_udp = false;
    bool has_icmp = false;
    
    MacAddress src_mac;
    MacAddress dst_mac;
    uint16_t ethertype = 0;
    
    IPv4Address src_ipv4;
    IPv4Address dst_ipv4;
    uint8_t ip_protocol = 0;
    
    uint16_t src_port = 0;
    uint16_t dst_port = 0;
    
    size_t payload_offset = 0;
    size_t payload_size = 0;
};

// Parse packet and extract information
PacketInfo analyze_packet(const std::vector<uint8_t>& packet);

// Common port numbers
namespace ports {
    constexpr uint16_t HTTP = 80;
    constexpr uint16_t HTTPS = 443;
    constexpr uint16_t FTP = 21;
    constexpr uint16_t SSH = 22;
    constexpr uint16_t TELNET = 23;
    constexpr uint16_t SMTP = 25;
    constexpr uint16_t DNS = 53;
    constexpr uint16_t DHCP_SERVER = 67;
    constexpr uint16_t DHCP_CLIENT = 68;
    constexpr uint16_t POP3 = 110;
    constexpr uint16_t IMAP = 143;
    constexpr uint16_t SNMP = 161;
    constexpr uint16_t MYSQL = 3306;
    constexpr uint16_t POSTGRESQL = 5432;
}

// Common IP addresses
namespace common_ips {
    inline IPv4Address google_dns1() { return IPv4Address("8.8.8.8"); }
    inline IPv4Address google_dns2() { return IPv4Address("8.8.4.4"); }
    inline IPv4Address cloudflare_dns1() { return IPv4Address("1.1.1.1"); }
    inline IPv4Address cloudflare_dns2() { return IPv4Address("1.0.0.1"); }
    inline IPv4Address private_192() { return IPv4Address("192.168.1.1"); }
    inline IPv4Address private_10() { return IPv4Address("10.0.0.1"); }
    inline IPv4Address private_172() { return IPv4Address("172.16.0.1"); }
}

// Payload generators for testing
std::vector<uint8_t> generate_random_payload(size_t size);
std::vector<uint8_t> generate_http_get_request(const std::string& host, const std::string& path = "/");
std::vector<uint8_t> generate_dns_query(const std::string& domain);

// Random payload generation utilities
namespace random {
    // Generate completely random payload
    std::vector<uint8_t> random_bytes(size_t size);
    
    // Generate random payload with specific byte range
    std::vector<uint8_t> random_bytes_range(size_t size, uint8_t min_val = 0, uint8_t max_val = 255);
    
    // Generate random printable ASCII payload
    std::vector<uint8_t> random_ascii(size_t size);
    
    // Generate random alphanumeric payload  
    std::vector<uint8_t> random_alphanumeric(size_t size);
    
    // Generate random hex-like payload (0-9, A-F)
    std::vector<uint8_t> random_hex_chars(size_t size);
    
    // Generate random payload with specific pattern
    std::vector<uint8_t> random_pattern(size_t size, const std::vector<uint8_t>& pattern_chars);
    
    // Generate random payload with seed (for reproducible results)
    std::vector<uint8_t> random_bytes_seeded(size_t size, uint32_t seed);
    
    // Generate random network-like payload (mix of binary and text)
    std::vector<uint8_t> random_network_data(size_t size);
    
    // Generate random payload with repeating pattern
    std::vector<uint8_t> random_repeating_pattern(size_t size, size_t pattern_length);
    
    // Generate random payload simulating specific protocols
    std::vector<uint8_t> random_http_like(size_t size);
    std::vector<uint8_t> random_binary_protocol(size_t size);
    
    // Generate incremental pattern payload (0, 1, 2, ..., 255, 0, 1, ...)
    std::vector<uint8_t> incremental_pattern(size_t size, uint8_t start_value = 0);
    
    // In-place randomization functions for existing vectors
    void randomize_bytes(std::vector<uint8_t>& data);
    void randomize_bytes_range(std::vector<uint8_t>& data, uint8_t min_val = 0, uint8_t max_val = 255);
    void randomize_ascii(std::vector<uint8_t>& data);
    void randomize_alphanumeric(std::vector<uint8_t>& data);
    void randomize_hex_chars(std::vector<uint8_t>& data);
    void randomize_pattern(std::vector<uint8_t>& data, const std::vector<uint8_t>& pattern_chars);
    void randomize_bytes_seeded(std::vector<uint8_t>& data, uint32_t seed);
    void randomize_network_data(std::vector<uint8_t>& data);
    void randomize_http_like(std::vector<uint8_t>& data);
    void randomize_binary_protocol(std::vector<uint8_t>& data);
    void fill_incremental_pattern(std::vector<uint8_t>& data, uint8_t start_value = 0);
    
    // Partial randomization functions (randomize only a portion of the vector)
    void randomize_bytes_partial(std::vector<uint8_t>& data, size_t start_pos, size_t length);
    void randomize_ascii_partial(std::vector<uint8_t>& data, size_t start_pos, size_t length);
    void fill_incremental_pattern_partial(std::vector<uint8_t>& data, size_t start_pos, size_t length, uint8_t start_value = 0);
} // namespace random

} // namespace utils
} // namespace cppscapy
