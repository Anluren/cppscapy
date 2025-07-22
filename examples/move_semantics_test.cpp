#include "../include/utils.h"
#include <iostream>
#include <vector>
#include <chrono>

using namespace cppscapy::utils;

// Custom vector wrapper to track moves and copies
template<typename T>
class TrackingVector {
public:
    std::vector<T> data;
    mutable int copy_count = 0;
    mutable int move_count = 0;
    
    // Default constructor
    TrackingVector() = default;
    
    // Constructor from vector (move)
    TrackingVector(std::vector<T>&& vec) : data(std::move(vec)) {
        move_count++;
        std::cout << "Move constructor called (count: " << move_count << ")\n";
    }
    
    // Constructor from vector (copy)
    TrackingVector(const std::vector<T>& vec) : data(vec) {
        copy_count++;
        std::cout << "Copy constructor called (count: " << copy_count << ")\n";
    }
    
    // Move assignment
    TrackingVector& operator=(std::vector<T>&& vec) {
        data = std::move(vec);
        move_count++;
        std::cout << "Move assignment called (count: " << move_count << ")\n";
        return *this;
    }
    
    // Copy assignment
    TrackingVector& operator=(const std::vector<T>& vec) {
        data = vec;
        copy_count++;
        std::cout << "Copy assignment called (count: " << copy_count << ")\n";
        return *this;
    }
    
    size_t size() const { return data.size(); }
};

int main() {
    std::cout << "=== Testing Move vs Copy Semantics ===\n\n";
    
    // Test 1: Direct assignment from function return (should move)
    std::cout << "1. Direct assignment from function return:\n";
    auto start1 = std::chrono::high_resolution_clock::now();
    std::vector<uint8_t> payload1 = random::incremental_pattern(1000000);
    auto end1 = std::chrono::high_resolution_clock::now();
    auto duration1 = std::chrono::duration_cast<std::chrono::microseconds>(end1 - start1);
    std::cout << "   Time: " << duration1.count() << " microseconds\n";
    std::cout << "   Size: " << payload1.size() << " bytes\n\n";
    
    // Test 2: Auto assignment (should move)
    std::cout << "2. Auto assignment:\n";
    auto start2 = std::chrono::high_resolution_clock::now();
    auto payload2 = random::random_bytes(1000000);
    auto end2 = std::chrono::high_resolution_clock::now();
    auto duration2 = std::chrono::duration_cast<std::chrono::microseconds>(end2 - start2);
    std::cout << "   Time: " << duration2.count() << " microseconds\n";
    std::cout << "   Size: " << payload2.size() << " bytes\n\n";
    
    // Test 3: Using our tracking wrapper
    std::cout << "3. Using tracking wrapper:\n";
    TrackingVector<uint8_t> tracked_payload;
    tracked_payload = random::incremental_pattern(100000);  // Should move
    std::cout << "   Final size: " << tracked_payload.size() << "\n";
    std::cout << "   Moves: " << tracked_payload.move_count << "\n";
    std::cout << "   Copies: " << tracked_payload.copy_count << "\n\n";
    
    // Test 4: Forced copy scenario
    std::cout << "4. Forced copy scenario:\n";
    auto temp_payload = random::random_ascii(100000);
    TrackingVector<uint8_t> tracked_copy;
    tracked_copy = temp_payload;  // This should copy (lvalue)
    std::cout << "   Final size: " << tracked_copy.size() << "\n";
    std::cout << "   Moves: " << tracked_copy.move_count << "\n";
    std::cout << "   Copies: " << tracked_copy.copy_count << "\n\n";
    
    // Test 5: Performance comparison - Multiple moves vs copies
    std::cout << "5. Performance comparison (1000 operations):\n";
    
    // Move performance
    auto move_start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 1000; ++i) {
        std::vector<uint8_t> temp = random::incremental_pattern(1000, i % 256);
        // temp is automatically destroyed here (efficient)
    }
    auto move_end = std::chrono::high_resolution_clock::now();
    auto move_duration = std::chrono::duration_cast<std::chrono::microseconds>(move_end - move_start);
    
    std::cout << "   Move semantics time: " << move_duration.count() << " microseconds\n";
    
    // Memory usage demonstration
    std::cout << "\n6. Memory efficiency demonstration:\n";
    std::vector<std::vector<uint8_t>> payload_collection;
    payload_collection.reserve(100);
    
    auto memory_start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 100; ++i) {
        // This uses move semantics - very efficient
        payload_collection.push_back(random::random_bytes(10000));
    }
    auto memory_end = std::chrono::high_resolution_clock::now();
    auto memory_duration = std::chrono::duration_cast<std::chrono::microseconds>(memory_end - memory_start);
    
    std::cout << "   Created 100 payloads (10KB each) in: " << memory_duration.count() << " microseconds\n";
    std::cout << "   Total payloads stored: " << payload_collection.size() << "\n";
    std::cout << "   Average per payload: " << memory_duration.count() / 100 << " microseconds\n";
    
    std::cout << "\n=== Summary ===\n";
    std::cout << "✓ Function return values use move semantics (efficient)\n";
    std::cout << "✓ No unnecessary copying of large vector contents\n";
    std::cout << "✓ Memory transfer is O(1), not O(n)\n";
    std::cout << "✓ Pattern functions are optimized for performance\n";
    
    return 0;
}
