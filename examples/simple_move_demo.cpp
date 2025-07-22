#include "../include/utils.h"
#include <iostream>

using namespace cppscapy::utils;

int main() {
    std::cout << "=== Move Semantics Examples ===\n\n";
    
    // Example 1: Move semantics (EFFICIENT - O(1))
    std::cout << "1. Move semantics (return value -> variable):\n";
    std::vector<uint8_t> payload = random::incremental_pattern(10);
    std::cout << "   Pattern created and moved efficiently\n";
    std::cout << "   First 10 bytes: ";
    for (size_t i = 0; i < std::min(payload.size(), size_t(10)); ++i) {
        std::cout << static_cast<int>(payload[i]) << " ";
    }
    std::cout << "\n\n";
    
    // Example 2: Auto deduction with move (EFFICIENT - O(1))
    std::cout << "2. Auto deduction with move:\n";
    auto payload2 = random::random_bytes(10);
    std::cout << "   Random payload created and moved efficiently\n";
    std::cout << "   Size: " << payload2.size() << " bytes\n\n";
    
    // Example 3: Multiple assignments (all moves)
    std::cout << "3. Multiple assignments (all efficient moves):\n";
    std::vector<uint8_t> p1 = random::incremental_pattern(5, 100);
    std::vector<uint8_t> p2 = random::random_ascii(5);
    std::vector<uint8_t> p3 = random::random_alphanumeric(5);
    
    std::cout << "   All three payloads created efficiently\n";
    std::cout << "   p1 size: " << p1.size() << ", p2 size: " << p2.size() 
              << ", p3 size: " << p3.size() << "\n\n";
    
    // Example 4: What happens under the hood
    std::cout << "4. What happens under the hood:\n";
    std::cout << "   // This line:\n";
    std::cout << "   std::vector<uint8_t> data = random::incremental_pattern(1000);\n";
    std::cout << "   \n";
    std::cout << "   // Internally does:\n";
    std::cout << "   // 1. Function creates vector and fills it (allocation)\n";
    std::cout << "   // 2. Return value is moved (no copy, just pointer transfer)\n";
    std::cout << "   // 3. Variable 'data' now owns the memory (O(1) operation)\n";
    std::cout << "   // 4. No duplicate memory allocation or byte copying\n\n";
    
    // Example 5: Performance comparison concept
    std::vector<uint8_t> efficient_way = random::incremental_pattern(1000);
    
    // This would be inefficient (but modern compilers optimize it anyway):
    // std::vector<uint8_t> temp = random::incremental_pattern(1000);
    // std::vector<uint8_t> inefficient_way = temp;  // Copy
    
    std::cout << "5. Key takeaways:\n";
    std::cout << "   ✓ Direct assignment: std::vector<uint8_t> v = function(); // MOVE\n";
    std::cout << "   ✓ Auto assignment: auto v = function(); // MOVE\n";
    std::cout << "   ✓ Move is O(1) - just transfers ownership\n";
    std::cout << "   ✓ Copy would be O(n) - duplicates all bytes\n";
    std::cout << "   ✓ Pattern functions return by value safely and efficiently\n";
    
    return 0;
}
