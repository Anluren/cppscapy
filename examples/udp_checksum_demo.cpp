#include "network_headers.h"
#include <iostream>
#include <iomanip>
#include <sstream>

using namespace cppscapy;

// Helper function to print bytes in hex
std::string bytes_to_hex(const std::vector<uint8_t>& bytes) {
    std::stringstream ss;
    for (size_t i = 0; i < bytes.size(); ++i) {
        if (i > 0) ss << " ";
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(bytes[i]);
    }
    return ss.str();
}

int main() {
    std::cout << "=== UDP Checksum Calculation Examples ===\n\n";
    
    // Example 1: IPv4 UDP packet
    std::cout << "1. IPv4 UDP Checksum Calculation\n";
    std::cout << "--------------------------------\n";
    
    IPv4Address src_ip("192.168.1.100");
    IPv4Address dst_ip("192.168.1.200");
    std::string payload_data = "Hello, UDP!";
    std::vector<uint8_t> payload(payload_data.begin(), payload_data.end());
    
    UDPHeader udp_header(12345, 80);
    udp_header.length(static_cast<uint16_t>(UDPHeader::SIZE + payload.size()));
    
    std::cout << "Source IP: " << src_ip.to_string() << "\n";
    std::cout << "Dest IP: " << dst_ip.to_string() << "\n";
    std::cout << "Source Port: " << udp_header.src_port() << "\n";
    std::cout << "Dest Port: " << udp_header.dst_port() << "\n";
    std::cout << "UDP Length: " << udp_header.length() << "\n";
    std::cout << "Payload: \"" << payload_data << "\"\n";
    std::cout << "Payload bytes: " << bytes_to_hex(payload) << "\n\n";
    
    // Calculate checksum
    uint16_t checksum = udp_header.calculate_checksum(src_ip, dst_ip, payload);
    std::cout << "Calculated Checksum: 0x" << std::hex << std::setw(4) << std::setfill('0') 
              << checksum << " (" << std::dec << checksum << ")\n\n";
    
    // Update header with checksum and show final packet
    udp_header.update_checksum(src_ip, dst_ip, payload);
    auto udp_bytes = udp_header.to_bytes();
    std::cout << "UDP Header with checksum: " << bytes_to_hex(udp_bytes) << "\n\n";
    
    // Example 2: IPv6 UDP packet
    std::cout << "2. IPv6 UDP Checksum Calculation\n";
    std::cout << "--------------------------------\n";
    
    IPv6Address src_ipv6("2001:db8::1");
    IPv6Address dst_ipv6("2001:db8::2");
    std::string payload_data_v6 = "IPv6 UDP test";
    std::vector<uint8_t> payload_v6(payload_data_v6.begin(), payload_data_v6.end());
    
    UDPHeader udp_header_v6(54321, 443);
    udp_header_v6.length(static_cast<uint16_t>(UDPHeader::SIZE + payload_v6.size()));
    
    std::cout << "Source IPv6: " << src_ipv6.to_string() << "\n";
    std::cout << "Dest IPv6: " << dst_ipv6.to_string() << "\n";
    std::cout << "Source Port: " << udp_header_v6.src_port() << "\n";
    std::cout << "Dest Port: " << udp_header_v6.dst_port() << "\n";
    std::cout << "UDP Length: " << udp_header_v6.length() << "\n";
    std::cout << "Payload: \"" << payload_data_v6 << "\"\n\n";
    
    // Calculate IPv6 checksum
    uint16_t checksum_v6 = udp_header_v6.calculate_checksum(src_ipv6, dst_ipv6, payload_v6);
    std::cout << "Calculated IPv6 Checksum: 0x" << std::hex << std::setw(4) << std::setfill('0') 
              << checksum_v6 << " (" << std::dec << checksum_v6 << ")\n\n";
    
    // Example 3: Step-by-step breakdown
    std::cout << "3. Step-by-Step Checksum Breakdown (IPv4)\n";
    std::cout << "==========================================\n";
    
    UDPHeader demo_header(1234, 5678);
    std::vector<uint8_t> demo_payload = {0x48, 0x65, 0x6c, 0x6c, 0x6f}; // "Hello"
    demo_header.length(static_cast<uint16_t>(UDPHeader::SIZE + demo_payload.size()));
    
    IPv4Address demo_src("10.0.0.1");
    IPv4Address demo_dst("10.0.0.2");
    
    std::cout << "UDP Header Fields:\n";
    std::cout << "  Source Port: " << demo_header.src_port() << " (0x" << std::hex 
              << demo_header.src_port() << std::dec << ")\n";
    std::cout << "  Dest Port: " << demo_header.dst_port() << " (0x" << std::hex 
              << demo_header.dst_port() << std::dec << ")\n";
    std::cout << "  Length: " << demo_header.length() << " (0x" << std::hex 
              << demo_header.length() << std::dec << ")\n";
    std::cout << "  Checksum: 0x0000 (set to 0 for calculation)\n\n";
    
    std::cout << "IPv4 Pseudo-Header:\n";
    auto src_bytes = demo_src.to_bytes();
    auto dst_bytes = demo_dst.to_bytes();
    std::cout << "  Source IP: " << bytes_to_hex(std::vector<uint8_t>(src_bytes.begin(), src_bytes.end())) << "\n";
    std::cout << "  Dest IP: " << bytes_to_hex(std::vector<uint8_t>(dst_bytes.begin(), dst_bytes.end())) << "\n";
    std::cout << "  Zero + Protocol: 00 11 (Protocol 17 = UDP)\n";
    std::cout << "  UDP Length: " << std::hex << std::setw(2) << std::setfill('0') 
              << ((demo_header.length() >> 8) & 0xFF) << " " 
              << std::setw(2) << std::setfill('0') << (demo_header.length() & 0xFF) << std::dec << "\n\n";
    
    std::cout << "UDP Header + Payload:\n";
    auto demo_bytes = demo_header.to_bytes();
    std::cout << "  Header: " << bytes_to_hex(demo_bytes) << "\n";
    std::cout << "  Payload: " << bytes_to_hex(demo_payload) << "\n\n";
    
    uint16_t final_checksum = demo_header.calculate_checksum(demo_src, demo_dst, demo_payload);
    std::cout << "Final Checksum: 0x" << std::hex << std::setw(4) << std::setfill('0') 
              << final_checksum << " (" << std::dec << final_checksum << ")\n\n";
    
    // Example 4: Zero checksum handling
    std::cout << "4. Special Cases\n";
    std::cout << "================\n";
    std::cout << "IPv4 UDP: Checksum of 0 means 'no checksum computed' (optional)\n";
    std::cout << "IPv6 UDP: Checksum is mandatory, 0 is converted to 0xFFFF\n";
    std::cout << "If calculated checksum = 0x0000, it becomes 0xFFFF\n\n";
    
    return 0;
}
