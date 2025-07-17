#include "utils.h"
#include <iostream>
#include <iomanip>

using namespace cppscapy;
using namespace cppscapy::utils;

// Helper function to print arrays nicely
template<size_t N>
void print_array_hex(const std::array<uint8_t, N>& arr, const std::string& label) {
    std::cout << label << ": ";
    for (size_t i = 0; i < N; ++i) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(arr[i]);
        if (i < N - 1) std::cout << " ";
    }
    std::cout << std::dec << std::endl;
}

int main() {
    std::cout << "=== Compile-Time Length Deduction Demo ===\n\n";
    
    // 1. Basic from_hex_string_auto usage
    std::cout << "1. Basic from_hex_string_auto usage:\n";
    
    constexpr auto mac_bytes = from_hex_string_auto("001122334455");
    constexpr auto ipv4_bytes = from_hex_string_auto("C0A80101");
    constexpr auto custom_2bytes = from_hex_string_auto("ABCD");
    constexpr auto single_byte = from_hex_string_auto("FF");
    
    print_array_hex(mac_bytes, "MAC (6 bytes)");
    print_array_hex(ipv4_bytes, "IPv4 (4 bytes)");
    print_array_hex(custom_2bytes, "Custom (2 bytes)");
    print_array_hex(single_byte, "Single byte");
    
    std::cout << "\n";
    
    // 2. Using convenience functions
    std::cout << "2. Using convenience functions:\n";
    
    auto mac_addr = make_mac_address("FFFFFFFFFFFF");
    auto ipv4_addr = make_ipv4_address("08080808");  // 8.8.8.8
    
    std::cout << "MAC Address: " << mac_addr.to_string() << std::endl;
    std::cout << "IPv4 Address: " << ipv4_addr.to_string() << std::endl;
    
    std::cout << "\n";
    
    // 3. Template deduction in action
    std::cout << "3. Template deduction in action:\n";
    
    // These all deduce different array sizes automatically
    constexpr auto short_key = from_hex_string_auto("1234");        // 2 bytes
    constexpr auto medium_key = from_hex_string_auto("123456789A"); // 5 bytes
    constexpr auto long_key = from_hex_string_auto("0123456789ABCDEF0123456789ABCDEF"); // 16 bytes
    
    std::cout << "Short key size: " << short_key.size() << " bytes" << std::endl;
    std::cout << "Medium key size: " << medium_key.size() << " bytes" << std::endl;
    std::cout << "Long key size: " << long_key.size() << " bytes" << std::endl;
    
    print_array_hex(short_key, "Short key");
    print_array_hex(medium_key, "Medium key");
    print_array_hex(long_key, "Long key");
    
    std::cout << "\n";
    
    // 4. Compile-time validation
    std::cout << "4. Compile-time validation:\n";
    
    // All these are validated at compile time
    static_assert(from_hex_string_auto("12").size() == 1, "Single byte");
    static_assert(from_hex_string_auto("1234").size() == 2, "Two bytes");
    static_assert(from_hex_string_auto("123456").size() == 3, "Three bytes");
    static_assert(from_hex_string_auto("12345678").size() == 4, "Four bytes");
    static_assert(from_hex_string_auto("1234567890").size() == 5, "Five bytes");
    static_assert(from_hex_string_auto("123456789012").size() == 6, "Six bytes");
    
    std::cout << "All compile-time assertions passed!" << std::endl;
    
    std::cout << "\n";
    
    // 5. Round-trip testing
    std::cout << "5. Round-trip testing:\n";
    
    constexpr auto original_data = from_hex_string_auto("DEADBEEFCAFE");
    constexpr auto hex_string = to_hex_string_array(original_data);
    
    std::cout << "Original: ";
    print_array_hex(original_data, "");
    std::cout << "Hex string: " << hex_string.data() << std::endl;
    
    // Parse it back
    auto parsed_back = from_hex_string_array<original_data.size()>(hex_string.data());
    
    bool round_trip_success = true;
    for (size_t i = 0; i < original_data.size(); ++i) {
        if (original_data[i] != parsed_back[i]) {
            round_trip_success = false;
            break;
        }
    }
    
    std::cout << "Round-trip test: " << (round_trip_success ? "PASS" : "FAIL") << std::endl;
    
    std::cout << "\n";
    
    // 6. Case insensitive parsing
    std::cout << "6. Case insensitive parsing:\n";
    
    constexpr auto upper_case = from_hex_string_auto("DEADBEEF");
    constexpr auto lower_case = from_hex_string_auto("deadbeef");
    constexpr auto mixed_case = from_hex_string_auto("DeAdBeEf");
    
    bool case_insensitive_success = true;
    for (size_t i = 0; i < upper_case.size(); ++i) {
        if (upper_case[i] != lower_case[i] || upper_case[i] != mixed_case[i]) {
            case_insensitive_success = false;
            break;
        }
    }
    
    std::cout << "Case insensitive test: " << (case_insensitive_success ? "PASS" : "FAIL") << std::endl;
    
    print_array_hex(upper_case, "Upper case");
    print_array_hex(lower_case, "Lower case");
    print_array_hex(mixed_case, "Mixed case");
    
    std::cout << "\n=== All tests completed successfully! ===\n";
    
    return 0;
}
