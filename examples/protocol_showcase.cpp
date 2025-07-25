#include "../include/generated_headers.h"
#include <iostream>
#include <iomanip>
#include <vector>

using namespace cppscapy::dsl;

void printBytes(const std::vector<uint8_t>& data, const std::string& name) {
    std::cout << name << " (" << data.size() << " bytes): ";
    for (size_t i = 0; i < data.size(); ++i) {
        std::cout << std::hex << std::setfill('0') << std::setw(2) << (int)data[i];
        if (i < data.size() - 1) std::cout << " ";
    }
    std::cout << std::dec << std::endl;
}

int main() {
    std::cout << "=== Network Protocol Showcase ===" << std::endl << std::endl;
    
    // 1. Ethernet Header
    std::cout << "1. Ethernet Header:" << std::endl;
    EthernetHeader eth;
    eth.set_dst_mac(0x001122334455ULL);
    eth.set_src_mac(0xAABBCCDDEEFFULL);
    eth.set_ethertype(EtherType::IPv4);
    
    auto eth_data = eth.to_bytes();
    printBytes(eth_data, "Ethernet");
    std::cout << "  Dst MAC: 0x" << std::hex << eth.dst_mac() << std::dec << std::endl;
    std::cout << "  EtherType: IPv4" << std::endl;
    std::cout << std::endl;
    
    // 2. IPv4 Header
    std::cout << "2. IPv4 Header:" << std::endl;
    IPv4Header ipv4;
    ipv4.set_version(4);
    ipv4.set_ihl(5);
    ipv4.set_tos(0);
    ipv4.set_total_length(40);
    ipv4.set_identification(0x1234);
    ipv4.set_dont_fragment(1); // Don't Fragment flag
    ipv4.set_fragment_offset(0);
    ipv4.set_ttl(64);
    ipv4.set_protocol(IPProtocol::TCP);
    ipv4.set_src_ip(0xC0A80001); // 192.168.0.1
    ipv4.set_dst_ip(0x08080808); // 8.8.8.8
    
    auto ipv4_data = ipv4.to_bytes();
    printBytes(ipv4_data, "IPv4");
    std::cout << "  Version: " << (int)ipv4.version() << std::endl;
    std::cout << "  Protocol: TCP" << std::endl;
    std::cout << "  Don't Fragment: " << (int)ipv4.dont_fragment() << std::endl;
    std::cout << std::endl;
    
    // 3. UDP Header
    std::cout << "3. UDP Header:" << std::endl;
    UDPHeader udp;
    udp.set_src_port(53);
    udp.set_dst_port(1234);
    udp.set_length(8);
    udp.set_checksum(0);
    
    auto udp_data = udp.to_bytes();
    printBytes(udp_data, "UDP");
    std::cout << "  Src Port: " << udp.src_port() << " (DNS)" << std::endl;
    std::cout << "  Dst Port: " << udp.dst_port() << std::endl;
    std::cout << "  Length: " << udp.length() << std::endl;
    std::cout << std::endl;
    
    // 4. IPv6 Header
    std::cout << "4. IPv6 Header:" << std::endl;
    IPv6Header ipv6;
    ipv6.set_version(6);
    ipv6.set_traffic_class(0x20);
    ipv6.set_flow_label(0x12345);
    ipv6.set_payload_length(64);
    ipv6.set_next_header(IPProtocol::TCP);
    ipv6.set_hop_limit(64);
    
    auto ipv6_data = ipv6.to_bytes();
    printBytes(ipv6_data, "IPv6");
    std::cout << "  Version: " << (int)ipv6.version() << std::endl;
    std::cout << "  Traffic Class: 0x" << std::hex << (int)ipv6.traffic_class() << std::dec << std::endl;
    std::cout << "  Payload Length: " << ipv6.payload_length() << std::endl;
    std::cout << std::endl;
    
    // 5. ARP Header
    std::cout << "5. ARP Header:" << std::endl;
    ARPHeader arp;
    arp.set_hardware_type(1); // Ethernet
    arp.set_protocol_type(0x0800); // IPv4
    arp.set_hardware_addr_len(6);
    arp.set_protocol_addr_len(4);
    arp.set_operation(1); // Request
    arp.set_sender_hw_addr(0xAABBCCDDEEFFULL);
    arp.set_sender_proto_addr(0xC0A80001); // 192.168.0.1
    arp.set_target_hw_addr(0x000000000000ULL);
    arp.set_target_proto_addr(0xC0A80002); // 192.168.0.2
    
    auto arp_data = arp.to_bytes();
    printBytes(arp_data, "ARP");
    std::cout << "  Hardware Type: " << arp.hardware_type() << " (Ethernet)" << std::endl;
    std::cout << "  Operation: " << arp.operation() << " (Request)" << std::endl;
    std::cout << std::endl;
    
    // 6. ICMP Header  
    std::cout << "6. ICMP Header:" << std::endl;
    ICMPHeader icmp;
    icmp.set_type(ICMPType::ECHO_REQUEST);
    icmp.set_code(0);
    icmp.set_checksum(0);
    icmp.set_identifier(0x1234);
    icmp.set_sequence(1);
    
    auto icmp_data = icmp.to_bytes();
    printBytes(icmp_data, "ICMP");
    std::cout << "  Type: Echo Request" << std::endl;
    std::cout << "  ID: 0x" << std::hex << icmp.identifier() << std::dec << std::endl;
    std::cout << "  Sequence: " << icmp.sequence() << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== Protocol Showcase Complete ===" << std::endl;
    std::cout << "Successfully demonstrated 6 core network protocols!" << std::endl;
    std::cout << "Total protocols available in HDL: 25" << std::endl;
    std::cout << std::endl;
    std::cout << "Available protocols include:" << std::endl;
    std::cout << "- Core: Ethernet, IPv4, IPv6, TCP, UDP, ICMP, ARP" << std::endl;
    std::cout << "- Application: DNS, DHCP, HTTP" << std::endl;
    std::cout << "- Tunneling: GRE, VXLAN, MPLS" << std::endl;
    std::cout << "- Security: ESP, AH (IPSec)" << std::endl;
    std::cout << "- Transport: SCTP" << std::endl;
    std::cout << "- Multicast: IGMP" << std::endl;
    std::cout << "- Routing: OSPF" << std::endl;
    std::cout << "- And more..." << std::endl;
    
    return 0;
}
