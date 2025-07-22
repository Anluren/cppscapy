#include "../include/utils.h"
#include <iostream>

using namespace cppscapy::utils;

int main() {
    std::cout << "=== Testing Incremental Pattern Generation ===" << std::endl;
    
    // Test 1: Basic incremental pattern starting from 0
    std::cout << "\n1. Basic incremental pattern (20 bytes, start from 0):" << std::endl;
    auto pattern1 = random::incremental_pattern(20);
    std::cout << "Values: ";
    for (size_t i = 0; i < pattern1.size(); ++i) {
        std::cout << static_cast<int>(pattern1[i]);
        if (i < pattern1.size() - 1) std::cout << ", ";
    }
    std::cout << std::endl;
    
    // Test 2: Incremental pattern starting from a different value
    std::cout << "\n2. Incremental pattern starting from 250 (10 bytes):" << std::endl;
    auto pattern2 = random::incremental_pattern(10, 250);
    std::cout << "Values: ";
    for (size_t i = 0; i < pattern2.size(); ++i) {
        std::cout << static_cast<int>(pattern2[i]);
        if (i < pattern2.size() - 1) std::cout << ", ";
    }
    std::cout << std::endl;
    
    // Test 3: Large pattern to show wrapping
    std::cout << "\n3. Large incremental pattern (300 bytes, showing wrap-around):" << std::endl;
    auto pattern3 = random::incremental_pattern(300);
    std::cout << "First 20 values: ";
    for (int i = 0; i < 20; ++i) {
        std::cout << static_cast<int>(pattern3[i]);
        if (i < 19) std::cout << ", ";
    }
    std::cout << std::endl;
    
    std::cout << "Values around wrap (positions 250-260): ";
    for (int i = 250; i < 261 && i < static_cast<int>(pattern3.size()); ++i) {
        std::cout << static_cast<int>(pattern3[i]);
        if (i < 260) std::cout << ", ";
    }
    std::cout << std::endl;
    
    // Test 4: Verify the pattern is truly incremental
    std::cout << "\n4. Verification test (100 bytes):" << std::endl;
    auto pattern4 = random::incremental_pattern(100, 50);
    bool is_correct = true;
    uint8_t expected = 50;
    
    for (size_t i = 0; i < pattern4.size(); ++i) {
        if (pattern4[i] != expected) {
            std::cout << "Error at position " << i << ": expected " 
                      << static_cast<int>(expected) << ", got " 
                      << static_cast<int>(pattern4[i]) << std::endl;
            is_correct = false;
            break;
        }
        expected = (expected + 1) % 256;
    }
    
    if (is_correct) {
        std::cout << "✓ Pattern verification passed!" << std::endl;
    }
    
    // Test 5: Hex display for visual inspection
    std::cout << "\n5. Hex display of 32-byte incremental pattern:" << std::endl;
    auto pattern5 = random::incremental_pattern(32);
    print_hex(pattern5, "Incremental Pattern (0-31)");
    
    std::cout << "\n6. Hex display showing wrap-around (start from 240, 32 bytes):" << std::endl;
    auto pattern6 = random::incremental_pattern(32, 240);
    print_hex(pattern6, "Incremental Pattern (240-255, 0-15)");
    
    std::cout << "\n=== All tests completed! ===" << std::endl;
    return 0;
}
