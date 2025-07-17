#pragma once

#include "network_headers.h"
#include <string>
#include <vector>
#include <iostream>

namespace cppscapy {
namespace utils {

// Utility to print packet in hex format
void print_hex(const std::vector<uint8_t>& data, const std::string& description = "");

// Utility to print packet in both hex and ASCII
void print_hex_ascii(const std::vector<uint8_t>& data, const std::string& description = "");

// Convert packet to hex string
std::string to_hex_string(const std::vector<uint8_t>& data);

// Parse hex string back to packet
std::vector<uint8_t> from_hex_string(const std::string& hex_str);

// Calculate and verify checksums
uint16_t calculate_ip_checksum(const std::vector<uint8_t>& header);
uint16_t calculate_tcp_checksum(const std::vector<uint8_t>& tcp_header, 
                               const IPv4Address& src_ip, 
                               const IPv4Address& dst_ip,
                               const std::vector<uint8_t>& payload = {});

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

} // namespace utils
} // namespace cppscapy
