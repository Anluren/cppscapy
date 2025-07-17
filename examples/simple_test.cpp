#include "../include/network_headers.h"
#include <iostream>
#include <cassert>

using namespace cppscapy;

void test_mac_address() {
    std::cout << "Testing MAC Address... ";
    
    MacAddress mac1("aa:bb:cc:dd:ee:ff");
    assert(mac1.to_string() == "aa:bb:cc:dd:ee:ff");
    
    MacAddress mac2(0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF);
    assert(mac2.to_string() == "aa:bb:cc:dd:ee:ff");
    
    MacAddress broadcast = MacAddress::broadcast();
    assert(broadcast.is_broadcast());
    
    std::cout << "PASSED\n";
}

void test_ipv4_address() {
    std::cout << "Testing IPv4 Address... ";
    
    IPv4Address ip1("192.168.1.1");
    assert(ip1.to_string() == "192.168.1.1");
    
    IPv4Address ip2(192, 168, 1, 1);
    assert(ip2.to_string() == "192.168.1.1");
    
    IPv4Address localhost = IPv4Address::localhost();
    assert(localhost.to_string() == "127.0.0.1");
    
    std::cout << "PASSED\n";
}

void test_ipv6_address() {
    std::cout << "Testing IPv6 Address... ";
    
    IPv6Address localhost = IPv6Address::localhost();
    assert(localhost.to_string() == "::1");
    
    IPv6Address any = IPv6Address::any();
    assert(any.to_string() == "::");
    
    std::cout << "PASSED\n";
}

void test_ethernet_header() {
    std::cout << "Testing Ethernet Header... ";
    
    MacAddress src("aa:bb:cc:dd:ee:ff");
    MacAddress dst("11:22:33:44:55:66");
    
    EthernetHeader eth(dst, src, EthernetHeader::ETHERTYPE_IPV4);
    auto bytes = eth.to_bytes();
    
    assert(bytes.size() == EthernetHeader::SIZE);
    assert(bytes[12] == 0x08); // EtherType IPv4 high byte
    assert(bytes[13] == 0x00); // EtherType IPv4 low byte
    
    std::cout << "PASSED\n";
}

void test_ipv4_header() {
    std::cout << "Testing IPv4 Header... ";
    
    IPv4Address src("192.168.1.1");
    IPv4Address dst("10.0.0.1");
    
    IPv4Header ip(src, dst, IPv4Header::PROTOCOL_TCP);
    auto bytes = ip.to_bytes();
    
    assert(bytes.size() == IPv4Header::MIN_SIZE);
    assert((bytes[0] >> 4) == 4); // Version
    assert((bytes[0] & 0x0F) == 5); // IHL
    assert(bytes[9] == IPv4Header::PROTOCOL_TCP); // Protocol
    
    std::cout << "PASSED\n";
}

void test_tcp_header() {
    std::cout << "Testing TCP Header... ";
    
    TCPHeader tcp(80, 8080);
    tcp.flags(TCPHeader::FLAG_SYN | TCPHeader::FLAG_ACK);
    
    auto bytes = tcp.to_bytes();
    
    assert(bytes.size() == TCPHeader::MIN_SIZE);
    assert(bytes[0] == 0 && bytes[1] == 80); // Source port
    assert(bytes[2] == 0x1F && bytes[3] == 0x90); // Dest port (8080)
    assert(bytes[13] == (TCPHeader::FLAG_SYN | TCPHeader::FLAG_ACK)); // Flags
    
    std::cout << "PASSED\n";
}

void test_packet_builder() {
    std::cout << "Testing PacketBuilder... ";
    
    MacAddress src_mac("aa:bb:cc:dd:ee:ff");
    MacAddress dst_mac("11:22:33:44:55:66");
    IPv4Address src_ip("192.168.1.1");
    IPv4Address dst_ip("10.0.0.1");
    
    PacketBuilder builder;
    auto packet = builder
        .ethernet(EthernetHeader(dst_mac, src_mac, EthernetHeader::ETHERTYPE_IPV4))
        .ipv4(IPv4Header(src_ip, dst_ip, IPv4Header::PROTOCOL_TCP))
        .tcp(TCPHeader(80, 8080))
        .payload("Hello")
        .build();
    
    assert(packet.size() == EthernetHeader::SIZE + IPv4Header::MIN_SIZE + TCPHeader::MIN_SIZE + 5);
    
    std::cout << "PASSED\n";
}

void test_patterns() {
    std::cout << "Testing Patterns... ";
    
    IPv4Address src_ip("192.168.1.1");
    IPv4Address dst_ip("10.0.0.1");
    
    // Test TCP SYN
    auto syn_packet = patterns::tcp_syn(src_ip, dst_ip, 12345, 80);
    assert(syn_packet.size() == IPv4Header::MIN_SIZE + TCPHeader::MIN_SIZE);
    
    // Test UDP packet
    std::vector<uint8_t> payload = {'t', 'e', 's', 't'};
    auto udp_packet = patterns::udp_packet(src_ip, dst_ip, 12345, 53, payload);
    assert(udp_packet.size() == IPv4Header::MIN_SIZE + UDPHeader::SIZE + 4);
    
    // Test ICMP ping
    auto ping_packet = patterns::icmp_ping(src_ip, dst_ip, 1, 1);
    assert(ping_packet.size() == IPv4Header::MIN_SIZE + ICMPHeader::MIN_SIZE);
    
    std::cout << "PASSED\n";
}

int main() {
    std::cout << "=== Running CppScapy Tests ===\n";
    
    test_mac_address();
    test_ipv4_address();
    test_ipv6_address();
    test_ethernet_header();
    test_ipv4_header();
    test_tcp_header();
    test_packet_builder();
    test_patterns();
    
    std::cout << "\n=== All Tests PASSED! ===\n";
    
    return 0;
}
