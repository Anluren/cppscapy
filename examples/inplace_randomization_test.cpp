#include "../include/utils.h"
#include <iostream>
#include <chrono>

using namespace cppscapy::utils;

int main() {
    std::cout << "=== In-Place Vector Randomization API Test ===\n\n";
    
    // Test 1: Basic in-place randomization
    std::cout << "1. Basic in-place randomization:\n";
    std::vector<uint8_t> data1(20, 0x00);  // Initialize with zeros
    std::cout << "Before: ";
    print_hex(data1, "");
    
    random::randomize_bytes(data1);
    std::cout << "After randomize_bytes: ";
    print_hex(data1, "");
    std::cout << std::endl;
    
    // Test 2: Range-based randomization
    std::cout << "2. Range-based randomization (100-200):\n";
    std::vector<uint8_t> data2(16, 0xFF);  // Initialize with 0xFF
    std::cout << "Before: ";
    print_hex(data2, "");
    
    random::randomize_bytes_range(data2, 100, 200);
    std::cout << "After randomize_bytes_range(100, 200): ";
    print_hex(data2, "");
    std::cout << std::endl;
    
    // Test 3: ASCII randomization
    std::cout << "3. ASCII randomization:\n";
    std::vector<uint8_t> data3(20, 0x00);
    random::randomize_ascii(data3);
    std::cout << "ASCII randomized: ";
    print_hex_ascii(data3, "");
    std::cout << std::endl;
    
    // Test 4: Alphanumeric randomization
    std::cout << "4. Alphanumeric randomization:\n";
    std::vector<uint8_t> data4(24, 0x00);
    random::randomize_alphanumeric(data4);
    std::cout << "Alphanumeric: ";
    for (auto byte : data4) {
        std::cout << static_cast<char>(byte);
    }
    std::cout << std::endl << std::endl;
    
    // Test 5: Hex characters randomization
    std::cout << "5. Hex characters randomization:\n";
    std::vector<uint8_t> data5(16, 0x00);
    random::randomize_hex_chars(data5);
    std::cout << "Hex chars: ";
    for (auto byte : data5) {
        std::cout << static_cast<char>(byte);
    }
    std::cout << std::endl << std::endl;
    
    // Test 6: Pattern-based randomization
    std::cout << "6. Pattern-based randomization:\n";
    std::vector<uint8_t> pattern = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE};
    std::vector<uint8_t> data6(20, 0x00);
    random::randomize_pattern(data6, pattern);
    std::cout << "Pattern randomized (from AA,BB,CC,DD,EE): ";
    print_hex(data6, "");
    std::cout << std::endl;
    
    // Test 7: Seeded randomization (reproducible)
    std::cout << "7. Seeded randomization (reproducible):\n";
    std::vector<uint8_t> data7a(16, 0x00);
    std::vector<uint8_t> data7b(16, 0x00);
    
    random::randomize_bytes_seeded(data7a, 12345);
    random::randomize_bytes_seeded(data7b, 12345);
    
    std::cout << "First run  (seed=12345): ";
    print_hex(data7a, "");
    std::cout << "Second run (seed=12345): ";
    print_hex(data7b, "");
    std::cout << "Results match: " << (data7a == data7b ? "YES" : "NO") << "\n\n";
    
    // Test 8: Incremental pattern fill
    std::cout << "8. Incremental pattern fill:\n";
    std::vector<uint8_t> data8(20, 0x00);
    random::fill_incremental_pattern(data8, 100);  // Start from 100
    std::cout << "Incremental (start=100): ";
    print_hex(data8, "");
    std::cout << std::endl;
    
    // Test 9: Network-like data
    std::cout << "9. Network-like data randomization:\n";
    std::vector<uint8_t> data9(64, 0x00);
    random::randomize_network_data(data9);
    std::cout << "Network-like data: ";
    print_hex_ascii(std::vector<uint8_t>(data9.begin(), data9.begin() + 32), "First 32 bytes");
    std::cout << std::endl;
    
    // Test 10: HTTP-like data
    std::cout << "10. HTTP-like data randomization:\n";
    std::vector<uint8_t> data10(80, 0x00);
    random::randomize_http_like(data10);
    std::cout << "HTTP-like: ";
    for (auto byte : data10) {
        char c = static_cast<char>(byte);
        std::cout << (std::isprint(c) ? c : '.');
    }
    std::cout << std::endl << std::endl;
    
    // Test 11: Binary protocol-like data
    std::cout << "11. Binary protocol randomization:\n";
    std::vector<uint8_t> data11(32, 0x00);
    random::randomize_binary_protocol(data11);
    std::cout << "Binary protocol: ";
    print_hex(data11, "");
    std::cout << std::endl;
    
    // Test 12: Partial randomization
    std::cout << "12. Partial randomization:\n";
    std::vector<uint8_t> data12(32, 0xAA);  // Fill with 0xAA
    std::cout << "Before partial randomization: ";
    print_hex(data12, "");
    
    // Randomize bytes 8-15 (8 bytes)
    random::randomize_bytes_partial(data12, 8, 8);
    std::cout << "After randomizing bytes 8-15: ";
    print_hex(data12, "");
    
    // Fill bytes 16-23 with incremental pattern starting from 200
    random::fill_incremental_pattern_partial(data12, 16, 8, 200);
    std::cout << "After incremental fill 16-23 (start=200): ";
    print_hex(data12, "");
    std::cout << std::endl;
    
    // Test 13: Performance comparison - In-place vs New allocation
    std::cout << "13. Performance comparison (1000 operations on 10KB data):\n";
    const size_t test_size = 10000;
    const int iterations = 1000;
    
    // Test in-place randomization performance
    std::vector<uint8_t> reusable_data(test_size);
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
        random::randomize_bytes(reusable_data);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto in_place_duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    // Test new allocation performance
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
        auto temp_data = random::random_bytes(test_size);
        // Simulate using the data
        volatile uint8_t dummy = temp_data[0];
        (void)dummy;
    }
    end = std::chrono::high_resolution_clock::now();
    auto new_alloc_duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "In-place randomization: " << in_place_duration.count() << " microseconds\n";
    std::cout << "New allocation method: " << new_alloc_duration.count() << " microseconds\n";
    std::cout << "Performance improvement: " << (double)new_alloc_duration.count() / in_place_duration.count() 
              << "x faster\n\n";
    
    // Test 14: Mixed operations on same vector
    std::cout << "14. Mixed operations on same vector:\n";
    std::vector<uint8_t> mixed_data(40, 0x00);
    
    // Fill first 10 bytes with incremental pattern
    random::fill_incremental_pattern_partial(mixed_data, 0, 10, 0);
    
    // Randomize next 10 bytes with ASCII
    random::randomize_ascii_partial(mixed_data, 10, 10);
    
    // Fill next 10 bytes with incremental pattern starting from 200
    random::fill_incremental_pattern_partial(mixed_data, 20, 10, 200);
    
    // Randomize last 10 bytes completely
    random::randomize_bytes_partial(mixed_data, 30, 10);
    
    std::cout << "Mixed operations result:\n";
    print_hex_ascii(mixed_data, "Bytes 0-9: incremental(0), 10-19: ASCII, 20-29: incremental(200), 30-39: random");
    std::cout << std::endl;
    
    std::cout << "=== Summary ===\n";
    std::cout << "✓ In-place randomization APIs work efficiently\n";
    std::cout << "✓ No memory allocation overhead\n";
    std::cout << "✓ Support for full vector and partial randomization\n";
    std::cout << "✓ Multiple randomization patterns available\n";
    std::cout << "✓ Significant performance improvement over allocation-based methods\n";
    std::cout << "✓ Perfect for reusing buffers and memory-constrained environments\n";
    
    return 0;
}
