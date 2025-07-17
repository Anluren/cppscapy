#include "utils.h"
#include <iostream>
#include <cassert>

using namespace cppscapy::utils;

int main() {
    std::cout << "Testing from_hex_string_auto with compile-time length deduction...\n\n";
    
    // Test 1: MAC address (6 bytes from 12 hex chars)
    constexpr auto mac_bytes = from_hex_string_auto("001122334455");
    static_assert(mac_bytes.size() == 6, "MAC address should be 6 bytes");
    
    std::cout << "MAC from hex \"001122334455\":\n";
    for (size_t i = 0; i < mac_bytes.size(); ++i) {
        std::cout << std::hex << "0x" << static_cast<int>(mac_bytes[i]);
        if (i < mac_bytes.size() - 1) std::cout << " ";
    }
    std::cout << std::dec << "\n\n";
    
    // Test 2: IPv4 address (4 bytes from 8 hex chars)
    constexpr auto ipv4_bytes = from_hex_string_auto("C0A80101");  // 192.168.1.1
    static_assert(ipv4_bytes.size() == 4, "IPv4 address should be 4 bytes");
    
    std::cout << "IPv4 from hex \"C0A80101\":\n";
    for (size_t i = 0; i < ipv4_bytes.size(); ++i) {
        std::cout << static_cast<int>(ipv4_bytes[i]);
        if (i < ipv4_bytes.size() - 1) std::cout << ".";
    }
    std::cout << "\n\n";
    
    // Test 3: Custom length (2 bytes from 4 hex chars)
    constexpr auto custom_bytes = from_hex_string_auto("ABCD");
    static_assert(custom_bytes.size() == 2, "Custom array should be 2 bytes");
    
    std::cout << "Custom from hex \"ABCD\":\n";
    for (size_t i = 0; i < custom_bytes.size(); ++i) {
        std::cout << std::hex << "0x" << static_cast<int>(custom_bytes[i]);
        if (i < custom_bytes.size() - 1) std::cout << " ";
    }
    std::cout << std::dec << "\n\n";
    
    // Test 4: Single byte
    constexpr auto single_byte = from_hex_string_auto("FF");
    static_assert(single_byte.size() == 1, "Single byte array should be 1 byte");
    
    std::cout << "Single byte from hex \"FF\": 0x" << std::hex << static_cast<int>(single_byte[0]) << std::dec << "\n\n";
    
    // Test 5: Lowercase hex
    constexpr auto lower_hex = from_hex_string_auto("deadbeef");
    static_assert(lower_hex.size() == 4, "Lower hex should be 4 bytes");
    
    std::cout << "Lowercase hex \"deadbeef\":\n";
    for (size_t i = 0; i < lower_hex.size(); ++i) {
        std::cout << std::hex << "0x" << static_cast<int>(lower_hex[i]);
        if (i < lower_hex.size() - 1) std::cout << " ";
    }
    std::cout << std::dec << "\n\n";
    
    // Test 6: Using with network headers
    constexpr auto mac_data = from_hex_string_auto("001122334455");
    cppscapy::MacAddress mac_addr(mac_data);
    
    std::cout << "Created MacAddress from auto-parsed hex: " << mac_addr.to_string() << "\n\n";
    
    // Test 7: Compare with manual array
    constexpr std::array<uint8_t, 3> manual_array = {0x12, 0x34, 0x56};
    constexpr auto parsed_array = from_hex_string_auto("123456");
    
    bool arrays_equal = true;
    for (size_t i = 0; i < 3; ++i) {
        if (manual_array[i] != parsed_array[i]) {
            arrays_equal = false;
            break;
        }
    }
    
    std::cout << "Manual vs parsed array comparison: " << (arrays_equal ? "MATCH" : "MISMATCH") << "\n\n";
    
    std::cout << "All tests passed! The from_hex_string_auto function successfully deduces array size at compile time.\n";
    
    return 0;
}
