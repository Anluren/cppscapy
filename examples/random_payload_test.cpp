#include "network_headers.h"
#include "utils.h"
#include <iostream>
#include <iomanip>
#include <chrono>

using namespace cppscapy;
using namespace cppscapy::utils;

int main() {
    std::cout << "=== Random Payload Generation Test ===\n\n";
    
    const size_t test_size = 32;
    
    std::cout << "1. Completely Random Bytes:\n";
    auto payload1 = random::random_bytes(test_size);
    print_hex_ascii(payload1, "");
    
    std::cout << "\n2. Random Bytes in Range (0x20-0x7E - printable ASCII):\n";
    auto payload2 = random::random_bytes_range(test_size, 0x20, 0x7E);
    print_hex_ascii(payload2, "");
    
    std::cout << "\n3. Random Printable ASCII:\n";
    auto payload3 = random::random_ascii(test_size);
    print_hex_ascii(payload3, "");
    
    std::cout << "\n4. Random Alphanumeric:\n";
    auto payload4 = random::random_alphanumeric(test_size);
    print_hex_ascii(payload4, "");
    
    std::cout << "\n5. Random Hex Characters (0-9, A-F):\n";
    auto payload5 = random::random_hex_chars(test_size);
    print_hex_ascii(payload5, "");
    
    std::cout << "\n6. Random Pattern from Custom Set:\n";
    std::vector<uint8_t> pattern_chars = {'A', 'B', 'C', 'X', 'Y', 'Z', '1', '2', '3'};
    auto payload6 = random::random_pattern(test_size, pattern_chars);
    print_hex_ascii(payload6, "");
    
    std::cout << "\n7. Seeded Random (Reproducible):\n";
    auto payload7a = random::random_bytes_seeded(test_size, 12345);
    auto payload7b = random::random_bytes_seeded(test_size, 12345);
    std::cout << "First generation:  ";
    for (size_t i = 0; i < std::min(size_t(16), payload7a.size()); ++i) {
        std::cout << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(payload7a[i]) << " ";
    }
    std::cout << std::dec << "\nSecond generation: ";
    for (size_t i = 0; i < std::min(size_t(16), payload7b.size()); ++i) {
        std::cout << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(payload7b[i]) << " ";
    }
    std::cout << std::dec << "\nIdentical: " << (payload7a == payload7b ? "YES" : "NO") << "\n";
    
    std::cout << "\n8. Random Network-like Data:\n";
    auto payload8 = random::random_network_data(test_size);
    print_hex_ascii(payload8, "");
    
    std::cout << "\n9. Repeating Pattern:\n";
    auto payload9 = random::random_repeating_pattern(test_size, 4);
    print_hex_ascii(payload9, "");
    
    std::cout << "\n10. HTTP-like Data:\n";
    auto payload10 = random::random_http_like(test_size);
    print_hex_ascii(payload10, "");
    
    std::cout << "\n11. Binary Protocol-like Data:\n";
    auto payload11 = random::random_binary_protocol(test_size);
    print_hex_ascii(payload11, "");
    
    // Practical example: Using random payloads in network packets
    std::cout << "\n=== Practical Usage Examples ===\n";
    
    // Create packets with different random payloads
    MacAddress src_mac("aa:bb:cc:dd:ee:ff");
    MacAddress dst_mac("11:22:33:44:55:66");
    IPv4Address src_ip("192.168.1.100");
    IPv4Address dst_ip("10.0.0.1");
    
    std::cout << "\n1. TCP packet with random binary payload:\n";
    auto tcp_payload = random::random_bytes(64);
    auto tcp_packet = PacketBuilder()
        .ethernet(EthernetHeader(dst_mac, src_mac, EthernetHeader::ETHERTYPE_IPV4))
        .ipv4(IPv4Header(src_ip, dst_ip, IPv4Header::PROTOCOL_TCP))
        .tcp(TCPHeader(80, 8080).flags(TCPHeader::FLAG_SYN))
        .payload(tcp_payload)
        .build();
    std::cout << "TCP packet size: " << tcp_packet.size() << " bytes\n";
    
    std::cout << "\n2. UDP packet with random ASCII payload:\n";
    auto udp_payload = random::random_ascii(32);
    auto udp_packet = PacketBuilder()
        .ethernet(EthernetHeader(dst_mac, src_mac, EthernetHeader::ETHERTYPE_IPV4))
        .ipv4(IPv4Header(src_ip, dst_ip, IPv4Header::PROTOCOL_UDP))
        .udp(UDPHeader(53, 12345))
        .payload(udp_payload)
        .build();
    std::cout << "UDP packet size: " << udp_packet.size() << " bytes\n";
    print_hex_ascii(std::vector<uint8_t>(udp_packet.end() - 32, udp_packet.end()), "UDP payload");
    
    std::cout << "\n3. MPLS packet with HTTP-like payload:\n";
    auto mpls_payload = random::random_http_like(48);
    auto mpls_packet = PacketBuilder()
        .ethernet(EthernetHeader(dst_mac, src_mac, EthernetHeader::ETHERTYPE_MPLS))
        .mpls(MPLSHeader(1000, 3, true, 64))
        .payload(mpls_payload)
        .build();
    std::cout << "MPLS packet size: " << mpls_packet.size() << " bytes\n";
    print_hex_ascii(std::vector<uint8_t>(mpls_packet.end() - 48, mpls_packet.end()), "MPLS payload");
    
    // Test 12: Incremental pattern
    std::cout << "\n12. Incremental Pattern (values 0-255):\n";
    auto incremental1 = random::incremental_pattern(20);
    std::cout << "Size: " << incremental1.size() << " bytes\n";
    print_hex(incremental1, "Incremental pattern (20 bytes)");
    
    auto incremental2 = random::incremental_pattern(20, 240);
    std::cout << "Size: " << incremental2.size() << " bytes\n";
    print_hex(incremental2, "Incremental pattern starting from 240");
    
    // Performance comparison
    std::cout << "\n=== Performance Comparison ===\n";
    const size_t large_size = 1024;
    const int iterations = 1000;
    
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
        auto payload = random::random_bytes(large_size);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "Generated " << iterations << " payloads of " << large_size 
              << " bytes each in " << duration.count() << " microseconds\n";
    std::cout << "Average: " << duration.count() / iterations << " microseconds per payload\n";
    
    std::cout << "\n=== Random Payload Generation Test Complete ===\n";
    
    return 0;
}
