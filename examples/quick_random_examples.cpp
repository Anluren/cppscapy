#include "network_headers.h"
#include "utils.h"
#include <iostream>

using namespace cppscapy;
using namespace cppscapy::utils;

int main() {
    std::cout << "=== Quick Random Payload Examples ===\n\n";
    
    // Example 1: Simple usage
    std::cout << "1. Generate random TCP packet:\n";
    auto tcp_packet = PacketBuilder()
        .ipv4(IPv4Header(IPv4Address("192.168.1.1"), IPv4Address("10.0.0.1"), IPv4Header::PROTOCOL_TCP))
        .tcp(TCPHeader(80, 8080).flags(TCPHeader::FLAG_SYN))
        .payload(random::random_bytes(64))  // 64 bytes of random data
        .build();
    
    std::cout << "Packet size: " << tcp_packet.size() << " bytes\n";
    
    // Example 2: Different payload types
    std::cout << "\n2. Different payload types for testing:\n";
    
    // Printable ASCII for text-based protocols
    auto http_payload = random::random_ascii(128);
    std::cout << "ASCII payload: " << http_payload.size() << " bytes\n";
    
    // Binary data for binary protocols
    auto binary_payload = random::random_bytes(256);
    std::cout << "Binary payload: " << binary_payload.size() << " bytes\n";
    
    // Reproducible payload for testing
    auto test_payload1 = random::random_bytes_seeded(100, 42);
    auto test_payload2 = random::random_bytes_seeded(100, 42);
    std::cout << "Reproducible payloads identical: " << (test_payload1 == test_payload2 ? "YES" : "NO") << "\n";
    
    // Example 3: Using with network patterns
    std::cout << "\n3. Using with pattern functions:\n";
    
    auto udp_packet = patterns::udp_packet(
        IPv4Address("192.168.1.100"),
        IPv4Address("8.8.8.8"),
        12345, 53,
        random::random_network_data(128)  // DNS-like random data
    );
    
    std::cout << "UDP packet with random payload: " << udp_packet.size() << " bytes\n";
    
    // Example 4: Templated lambda with random payloads
    std::cout << "\n4. Smart payload selection:\n";
    
    auto smart_packet = [](auto protocol_type, size_t size) {
        if constexpr (std::is_same_v<decltype(protocol_type), std::string>) {
            if (protocol_type == "http") {
                return random::random_http_like(size);
            } else if (protocol_type == "binary") {
                return random::random_binary_protocol(size);
            }
        }
        return random::random_bytes(size);
    };
    
    auto http_like = smart_packet(std::string("http"), 64);
    auto binary_like = smart_packet(std::string("binary"), 64);
    auto default_random = smart_packet(42, 64);  // Non-string, uses random_bytes
    
    std::cout << "HTTP-like payload generated\n";
    std::cout << "Binary protocol-like payload generated\n";
    std::cout << "Default random payload generated\n";
    
    // 9. Incremental Pattern Examples
    std::cout << "\n=== Incremental Pattern Examples ===\n";
    
    // Basic incremental pattern
    auto inc1 = random::incremental_pattern(16);  // 0, 1, 2, ..., 15
    print_hex(inc1, "Basic incremental (16 bytes)");
    
    // Incremental pattern with custom start
    auto inc2 = random::incremental_pattern(16, 240);  // 240, 241, ..., 255, 0, 1, ...
    print_hex(inc2, "Incremental starting from 240");
    
    std::cout << "\n=== Quick Examples Complete ===\n";
    
    return 0;
}
