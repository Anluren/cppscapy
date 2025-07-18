#include "network_headers.h"
#include "utils.h"
#include <iostream>
#include <iomanip>
#include <cassert>

using namespace cppscapy;
using namespace cppscapy::utils;

int main() {
    std::cout << "=== Testing MPLS Header Implementation ===\n\n";
    
    // Test 1: Basic MPLS header creation
    std::cout << "Test 1: Basic MPLS header creation\n";
    
    MPLSHeader mpls1(100, 5, true, 64); // label=100, tc=5, bottom_of_stack=true, ttl=64
    
    std::cout << "MPLS Header:\n";
    std::cout << "  Label: " << mpls1.label() << "\n";
    std::cout << "  Traffic Class: " << static_cast<int>(mpls1.traffic_class()) << "\n";
    std::cout << "  Bottom of Stack: " << (mpls1.bottom_of_stack() ? "Yes" : "No") << "\n";
    std::cout << "  TTL: " << static_cast<int>(mpls1.ttl()) << "\n";
    
    auto mpls_bytes = mpls1.to_bytes();
    std::cout << "  Raw bytes: ";
    for (size_t i = 0; i < mpls_bytes.size(); ++i) {
        std::cout << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(mpls_bytes[i]) << " ";
    }
    std::cout << std::dec << "\n\n";
    
    // Test 2: MPLS header with fluent API
    std::cout << "Test 2: MPLS header with fluent API\n";
    
    MPLSHeader mpls2;
    mpls2.label(0xABCDE)     // 20-bit label (should be masked to 0xBCDE)
         .traffic_class(7)   // 3-bit TC
         .bottom_of_stack(false)
         .ttl(32);
    
    std::cout << "Fluent API MPLS Header:\n";
    std::cout << "  Label: 0x" << std::hex << mpls2.label() << std::dec << " (" << mpls2.label() << ")\n";
    std::cout << "  Traffic Class: " << static_cast<int>(mpls2.traffic_class()) << "\n";
    std::cout << "  Bottom of Stack: " << (mpls2.bottom_of_stack() ? "Yes" : "No") << "\n";
    std::cout << "  TTL: " << static_cast<int>(mpls2.ttl()) << "\n";
    
    auto mpls2_bytes = mpls2.to_bytes();
    std::cout << "  Raw bytes: ";
    for (size_t i = 0; i < mpls2_bytes.size(); ++i) {
        std::cout << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(mpls2_bytes[i]) << " ";
    }
    std::cout << std::dec << "\n\n";
    
    // Test 3: Special MPLS labels
    std::cout << "Test 3: Special MPLS labels\n";
    
    MPLSHeader ipv4_null(MPLSHeader::LABEL_IPV4_EXPLICIT_NULL);
    MPLSHeader router_alert(MPLSHeader::LABEL_ROUTER_ALERT);
    MPLSHeader ipv6_null(MPLSHeader::LABEL_IPV6_EXPLICIT_NULL);
    
    std::cout << "IPv4 Explicit Null label: " << ipv4_null.label() << "\n";
    std::cout << "Router Alert label: " << router_alert.label() << "\n";
    std::cout << "IPv6 Explicit Null label: " << ipv6_null.label() << "\n\n";
    
    // Test 4: MPLS over Ethernet
    std::cout << "Test 4: MPLS over Ethernet\n";
    
    MacAddress src_mac("aa:bb:cc:dd:ee:ff");
    MacAddress dst_mac("11:22:33:44:55:66");
    
    EthernetHeader eth(dst_mac, src_mac, EthernetHeader::ETHERTYPE_MPLS);
    MPLSHeader mpls_label(1000, 3, true, 255);
    
    std::cout << "Ethernet EtherType for MPLS: 0x" << std::hex << EthernetHeader::ETHERTYPE_MPLS << std::dec << "\n";
    std::cout << "Ethernet EtherType for MPLS Multicast: 0x" << std::hex << EthernetHeader::ETHERTYPE_MPLS_MCAST << std::dec << "\n";
    
    // Build complete packet
    PacketBuilder builder;
    auto packet = builder
        .ethernet(eth)
        .mpls(mpls_label)
        .payload("Hello MPLS!")
        .build();
    
    std::cout << "Complete MPLS packet size: " << packet.size() << " bytes\n";
    std::cout << "Packet bytes: ";
    for (size_t i = 0; i < std::min(packet.size(), size_t(32)); ++i) {
        std::cout << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(packet[i]) << " ";
    }
    if (packet.size() > 32) std::cout << "...";
    std::cout << std::dec << "\n\n";
    
    // Test 5: MPLS patterns
    std::cout << "Test 5: MPLS utility patterns\n";
    
    // Test MPLS packet pattern
    auto mpls_packet = patterns::mpls_packet(500, 128, 2, {0xDE, 0xAD, 0xBE, 0xEF});
    std::cout << "MPLS packet size: " << mpls_packet.size() << " bytes\n";
    
    // Test MPLS over Ethernet pattern
    auto mpls_eth_packet = patterns::mpls_ethernet_frame(
        src_mac, dst_mac,
        750, 200, 4,
        {0xCA, 0xFE, 0xBA, 0xBE});
    
    std::cout << "MPLS over Ethernet packet size: " << mpls_eth_packet.size() << " bytes\n";
    
    print_hex_ascii(mpls_eth_packet, "MPLS over Ethernet packet");
    
    // Test 6: MPLS stack (multiple labels)
    std::cout << "\nTest 6: MPLS label stack\n";
    
    MPLSHeader outer_label(2000, 1, false, 64);  // Not bottom of stack
    MPLSHeader inner_label(3000, 2, true, 64);   // Bottom of stack
    
    PacketBuilder stack_builder;
    auto stacked_packet = stack_builder
        .ethernet(EthernetHeader(dst_mac, src_mac, EthernetHeader::ETHERTYPE_MPLS))
        .mpls(outer_label)
        .mpls(inner_label)
        .payload("Stacked MPLS")
        .build();
    
    std::cout << "MPLS stacked packet size: " << stacked_packet.size() << " bytes\n";
    print_hex_ascii(stacked_packet, "MPLS stacked packet");
    
    // Test 7: Field validation
    std::cout << "\nTest 7: Field validation\n";
    
    MPLSHeader test_limits;
    test_limits.label(0xFFFFF);      // Max 20-bit value
    test_limits.traffic_class(0x7);  // Max 3-bit value
    
    std::cout << "Max label value (0xFFFFF): 0x" << std::hex << test_limits.label() << std::dec << "\n";
    std::cout << "Max TC value (0x7): " << static_cast<int>(test_limits.traffic_class()) << "\n";
    
    // Test with values that exceed field sizes
    test_limits.label(0x1FFFFF);     // Should be masked to 20 bits
    test_limits.traffic_class(0xF);  // Should be masked to 3 bits
    
    std::cout << "Masked label (0x1FFFFF -> 0xFFFFF): 0x" << std::hex << test_limits.label() << std::dec << "\n";
    std::cout << "Masked TC (0xF -> 0x7): " << static_cast<int>(test_limits.traffic_class()) << "\n";
    
    std::cout << "\n=== MPLS Tests Complete ===\n";
    
    return 0;
}
