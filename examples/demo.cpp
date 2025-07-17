#include "../include/network_headers.h"
#include <iostream>
#include <iomanip>

using namespace cppscapy;

// Helper function to print packet bytes
void print_packet(const std::vector<uint8_t>& packet, const std::string& description) {
    std::cout << "\n" << description << ":\n";
    std::cout << "Length: " << packet.size() << " bytes\n";
    std::cout << "Hex dump: ";
    
    for (size_t i = 0; i < packet.size(); ++i) {
        if (i % 16 == 0) std::cout << "\n";
        std::cout << std::hex << std::setfill('0') << std::setw(2) 
                  << static_cast<int>(packet[i]) << " ";
    }
    std::cout << std::dec << "\n";
}

int main() {
    std::cout << "=== CppScapy Network Headers Demo ===\n";
    
    // 1. MAC Address examples
    std::cout << "\n1. MAC Address Examples:\n";
    MacAddress mac1("aa:bb:cc:dd:ee:ff");
    MacAddress mac2(0x11, 0x22, 0x33, 0x44, 0x55, 0x66);
    MacAddress broadcast = MacAddress::broadcast();
    
    std::cout << "MAC 1: " << mac1.to_string() << "\n";
    std::cout << "MAC 2: " << mac2.to_string() << "\n";
    std::cout << "Broadcast: " << broadcast.to_string() << "\n";
    std::cout << "Is broadcast: " << (broadcast.is_broadcast() ? "Yes" : "No") << "\n";
    
    // 2. IPv4 Address examples
    std::cout << "\n2. IPv4 Address Examples:\n";
    IPv4Address ip1("192.168.1.1");
    IPv4Address ip2(10, 0, 0, 1);
    IPv4Address localhost = IPv4Address::localhost();
    
    std::cout << "IP 1: " << ip1.to_string() << "\n";
    std::cout << "IP 2: " << ip2.to_string() << "\n";
    std::cout << "Localhost: " << localhost.to_string() << "\n";
    
    // 3. IPv6 Address examples
    std::cout << "\n3. IPv6 Address Examples:\n";
    IPv6Address ipv6_1("2001:db8::1");
    IPv6Address ipv6_localhost = IPv6Address::localhost();
    
    std::cout << "IPv6 1: " << ipv6_1.to_string() << "\n";
    std::cout << "IPv6 Localhost: " << ipv6_localhost.to_string() << "\n";
    
    // 4. Ethernet Frame example
    std::cout << "\n4. Ethernet Frame Example:\n";
    EthernetHeader eth_header(mac1, mac2, EthernetHeader::ETHERTYPE_IPV4);
    auto eth_bytes = eth_header.to_bytes();
    print_packet(eth_bytes, "Ethernet Header");
    
    // 5. IPv4 Header example
    std::cout << "\n5. IPv4 Header Example:\n";
    IPv4Header ipv4_header(ip2, ip1, IPv4Header::PROTOCOL_TCP);
    ipv4_header.ttl(64).id(12345).length(60);
    auto ipv4_bytes = ipv4_header.to_bytes();
    print_packet(ipv4_bytes, "IPv4 Header");
    
    // 6. IPv6 Header example
    std::cout << "\n6. IPv6 Header Example:\n";
    IPv6Header ipv6_header(ipv6_localhost, ipv6_1, IPv6Header::NEXT_HEADER_TCP);
    ipv6_header.hop_limit(64).payload_length(20);
    auto ipv6_bytes = ipv6_header.to_bytes();
    print_packet(ipv6_bytes, "IPv6 Header");
    
    // 7. TCP Header example
    std::cout << "\n7. TCP Header Example:\n";
    TCPHeader tcp_header(80, 8080);
    tcp_header.seq_num(1000).ack_num(2000).flags(TCPHeader::FLAG_SYN | TCPHeader::FLAG_ACK);
    auto tcp_bytes = tcp_header.to_bytes();
    print_packet(tcp_bytes, "TCP Header");
    
    // 8. UDP Header example
    std::cout << "\n8. UDP Header Example:\n";
    UDPHeader udp_header(53, 12345, 28); // DNS query
    auto udp_bytes = udp_header.to_bytes();
    print_packet(udp_bytes, "UDP Header");
    
    // 9. ICMP Header example
    std::cout << "\n9. ICMP Header Example:\n";
    ICMPHeader icmp_header(ICMPHeader::TYPE_ECHO_REQUEST, 0);
    icmp_header.identifier(1).sequence(1);
    auto icmp_bytes = icmp_header.to_bytes();
    print_packet(icmp_bytes, "ICMP Header");
    
    // 10. PacketBuilder example - Full IPv4 TCP packet
    std::cout << "\n10. PacketBuilder Example - Full IPv4 TCP Packet:\n";
    PacketBuilder builder;
    std::string payload = "Hello, World!";
    
    auto full_packet = builder
        .ethernet(EthernetHeader(mac1, mac2, EthernetHeader::ETHERTYPE_IPV4))
        .ipv4(IPv4Header(ip2, ip1, IPv4Header::PROTOCOL_TCP)
              .length(IPv4Header::MIN_SIZE + TCPHeader::MIN_SIZE + payload.size()))
        .tcp(TCPHeader(80, 8080).flags(TCPHeader::FLAG_PSH | TCPHeader::FLAG_ACK))
        .payload(payload)
        .build();
    
    print_packet(full_packet, "Full Ethernet/IPv4/TCP Packet");
    
    // 11. Using convenience patterns
    std::cout << "\n11. Convenience Patterns:\n";
    
    // TCP SYN packet
    auto syn_packet = patterns::tcp_syn(ip2, ip1, 12345, 80, 1000);
    print_packet(syn_packet, "TCP SYN Packet");
    
    // UDP packet
    std::vector<uint8_t> udp_payload = {'D', 'N', 'S', ' ', 'Q', 'u', 'e', 'r', 'y'};
    auto udp_packet = patterns::udp_packet(ip2, ip1, 12345, 53, udp_payload);
    print_packet(udp_packet, "UDP Packet");
    
    // ICMP ping packet
    auto ping_packet = patterns::icmp_ping(ip2, ip1, 1, 1);
    print_packet(ping_packet, "ICMP Ping Packet");
    
    // Ethernet frame with IPv4 payload
    auto ipv4_frame = patterns::ethernet_frame(mac2, mac1, EthernetHeader::ETHERTYPE_IPV4, ipv4_bytes);
    print_packet(ipv4_frame, "Ethernet Frame with IPv4 Payload");
    
    // IPv6 packet
    std::vector<uint8_t> ipv6_payload = {'I', 'P', 'v', '6', ' ', 'D', 'a', 't', 'a'};
    auto ipv6_packet = patterns::ipv6_packet(ipv6_localhost, ipv6_1, IPv6Header::NEXT_HEADER_UDP, ipv6_payload);
    print_packet(ipv6_packet, "IPv6 Packet");
    
    std::cout << "\n=== Demo Complete ===\n";
    
    return 0;
}
