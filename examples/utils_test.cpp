#include "../include/network_headers.h"
#include "../include/utils.h"
#include <iostream>
#include <iomanip>
#include <cassert>

using namespace cppscapy;
using namespace cppscapy::utils;

int main() {
    std::cout << "=== Testing Utils Functions ===\n";
    
    // Test hex string conversion
    std::cout << "\n1. Testing hex string conversion:\n";
    std::vector<uint8_t> test_data = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
    
    std::string hex_str = to_hex_string(test_data);
    std::cout << "Original data to hex: " << hex_str << "\n";
    
    auto recovered_data = from_hex_string(hex_str);
    std::cout << "Recovered data matches: " << (test_data == recovered_data ? "Yes" : "No") << "\n";
    
    // Test with spaces in hex string
    auto recovered_with_spaces = from_hex_string("AA BB CC DD EE FF");
    std::cout << "With spaces matches: " << (test_data == recovered_with_spaces ? "Yes" : "No") << "\n";
    
    // Test array conversion
    std::cout << "\nTesting array conversion:\n";
    
    // Test MAC address conversion
    auto mac_array = mac_from_hex_string("aa:bb:cc:dd:ee:ff");
    std::cout << "MAC array: ";
    for (size_t i = 0; i < mac_array.size(); ++i) {
        if (i > 0) std::cout << ":";
        std::cout << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(mac_array[i]);
    }
    std::cout << std::dec << "\n";
    
    // Test IPv4 address conversion
    auto ipv4_array = ipv4_from_hex_string("c0a80101"); // 192.168.1.1
    std::cout << "IPv4 array: ";
    for (size_t i = 0; i < ipv4_array.size(); ++i) {
        if (i > 0) std::cout << ".";
        std::cout << static_cast<int>(ipv4_array[i]);
    }
    std::cout << "\n";
    
    // Test generic array conversion
    auto generic_array = from_hex_string_array<3>("aabbcc");
    std::cout << "Generic 3-byte array: ";
    for (size_t i = 0; i < generic_array.size(); ++i) {
        std::cout << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(generic_array[i]) << " ";
    }
    std::cout << std::dec << "\n";

    // Test print functions
    std::cout << "\n2. Testing print functions:\n";
    print_hex(test_data, "Test data");
    print_hex_ascii(test_data, "Test data with ASCII");
    
    // Test packet analysis
    std::cout << "\n3. Testing packet analysis:\n";
    
    // Create a simple packet
    MacAddress src_mac("aa:bb:cc:dd:ee:ff");
    MacAddress dst_mac("11:22:33:44:55:66");
    IPv4Address src_ip("192.168.1.100");
    IPv4Address dst_ip("192.168.1.1");
    
    auto packet = PacketBuilder()
        .ethernet(EthernetHeader(dst_mac, src_mac, EthernetHeader::ETHERTYPE_IPV4))
        .ipv4(IPv4Header(src_ip, dst_ip, IPv4Header::PROTOCOL_TCP)
              .length(IPv4Header::MIN_SIZE + TCPHeader::MIN_SIZE))
        .tcp(TCPHeader(12345, 80).flags(TCPHeader::FLAG_SYN))
        .build();
    
    PacketInfo info = analyze_packet(packet);
    
    std::cout << "Packet analysis results:\n";
    std::cout << "  Has Ethernet: " << (info.has_ethernet ? "Yes" : "No") << "\n";
    std::cout << "  Has IPv4: " << (info.has_ipv4 ? "Yes" : "No") << "\n";
    std::cout << "  Has TCP: " << (info.has_tcp ? "Yes" : "No") << "\n";
    std::cout << "  Source MAC: " << info.src_mac.to_string() << "\n";
    std::cout << "  Dest MAC: " << info.dst_mac.to_string() << "\n";
    std::cout << "  Source IP: " << info.src_ipv4.to_string() << "\n";
    std::cout << "  Dest IP: " << info.dst_ipv4.to_string() << "\n";
    std::cout << "  Source Port: " << info.src_port << "\n";
    std::cout << "  Dest Port: " << info.dst_port << "\n";
    std::cout << "  Payload size: " << info.payload_size << " bytes\n";
    
    // Test payload generators
    std::cout << "\n4. Testing payload generators:\n";
    
    auto random_payload = generate_random_payload(10);
    std::cout << "Random payload (10 bytes): ";
    for (uint8_t b : random_payload) {
        std::cout << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(b) << " ";
    }
    std::cout << std::dec << "\n";
    
    auto http_request = generate_http_get_request("example.com", "/test");
    std::cout << "HTTP GET request size: " << http_request.size() << " bytes\n";
    std::cout << "HTTP request preview: " << std::string(http_request.begin(), http_request.begin() + 20) << "...\n";
    
    auto dns_query = generate_dns_query("example.com");
    std::cout << "DNS query size: " << dns_query.size() << " bytes\n";
    
    // Test common utilities
    std::cout << "\n5. Testing common utilities:\n";
    std::cout << "Google DNS: " << common_ips::google_dns1().to_string() << "\n";
    std::cout << "Cloudflare DNS: " << common_ips::cloudflare_dns1().to_string() << "\n";
    std::cout << "HTTP port: " << ports::HTTP << "\n";
    std::cout << "HTTPS port: " << ports::HTTPS << "\n";
    
    // Test IPv4 checksum verification
    std::cout << "\n6. Testing IPv4 checksum verification:\n";
    
    // Create a test IPv4 header
    IPv4Address test_src("192.168.1.100");
    IPv4Address test_dst("10.0.0.1");
    IPv4Header test_ipv4(test_src, test_dst, IPv4Header::PROTOCOL_TCP);
    test_ipv4.ttl(64).id(12345).length(40);
    
    auto test_header = test_ipv4.to_bytes();
    bool checksum_valid = verify_ipv4_checksum(test_header);
    std::cout << "IPv4 header checksum valid: " << (checksum_valid ? "Yes" : "No") << "\n";
    
    // Test with corrupted checksum
    auto corrupted = test_header;
    corrupted[10] ^= 0xFF; // Corrupt checksum
    bool corrupted_valid = verify_ipv4_checksum(corrupted);
    std::cout << "Corrupted checksum valid: " << (corrupted_valid ? "Yes" : "No") << "\n";
    
    // Test raw pointer version
    bool pointer_valid = verify_ipv4_checksum(test_header.data(), test_header.size());
    std::cout << "Raw pointer checksum valid: " << (pointer_valid ? "Yes" : "No") << "\n";

    std::cout << "\n=== Utils Tests Complete ===\n";
    return 0;
}
