#include "utils.h"
#include "network_headers.h"
#include <iostream>
#include <iomanip>
#include <cassert>

using namespace cppscapy;
using namespace cppscapy::utils;

void test_checksum_calculation_methods() {
    std::cout << "=== Testing IPv4 Checksum Calculation Methods ===\n\n";
    
    // Create a test IPv4 header
    IPv4Address src_ip("192.168.1.100");
    IPv4Address dst_ip("10.0.0.1");
    IPv4Header ipv4(src_ip, dst_ip, IPv4Header::PROTOCOL_TCP);
    ipv4.ttl(64).id(12345).length(40);
    
    auto header_bytes = ipv4.to_bytes();
    
    std::cout << "Original IPv4 header with calculated checksum:\n";
    for (size_t i = 0; i < header_bytes.size(); ++i) {
        std::cout << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(header_bytes[i]) << " ";
        if ((i + 1) % 8 == 0) std::cout << "\n";
    }
    std::cout << std::dec << "\n\n";
    
    // Get the checksum that was calculated by the IPv4Header class
    uint16_t original_checksum = (header_bytes[10] << 8) | header_bytes[11];
    std::cout << "Original checksum from IPv4Header: 0x" << std::hex << original_checksum << std::dec << "\n\n";
    
    // Test 1: Using the new calculate_ipv4_header_checksum function
    std::cout << "Test 1: Using calculate_ipv4_header_checksum (auto-clears checksum field)\n";
    uint16_t checksum1 = calculate_ipv4_header_checksum(header_bytes);
    std::cout << "Calculated checksum: 0x" << std::hex << checksum1 << std::dec << "\n";
    std::cout << "Matches original: " << (checksum1 == original_checksum ? "YES" : "NO") << "\n\n";
    
    // Test 2: Using calculate_ip_checksum with manually cleared checksum field
    std::cout << "Test 2: Using calculate_ip_checksum with manually cleared checksum field\n";
    auto header_copy = header_bytes;
    header_copy[10] = 0; // Clear checksum field
    header_copy[11] = 0;
    uint16_t checksum2 = calculate_ip_checksum(header_copy);
    std::cout << "Calculated checksum: 0x" << std::hex << checksum2 << std::dec << "\n";
    std::cout << "Matches original: " << (checksum2 == original_checksum ? "YES" : "NO") << "\n\n";
    
    // Test 3: Using calculate_ip_checksum with non-cleared checksum field (WRONG way)
    std::cout << "Test 3: Using calculate_ip_checksum WITHOUT clearing checksum field (WRONG)\n";
    uint16_t checksum3 = calculate_ip_checksum(header_bytes);
    std::cout << "Calculated checksum: 0x" << std::hex << checksum3 << std::dec << "\n";
    std::cout << "Matches original: " << (checksum3 == original_checksum ? "YES" : "NO") << "\n";
    std::cout << "This should be different because the checksum field wasn't cleared!\n\n";
    
    // Test 4: Verify all our calculation methods work with verification
    std::cout << "Test 4: Verification tests\n";
    
    // Create a header with the checksum from method 1
    auto test_header1 = header_bytes;
    test_header1[10] = (checksum1 >> 8) & 0xFF;
    test_header1[11] = checksum1 & 0xFF;
    bool verify1 = verify_ipv4_checksum(test_header1);
    std::cout << "Header with checksum1 verifies: " << (verify1 ? "YES" : "NO") << "\n";
    
    // Create a header with the checksum from method 2
    auto test_header2 = header_bytes;
    test_header2[10] = (checksum2 >> 8) & 0xFF;
    test_header2[11] = checksum2 & 0xFF;
    bool verify2 = verify_ipv4_checksum(test_header2);
    std::cout << "Header with checksum2 verifies: " << (verify2 ? "YES" : "NO") << "\n";
    
    // Original header should also verify
    bool verify_original = verify_ipv4_checksum(header_bytes);
    std::cout << "Original header verifies: " << (verify_original ? "YES" : "NO") << "\n\n";
    
    // Test 5: Test with a manually created header
    std::cout << "Test 5: Manual header creation and checksum calculation\n";
    
    std::vector<uint8_t> manual_header = {
        0x45, 0x00, // Version=4, IHL=5, ToS=0
        0x00, 0x1C, // Total Length = 28
        0x00, 0x01, // Identification = 1
        0x40, 0x00, // Flags=0x4 (DF), Fragment Offset=0
        0x40, 0x11, // TTL=64, Protocol=17 (UDP)
        0x00, 0x00, // Checksum = 0 (cleared)
        0xC0, 0xA8, 0x01, 0x64, // Source IP = 192.168.1.100
        0x08, 0x08, 0x08, 0x08  // Dest IP = 8.8.8.8
    };
    
    std::cout << "Manual header (checksum field already cleared):\n";
    for (size_t i = 0; i < manual_header.size(); ++i) {
        std::cout << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(manual_header[i]) << " ";
    }
    std::cout << std::dec << "\n";
    
    uint16_t manual_checksum1 = calculate_ipv4_header_checksum(manual_header);
    uint16_t manual_checksum2 = calculate_ip_checksum(manual_header);
    
    std::cout << "calculate_ipv4_header_checksum: 0x" << std::hex << manual_checksum1 << std::dec << "\n";
    std::cout << "calculate_ip_checksum: 0x" << std::hex << manual_checksum2 << std::dec << "\n";
    std::cout << "Both methods match: " << (manual_checksum1 == manual_checksum2 ? "YES" : "NO") << "\n\n";
    
    // Set the checksum and verify
    manual_header[10] = (manual_checksum1 >> 8) & 0xFF;
    manual_header[11] = manual_checksum1 & 0xFF;
    
    std::cout << "Manual header with calculated checksum:\n";
    for (size_t i = 0; i < manual_header.size(); ++i) {
        std::cout << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(manual_header[i]) << " ";
    }
    std::cout << std::dec << "\n";
    
    bool manual_verify = verify_ipv4_checksum(manual_header);
    std::cout << "Manual header verifies: " << (manual_verify ? "YES" : "NO") << "\n\n";
    
    std::cout << "=== Key Takeaways ===\n";
    std::cout << "1. When calculating IPv4 checksum, the checksum field MUST be cleared first\n";
    std::cout << "2. calculate_ipv4_header_checksum() automatically handles this\n";
    std::cout << "3. calculate_ip_checksum() is generic and requires manual clearing\n";
    std::cout << "4. Both methods produce identical results when used correctly\n";
    std::cout << "5. The verification function correctly handles the checksum field\n\n";
    
    std::cout << "=== IPv4 Checksum Calculation Tests Complete ===\n";
}

int main() {
    test_checksum_calculation_methods();
    return 0;
}
