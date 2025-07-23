#include "network_headers.h"
#include <iostream>
#include <iomanip>

using namespace cppscapy;

void demonstrate_pseudo_header_importance() {
    std::cout << "=== Why UDP Checksum Includes IP Header Info ===\n\n";
    
    // Same UDP header and payload, different destinations
    std::string payload = "Hello, World!";
    std::vector<uint8_t> payload_bytes(payload.begin(), payload.end());
    
    UDPHeader udp_header(12345, 80);
    udp_header.length(static_cast<uint16_t>(UDPHeader::SIZE + payload_bytes.size()));
    
    // Scenario 1: Correct destination
    IPv4Address src_ip("192.168.1.10");
    IPv4Address correct_dst("192.168.1.20");
    
    uint16_t correct_checksum = udp_header.calculate_checksum(src_ip, correct_dst, payload_bytes);
    
    std::cout << "Scenario 1: Correct Packet\n";
    std::cout << "Source: " << src_ip.to_string() << "\n";
    std::cout << "Destination: " << correct_dst.to_string() << "\n";
    std::cout << "UDP Payload: \"" << payload << "\"\n";
    std::cout << "Calculated Checksum: 0x" << std::hex << std::setw(4) << std::setfill('0') 
              << correct_checksum << std::dec << "\n\n";
    
    // Scenario 2: Misdelivered packet (wrong destination)
    IPv4Address wrong_dst("192.168.1.30");  // Wrong destination!
    
    uint16_t wrong_checksum = udp_header.calculate_checksum(src_ip, wrong_dst, payload_bytes);
    
    std::cout << "Scenario 2: Misdelivered Packet\n";
    std::cout << "Source: " << src_ip.to_string() << "\n";
    std::cout << "Destination: " << wrong_dst.to_string() << " (WRONG!)\n";
    std::cout << "UDP Payload: \"" << payload << "\" (identical)\n";
    std::cout << "Calculated Checksum: 0x" << std::hex << std::setw(4) << std::setfill('0') 
              << wrong_checksum << std::dec << "\n\n";
    
    std::cout << "Analysis:\n";
    std::cout << "- UDP header and payload are IDENTICAL\n";
    std::cout << "- Only the destination IP differs\n";
    std::cout << "- Checksums are DIFFERENT: 0x" << std::hex << correct_checksum 
              << " vs 0x" << wrong_checksum << std::dec << "\n";
    std::cout << "- This allows detection of misdelivered packets!\n\n";
    
    // Scenario 3: What if we only checksummed UDP header + payload?
    std::cout << "Scenario 3: If we ignored IP addresses\n";
    std::cout << "- Both packets would have the same checksum\n";
    std::cout << "- Misdelivered packet would appear valid\n";
    std::cout << "- Application would receive wrong data\n";
    std::cout << "- No way to detect the error!\n\n";
    
    // Scenario 4: Source IP spoofing detection
    IPv4Address spoofed_src("10.0.0.1");  // Spoofed source
    uint16_t spoofed_checksum = udp_header.calculate_checksum(spoofed_src, correct_dst, payload_bytes);
    
    std::cout << "Scenario 4: Source IP Spoofing\n";
    std::cout << "Real Source: " << src_ip.to_string() << "\n";
    std::cout << "Spoofed Source: " << spoofed_src.to_string() << "\n";
    std::cout << "Destination: " << correct_dst.to_string() << "\n";
    std::cout << "Original Checksum: 0x" << std::hex << correct_checksum << "\n";
    std::cout << "Spoofed Checksum: 0x" << spoofed_checksum << std::dec << "\n";
    std::cout << "- Different checksums help detect source spoofing\n\n";
}

void demonstrate_pseudo_header_contents() {
    std::cout << "=== Pseudo-Header Contents ===\n\n";
    
    IPv4Address src("10.0.0.1");
    IPv4Address dst("10.0.0.2");
    
    std::cout << "IPv4 Pseudo-Header Structure:\n";
    std::cout << "+--------+--------+--------+--------+\n";
    std::cout << "|           Source Address          |  <- " << src.to_string() << "\n";
    std::cout << "+--------+--------+--------+--------+\n";
    std::cout << "|         Destination Address       |  <- " << dst.to_string() << "\n";
    std::cout << "+--------+--------+--------+--------+\n";
    std::cout << "|  zero  |Protocol|   UDP Length    |  <- 0, 17 (UDP), length\n";
    std::cout << "+--------+--------+--------+--------+\n\n";
    
    std::cout << "This pseudo-header is prepended to:\n";
    std::cout << "- UDP Header (8 bytes)\n";
    std::cout << "- UDP Payload (variable)\n";
    std::cout << "- Padding (if needed for 16-bit alignment)\n\n";
    
    std::cout << "Key Points:\n";
    std::cout << "1. Pseudo-header is NOT transmitted on the wire\n";
    std::cout << "2. It's only used for checksum calculation\n";
    std::cout << "3. Both sender and receiver must calculate it identically\n";
    std::cout << "4. Provides end-to-end integrity across network + transport layers\n\n";
}

void demonstrate_ipv6_differences() {
    std::cout << "=== IPv6 UDP Checksum Differences ===\n\n";
    
    IPv6Address src_v6("2001:db8::1");
    IPv6Address dst_v6("2001:db8::2");
    
    std::string payload = "IPv6 test";
    std::vector<uint8_t> payload_bytes(payload.begin(), payload.end());
    
    UDPHeader udp_v6(8080, 443);
    udp_v6.length(static_cast<uint16_t>(UDPHeader::SIZE + payload_bytes.size()));
    
    uint16_t ipv6_checksum = udp_v6.calculate_checksum(src_v6, dst_v6, payload_bytes);
    
    std::cout << "IPv6 UDP Checksum: 0x" << std::hex << std::setw(4) << std::setfill('0') 
              << ipv6_checksum << std::dec << "\n\n";
    
    std::cout << "IPv6 Differences:\n";
    std::cout << "1. Pseudo-header is 40 bytes (vs 12 for IPv4)\n";
    std::cout << "2. Includes 128-bit source and destination addresses\n";
    std::cout << "3. UDP checksum is MANDATORY (not optional like IPv4)\n";
    std::cout << "4. Next Header field = 17 (UDP protocol number)\n\n";
}

int main() {
    demonstrate_pseudo_header_importance();
    demonstrate_pseudo_header_contents();
    demonstrate_ipv6_differences();
    
    std::cout << "=== Summary ===\n";
    std::cout << "The UDP pseudo-header serves critical purposes:\n";
    std::cout << "✓ Detects misdelivered packets\n";
    std::cout << "✓ Helps identify source IP spoofing\n";
    std::cout << "✓ Provides end-to-end data integrity\n";
    std::cout << "✓ Ensures packets reach the correct destination\n";
    std::cout << "✓ Minimal overhead (no extra header fields)\n\n";
    
    return 0;
}
