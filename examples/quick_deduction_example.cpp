#include "utils.h"
#include <iostream>

using namespace cppscapy;
using namespace cppscapy::utils;

int main() {
    std::cout << "=== Compile-Time Length Deduction Example ===\n\n";
    
    // The magic happens here - no need to specify array sizes!
    constexpr auto mac = from_hex_string_auto("001122334455");
    constexpr auto ipv4 = from_hex_string_auto("C0A80101");
    constexpr auto key = from_hex_string_auto("DEADBEEF");
    
    // All sizes are known at compile time
    std::cout << "MAC bytes: " << mac.size() << " bytes\n";
    std::cout << "IPv4 bytes: " << ipv4.size() << " bytes\n";
    std::cout << "Key bytes: " << key.size() << " bytes\n";
    
    // Create network objects
    MacAddress mac_addr(mac);
    IPv4Address ipv4_addr(ipv4[0], ipv4[1], ipv4[2], ipv4[3]);
    
    std::cout << "\nNetwork objects:\n";
    std::cout << "MAC: " << mac_addr.to_string() << "\n";
    std::cout << "IPv4: " << ipv4_addr.to_string() << "\n";
    
    // Convenience functions
    auto broadcast_mac = make_mac_address("FFFFFFFFFFFF");
    auto google_dns = make_ipv4_address("08080808");
    
    std::cout << "\nConvenience functions:\n";
    std::cout << "Broadcast MAC: " << broadcast_mac.to_string() << "\n";
    std::cout << "Google DNS: " << google_dns.to_string() << "\n";
    
    return 0;
}
