#include "utils.h"
#include "network_headers.h"
#include <iostream>
#include <iomanip>
#include <cassert>

using namespace cppscapy;
using namespace cppscapy::utils;

void test_ipv4_checksum_verification() {
    std::cout << "=== Testing IPv4 Checksum Verification ===\n\n";
    
    // Test 1: Create a valid IPv4 header and verify its checksum
    std::cout << "Test 1: Valid IPv4 header checksum verification\n";
    
    IPv4Address src_ip("192.168.1.100");
    IPv4Address dst_ip("10.0.0.1");
    
    IPv4Header ipv4(src_ip, dst_ip, IPv4Header::PROTOCOL_TCP);
    ipv4.ttl(64).id(12345).length(40); // 20 (IPv4) + 20 (TCP)
    
    auto header_bytes = ipv4.to_bytes();
    
    std::cout << "IPv4 header (" << header_bytes.size() << " bytes): ";
    for (size_t i = 0; i < header_bytes.size(); ++i) {
        std::cout << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(header_bytes[i]) << " ";
    }
    std::cout << std::dec << "\n";
    
    // Test with vector version
    bool is_valid_vector = verify_ipv4_checksum(header_bytes);
    std::cout << "Vector version checksum valid: " << (is_valid_vector ? "YES" : "NO") << "\n";
    
    // Test with raw pointer version
    bool is_valid_pointer = verify_ipv4_checksum(header_bytes.data(), header_bytes.size());
    std::cout << "Pointer version checksum valid: " << (is_valid_pointer ? "YES" : "NO") << "\n";
    
    std::cout << "\n";
    
    // Test 2: Corrupt the checksum and verify it fails
    std::cout << "Test 2: Corrupted checksum verification\n";
    
    auto corrupted_header = header_bytes;
    corrupted_header[10] ^= 0xFF; // Flip all bits in first checksum byte
    
    bool is_valid_corrupted = verify_ipv4_checksum(corrupted_header);
    std::cout << "Corrupted checksum valid: " << (is_valid_corrupted ? "YES" : "NO") << "\n";
    
    std::cout << "\n";
    
    // Test 3: Test with various header lengths
    std::cout << "Test 3: Different header lengths\n";
    
    // Create header with options (larger than 20 bytes)
    IPv4Header ipv4_with_options(src_ip, dst_ip, IPv4Header::PROTOCOL_UDP);
    ipv4_with_options.ihl(6); // 6 * 4 = 24 bytes
    
    auto extended_header = ipv4_with_options.to_bytes();
    // Pad to 24 bytes (add 4 bytes of options)
    extended_header.resize(24, 0);
    
    // Recalculate checksum for the extended header
    // We need to manually fix the checksum since the library might not handle options properly
    std::vector<uint8_t> temp_header = extended_header;
    temp_header[10] = 0; // Clear checksum field
    temp_header[11] = 0;
    
    uint16_t correct_checksum = calculate_ip_checksum(temp_header);
    extended_header[10] = (correct_checksum >> 8) & 0xFF;
    extended_header[11] = correct_checksum & 0xFF;
    
    bool is_valid_extended = verify_ipv4_checksum(extended_header);
    std::cout << "Extended header (24 bytes) checksum valid: " << (is_valid_extended ? "YES" : "NO") << "\n";
    
    std::cout << "\n";
    
    // Test 4: Edge cases
    std::cout << "Test 4: Edge cases\n";
    
    // Empty vector
    std::vector<uint8_t> empty_header;
    bool is_valid_empty = verify_ipv4_checksum(empty_header);
    std::cout << "Empty header valid: " << (is_valid_empty ? "YES" : "NO") << "\n";
    
    // Too short header
    std::vector<uint8_t> short_header(10, 0);
    bool is_valid_short = verify_ipv4_checksum(short_header);
    std::cout << "Short header (10 bytes) valid: " << (is_valid_short ? "YES" : "NO") << "\n";
    
    // Null pointer
    bool is_valid_null = verify_ipv4_checksum(nullptr, 20);
    std::cout << "Null pointer valid: " << (is_valid_null ? "YES" : "NO") << "\n";
    
    std::cout << "\n";
    
    // Test 5: Real-world example with known good checksum
    std::cout << "Test 5: Manual verification with known values\n";
    
    // Create a simple IPv4 header manually
    std::vector<uint8_t> manual_header = {
        0x45, 0x00, // Version=4, IHL=5, ToS=0
        0x00, 0x1C, // Total Length = 28
        0x00, 0x01, // Identification = 1
        0x40, 0x00, // Flags=0x4 (DF), Fragment Offset=0
        0x40, 0x11, // TTL=64, Protocol=17 (UDP)
        0x00, 0x00, // Checksum = 0 (will be calculated)
        0xC0, 0xA8, 0x01, 0x64, // Source IP = 192.168.1.100
        0x08, 0x08, 0x08, 0x08  // Dest IP = 8.8.8.8
    };
    
    // Calculate correct checksum
    uint16_t manual_checksum = calculate_ip_checksum(manual_header);
    manual_header[10] = (manual_checksum >> 8) & 0xFF;
    manual_header[11] = manual_checksum & 0xFF;
    
    std::cout << "Manual header with calculated checksum: ";
    for (size_t i = 0; i < manual_header.size(); ++i) {
        std::cout << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(manual_header[i]) << " ";
    }
    std::cout << std::dec << "\n";
    
    bool is_valid_manual = verify_ipv4_checksum(manual_header);
    std::cout << "Manual header checksum valid: " << (is_valid_manual ? "YES" : "NO") << "\n";
    
    std::cout << "\n=== IPv4 Checksum Verification Tests Complete ===\n";
}

int main() {
    test_ipv4_checksum_verification();
    return 0;
}
