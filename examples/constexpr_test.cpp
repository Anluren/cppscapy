#include "../include/utils.h"
#include <iostream>
#include <iomanip>

using namespace cppscapy::utils;

int main() {
    std::cout << "=== Testing constexpr Utils Functions ===\n";
    
    // Test constexpr hex string to array conversion
    std::cout << "\n1. Testing constexpr hex string to array conversion:\n";
    
    // These should be evaluated at compile time
    constexpr auto mac_array = mac_from_hex_string("aa:bb:cc:dd:ee:ff");
    constexpr auto ipv4_array = ipv4_from_hex_string("c0a80101"); // 192.168.1.1
    constexpr auto custom_array = from_hex_string_array<3>("aabbcc");
    
    std::cout << "Constexpr MAC array: ";
    for (size_t i = 0; i < mac_array.size(); ++i) {
        if (i > 0) std::cout << ":";
        std::cout << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(mac_array[i]);
    }
    std::cout << std::dec << "\n";
    
    std::cout << "Constexpr IPv4 array: ";
    for (size_t i = 0; i < ipv4_array.size(); ++i) {
        if (i > 0) std::cout << ".";
        std::cout << static_cast<int>(ipv4_array[i]);
    }
    std::cout << "\n";
    
    std::cout << "Constexpr custom array: ";
    for (size_t i = 0; i < custom_array.size(); ++i) {
        std::cout << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(custom_array[i]) << " ";
    }
    std::cout << std::dec << "\n";
    
    // Test constexpr array to hex string conversion
    std::cout << "\n2. Testing constexpr array to hex string conversion:\n";
    
    constexpr auto hex_string_mac = to_hex_string_array(mac_array);
    constexpr auto hex_string_ipv4 = to_hex_string_array(ipv4_array);
    constexpr auto hex_string_custom = to_hex_string_array(custom_array);
    
    std::cout << "MAC to hex string: " << hex_string_mac.data() << "\n";
    std::cout << "IPv4 to hex string: " << hex_string_ipv4.data() << "\n";
    std::cout << "Custom to hex string: " << hex_string_custom.data() << "\n";
    
    // Test round-trip conversion
    std::cout << "\n3. Testing round-trip conversion:\n";
    
    constexpr auto original = from_hex_string_array<4>("deadbeef");
    constexpr auto hex_str = to_hex_string_array(original);
    constexpr auto recovered = from_hex_string_array<4>(hex_str.data());
    
    std::cout << "Original: ";
    for (auto byte : original) {
        std::cout << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(byte) << " ";
    }
    std::cout << "\n";
    
    std::cout << "Hex string: " << hex_str.data() << "\n";
    
    std::cout << "Recovered: ";
    for (auto byte : recovered) {
        std::cout << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(byte) << " ";
    }
    std::cout << std::dec << "\n";
    
    // Verify they match
    bool match = true;
    for (size_t i = 0; i < original.size(); ++i) {
        if (original[i] != recovered[i]) {
            match = false;
            break;
        }
    }
    std::cout << "Round-trip match: " << (match ? "Yes" : "No") << "\n";
    
    std::cout << "\n=== Constexpr Tests Complete ===\n";
    
    return 0;
}
