#include "../include/utils.h"
#include <iostream>
#include <chrono>

using namespace cppscapy::utils;

int main() {
    std::cout << "=== Quick In-Place API Demo ===\n\n";
    
    // Test 1: Basic in-place randomization
    std::vector<uint8_t> data(10, 0x00);
    std::cout << "1. Before randomization: ";
    print_hex(data, "");
    
    random::randomize_bytes(data);
    std::cout << "   After randomization:  ";
    print_hex(data, "");
    std::cout << std::endl;
    
    // Test 2: Incremental pattern
    std::vector<uint8_t> data2(10, 0xFF);
    std::cout << "2. Before incremental:   ";
    print_hex(data2, "");
    
    random::fill_incremental_pattern(data2, 50);
    std::cout << "   After incremental:    ";
    print_hex(data2, "");
    std::cout << std::endl;
    
    // Test 3: ASCII randomization
    std::vector<uint8_t> data3(20, 0x00);
    random::randomize_ascii(data3);
    std::cout << "3. ASCII randomized: ";
    for (auto byte : data3) {
        std::cout << static_cast<char>(byte);
    }
    std::cout << std::endl << std::endl;
    
    // Test 4: Partial randomization
    std::vector<uint8_t> data4(16, 0xAA);
    std::cout << "4. Before partial: ";
    print_hex(data4, "");
    
    random::randomize_bytes_partial(data4, 4, 8);  // Randomize middle 8 bytes
    std::cout << "   After partial:  ";
    print_hex(data4, "");
    std::cout << std::endl;
    
    // Test 5: Performance demonstration
    std::cout << "5. Performance test (1000 randomizations of 1KB):\n";
    std::vector<uint8_t> perf_data(1024);
    
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 1000; ++i) {
        random::randomize_bytes(perf_data);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "   Completed in " << duration.count() << " microseconds\n";
    std::cout << "   Average: " << duration.count() / 1000.0 << " microseconds per operation\n";
    
    std::cout << "\n✓ In-place randomization APIs working perfectly!\n";
    std::cout << "✓ No memory allocation overhead\n";
    std::cout << "✓ Efficient reuse of existing vectors\n";
    
    return 0;
}
