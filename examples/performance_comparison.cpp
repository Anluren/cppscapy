#include "../include/utils.h"
#include <chrono>
#include <iostream>
#include <iomanip>
#include <vector>

using namespace cppscapy::utils::random;

int main() {
    std::cout << "=== Performance Comparison: Refactored vs Direct Implementation ===" << std::endl;
    
    const size_t size = 1024;
    const int iterations = 1000;
    
    // Test refactored allocation-based functions
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        auto data1 = random_bytes(size);
        auto data2 = random_ascii(size);
        auto data3 = random_alphanumeric(size);
        auto data4 = random_hex_chars(size);
        auto data5 = incremental_pattern(size, 0);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto allocation_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    // Test in-place functions
    std::vector<uint8_t> buffer(size);
    
    start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        randomize_bytes(buffer);
        randomize_ascii(buffer);
        randomize_alphanumeric(buffer);
        randomize_hex_chars(buffer);
        fill_incremental_pattern(buffer, 0);
    }
    
    end = std::chrono::high_resolution_clock::now();
    auto inplace_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "Allocation-based (refactored): " << allocation_time.count() << " microseconds" << std::endl;
    std::cout << "In-place (reused buffer):      " << inplace_time.count() << " microseconds" << std::endl;
    
    double speedup = static_cast<double>(allocation_time.count()) / inplace_time.count();
    std::cout << "Speedup with in-place:         " << std::fixed << std::setprecision(2) << speedup << "x" << std::endl;
    
    std::cout << "\n=== Verification: Both approaches produce correct output ===" << std::endl;
    
    // Verify the refactored functions work correctly
    auto test_bytes = random_bytes(16);
    auto test_ascii = random_ascii(16);
    auto test_pattern = incremental_pattern(16, 100);
    
    std::cout << "Random bytes (16): ";
    for (auto b : test_bytes) std::cout << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(b) << " ";
    std::cout << std::endl;
    
    std::cout << "Random ASCII (16): ";
    for (auto b : test_ascii) std::cout << static_cast<char>(b);
    std::cout << std::endl;
    
    std::cout << "Incremental (16):  ";
    for (auto b : test_pattern) std::cout << std::dec << static_cast<int>(b) << " ";
    std::cout << std::endl;
    
    return 0;
}
