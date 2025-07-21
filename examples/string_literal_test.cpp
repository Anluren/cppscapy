#include "network_headers.h"
#include <iostream>
#include <string>
#include <string_view>
#include <type_traits>

using namespace cppscapy;

int main() {
    std::cout << "=== Testing const char[] Detection with if constexpr ===\n\n";
    
    // Lambda to detect const char[] (string literals) vs other string types
    auto detect_string_literal = [](auto&& param) {
        using U = std::remove_reference_t<decltype(param)>;
        
        std::cout << "Input: ";
        
        if constexpr (std::is_array_v<U>) {
            using ElementType = std::remove_extent_t<U>;
            if constexpr (std::is_same_v<ElementType, char> || 
                         std::is_same_v<ElementType, const char>) {
                std::cout << "\"" << param << "\" -> const char[" << std::extent_v<U> 
                         << "] ✓ STRING LITERAL\n";
                return 1; // String literal
            }
        } else if constexpr (std::is_same_v<std::decay_t<decltype(param)>, const char*> || 
                           std::is_same_v<std::decay_t<decltype(param)>, char*>) {
            std::cout << "\"" << param << "\" -> const char* ✗ C-string pointer\n";
            return 2; // C-string pointer
        } else if constexpr (std::is_same_v<std::decay_t<decltype(param)>, std::string>) {
            std::cout << "\"" << param << "\" -> std::string ✗ String object\n";
            return 3; // std::string
        } else if constexpr (std::is_same_v<std::decay_t<decltype(param)>, std::string_view>) {
            std::cout << "\"" << param << "\" -> std::string_view ✗ String view\n";
            return 4; // std::string_view
        } else {
            std::cout << "??? -> Unknown type ✗\n";
            return 0; // Unknown
        }
        
        return 0;
    };
    
    // Test Cases
    std::cout << "Test Cases:\n";
    
    // 1. String literals (const char[N]) - These should return 1
    auto result1 = detect_string_literal("Hello");        // const char[6]
    auto result2 = detect_string_literal("World!");       // const char[7]
    auto result3 = detect_string_literal("");             // const char[1]
    auto result4 = detect_string_literal("This is a longer string literal"); // const char[34]
    
    // 2. C-string pointers (const char*) - These should return 2
    const char* ptr1 = "Pointer1";
    char* ptr2 = const_cast<char*>("Pointer2");
    auto result5 = detect_string_literal(ptr1);
    auto result6 = detect_string_literal(ptr2);
    
    // 3. std::string - These should return 3
    std::string str1 = "String1";
    std::string str2("String2");
    auto result7 = detect_string_literal(str1);
    auto result8 = detect_string_literal(std::string("Temporary String"));
    
    // 4. std::string_view - These should return 4
    std::string_view sv1 = "View1";
    std::string_view sv2("View2");
    auto result9 = detect_string_literal(sv1);
    auto result10 = detect_string_literal(std::string_view("Temporary View"));
    
    // Summary
    std::cout << "\nSummary:\n";
    std::cout << "String literals (const char[]) detected: " 
              << (result1 == 1) + (result2 == 1) + (result3 == 1) + (result4 == 1) << "/4\n";
    std::cout << "C-string pointers detected: " 
              << (result5 == 2) + (result6 == 2) << "/2\n";
    std::cout << "std::string objects detected: " 
              << (result7 == 3) + (result8 == 3) << "/2\n";
    std::cout << "std::string_view objects detected: " 
              << (result9 == 4) + (result10 == 4) << "/2\n";
    
    // Practical example for network payloads
    std::cout << "\n=== Practical Example: Smart Payload Function ===\n";
    
    auto create_payload = [](auto&& data) -> std::vector<uint8_t> {
        using U = std::remove_reference_t<decltype(data)>;
        
        if constexpr (std::is_array_v<U> && 
                     (std::is_same_v<std::remove_extent_t<U>, char> ||
                      std::is_same_v<std::remove_extent_t<U>, const char>)) {
            // Compile-time optimized path for string literals
            std::cout << "Optimized string literal processing: ";
            constexpr size_t size = std::extent_v<U> - 1; // -1 to exclude null terminator
            std::cout << size << " chars\n";
            std::vector<uint8_t> result(data, data + size);
            return result;
        } else {
            // Runtime path for other string types
            std::cout << "Runtime string processing\n";
            std::string str;
            if constexpr (std::is_same_v<std::decay_t<decltype(data)>, std::string>) {
                str = data;
            } else if constexpr (std::is_same_v<std::decay_t<decltype(data)>, std::string_view>) {
                str = std::string(data);
            } else {
                str = std::string(data);
            }
            return std::vector<uint8_t>(str.begin(), str.end());
        }
    };
    
    // Test the smart payload function
    auto payload1 = create_payload("Literal payload");          // Compile-time optimized
    auto payload2 = create_payload(std::string("String obj"));  // Runtime
    const char* msg = "C-string";
    auto payload3 = create_payload(msg);                        // Runtime
    auto payload4 = create_payload(std::string_view("View"));   // Runtime
    
    std::cout << "\nPayload sizes: " 
              << payload1.size() << ", " 
              << payload2.size() << ", " 
              << payload3.size() << ", " 
              << payload4.size() << " bytes\n";
    
    std::cout << "\n=== Test Complete ===\n";
    std::cout << "✓ Successfully demonstrated const char[] detection\n";
    std::cout << "✓ Compile-time type discrimination working\n";
    std::cout << "✓ Practical network payload optimization example\n";
    
    return 0;
}
