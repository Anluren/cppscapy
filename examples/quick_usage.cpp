#include "../include/network_headers.h"
#include <iostream>

using namespace cppscapy;

int main() {
    std::cout << "=== Quick Usage Examples ===\n";
    
    // Example 1: Create a simple TCP SYN packet
    std::cout << "\n1. TCP SYN Packet:\n";
    auto syn = patterns::tcp_syn(
        IPv4Address("192.168.1.100"),  // Source IP
        IPv4Address("192.168.1.1"),    // Destination IP
        12345,                         // Source port
        80                            // Destination port (HTTP)
    );
    std::cout << "TCP SYN packet size: " << syn.size() << " bytes\n";
    
    // Example 2: Create a UDP DNS query packet
    std::cout << "\n2. UDP DNS Query:\n";
    std::vector<uint8_t> dns_query = {
        0x12, 0x34,        // Transaction ID
        0x01, 0x00,        // Flags (standard query)
        0x00, 0x01,        // Questions: 1
        0x00, 0x00,        // Answer RRs: 0
        0x00, 0x00,        // Authority RRs: 0
        0x00, 0x00         // Additional RRs: 0
    };
    
    auto dns_packet = patterns::udp_packet(
        IPv4Address("192.168.1.100"),
        IPv4Address("8.8.8.8"),
        53001,                         // Source port
        53,                           // DNS port
        dns_query
    );
    std::cout << "UDP DNS packet size: " << dns_packet.size() << " bytes\n";
    
    // Example 3: Create an ICMP ping packet
    std::cout << "\n3. ICMP Ping:\n";
    auto ping = patterns::icmp_ping(
        IPv4Address("192.168.1.100"),
        IPv4Address("8.8.8.8"),
        1,                            // Identifier
        1                             // Sequence number
    );
    std::cout << "ICMP ping packet size: " << ping.size() << " bytes\n";
    
    // Example 4: Create a complete Ethernet frame with IPv4 payload
    std::cout << "\n4. Complete Ethernet Frame:\n";
    MacAddress src_mac("aa:bb:cc:dd:ee:ff");
    MacAddress dst_mac("11:22:33:44:55:66");
    
    auto frame = patterns::ethernet_frame(
        src_mac,
        dst_mac,
        EthernetHeader::ETHERTYPE_IPV4,
        syn  // Use the TCP SYN packet as payload
    );
    std::cout << "Complete Ethernet frame size: " << frame.size() << " bytes\n";
    
    // Example 5: Manual packet building with PacketBuilder
    std::cout << "\n5. Manual Packet Building:\n";
    PacketBuilder builder;
    
    auto custom_packet = builder
        .ethernet(EthernetHeader(dst_mac, src_mac, EthernetHeader::ETHERTYPE_IPV4))
        .ipv4(IPv4Header(IPv4Address("10.0.0.1"), IPv4Address("10.0.0.2"), IPv4Header::PROTOCOL_TCP)
              .ttl(64)
              .length(IPv4Header::MIN_SIZE + TCPHeader::MIN_SIZE + 4))
        .tcp(TCPHeader(80, 8080)
             .flags(TCPHeader::FLAG_PSH | TCPHeader::FLAG_ACK)
             .seq_num(1000)
             .ack_num(2000))
        .payload("HTTP")
        .build();
    
    std::cout << "Custom packet size: " << custom_packet.size() << " bytes\n";
    
    // Example 6: IPv6 packet
    std::cout << "\n6. IPv6 Packet:\n";
    auto ipv6_packet = patterns::ipv6_packet(
        IPv6Address("2001:db8::1"),
        IPv6Address("2001:db8::2"),
        IPv6Header::NEXT_HEADER_TCP,
        {'H', 'e', 'l', 'l', 'o', ' ', 'I', 'P', 'v', '6'}
    );
    std::cout << "IPv6 packet size: " << ipv6_packet.size() << " bytes\n";
    
    std::cout << "\n=== All Examples Complete! ===\n";
    return 0;
}
