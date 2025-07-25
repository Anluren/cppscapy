#include "generated_headers.h"
#include <iostream>
#include <iomanip>
#include <sstream>

using namespace cppscapy::dsl;

// Helper function to print bytes in hex
std::string bytes_to_hex(const std::vector<uint8_t>& bytes) {
    std::stringstream ss;
    for (size_t i = 0; i < bytes.size(); ++i) {
        if (i > 0) ss << " ";
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(bytes[i]);
    }
    return ss.str();
}

void demonstrate_generated_headers() {
    std::cout << "=== Generated Headers from HDL Demo ===\n\n";
    
    // Create Ethernet header
    std::cout << "1. Ethernet Header (Generated from HDL)\n";
    std::cout << "---------------------------------------\n";
    
    EthernetHeader eth;
    eth.set_dst_mac(0x001122334455ULL);
    eth.set_src_mac(0x665544332211ULL);
    eth.set_ethertype(EtherType::IPv4);
    
    std::cout << "Field Values:\n";
    std::cout << "  Destination MAC: 0x" << std::hex << std::setw(12) << std::setfill('0') 
              << eth.dst_mac() << std::dec << "\n";
    std::cout << "  Source MAC: 0x" << std::hex << std::setw(12) << std::setfill('0') 
              << eth.src_mac() << std::dec << "\n";
    std::cout << "  EtherType: ";
    
    switch (eth.ethertype()) {
        case EtherType::IPv4: std::cout << "IPv4 (0x0800)"; break;
        case EtherType::IPv6: std::cout << "IPv6 (0x86DD)"; break;
        case EtherType::ARP: std::cout << "ARP (0x0806)"; break;
        default: std::cout << "0x" << std::hex << static_cast<uint16_t>(eth.ethertype()) << std::dec; break;
    }
    std::cout << "\n";
    
    auto eth_bytes = eth.to_bytes();
    std::cout << "  Serialized (" << eth_bytes.size() << " bytes): " << bytes_to_hex(eth_bytes) << "\n";
    std::cout << "  Header size: " << eth.size_bits() << " bits (" << eth.size_bytes() << " bytes)\n\n";
    
    // Create IPv4 header
    std::cout << "2. IPv4 Header (Generated from HDL)\n";
    std::cout << "-----------------------------------\n";
    
    IPv4Header ipv4;
    ipv4.set_src_ip(0xC0A80101);  // 192.168.1.1
    ipv4.set_dst_ip(0xC0A80102);  // 192.168.1.2
    ipv4.set_protocol(IPProtocol::TCP);
    ipv4.set_ttl(64);
    ipv4.update_computed_fields();
    
    std::cout << "Field Values:\n";
    std::cout << "  Version: " << static_cast<int>(ipv4.version()) << "\n";
    std::cout << "  IHL: " << static_cast<int>(ipv4.ihl()) << "\n";
    std::cout << "  TTL: " << static_cast<int>(ipv4.ttl()) << "\n";
    std::cout << "  Protocol: ";
    
    switch (ipv4.protocol()) {
        case IPProtocol::ICMP: std::cout << "ICMP (1)"; break;
        case IPProtocol::TCP: std::cout << "TCP (6)"; break;
        case IPProtocol::UDP: std::cout << "UDP (17)"; break;
        default: std::cout << static_cast<int>(ipv4.protocol()); break;
    }
    std::cout << "\n";
    
    std::cout << "  Source IP: " << ((ipv4.src_ip() >> 24) & 0xFF) << "." 
              << ((ipv4.src_ip() >> 16) & 0xFF) << "." 
              << ((ipv4.src_ip() >> 8) & 0xFF) << "." 
              << (ipv4.src_ip() & 0xFF) << "\n";
    
    std::cout << "  Dest IP: " << ((ipv4.dst_ip() >> 24) & 0xFF) << "." 
              << ((ipv4.dst_ip() >> 16) & 0xFF) << "." 
              << ((ipv4.dst_ip() >> 8) & 0xFF) << "." 
              << (ipv4.dst_ip() & 0xFF) << "\n";
    
    auto ipv4_bytes = ipv4.to_bytes();
    std::cout << "  Serialized (" << ipv4_bytes.size() << " bytes): " << bytes_to_hex(ipv4_bytes) << "\n";
    std::cout << "  Header size: " << ipv4.size_bits() << " bits (" << ipv4.size_bytes() << " bytes)\n\n";
    
    // Create TCP header with individual flag control
    std::cout << "3. TCP Header with Individual Flags (Generated from HDL)\n";
    std::cout << "--------------------------------------------------------\n";
    
    TCPHeader tcp;
    tcp.set_src_port(443);
    tcp.set_dst_port(12345);
    tcp.set_seq_num(0x12345678);
    tcp.set_ack_num(0x87654321);
    tcp.set_window_size(8192);
    
    // Set individual flags - this is the power of the DSL!
    tcp.set_flag_syn(true);
    tcp.set_flag_ack(true);
    tcp.set_flag_psh(false);
    tcp.set_flag_fin(false);
    
    tcp.update_computed_fields();
    
    std::cout << "Field Values:\n";
    std::cout << "  Source Port: " << tcp.src_port() << "\n";
    std::cout << "  Dest Port: " << tcp.dst_port() << "\n";
    std::cout << "  Sequence: 0x" << std::hex << tcp.seq_num() << std::dec << "\n";
    std::cout << "  Acknowledgment: 0x" << std::hex << tcp.ack_num() << std::dec << "\n";
    std::cout << "  Data Offset: " << static_cast<int>(tcp.data_offset()) << "\n";
    std::cout << "  Window Size: " << tcp.window_size() << "\n";
    
    std::cout << "  TCP Flags:\n";
    std::cout << "    SYN: " << (tcp.flag_syn() ? "true" : "false") << "\n";
    std::cout << "    ACK: " << (tcp.flag_ack() ? "true" : "false") << "\n";
    std::cout << "    PSH: " << (tcp.flag_psh() ? "true" : "false") << "\n";
    std::cout << "    FIN: " << (tcp.flag_fin() ? "true" : "false") << "\n";
    std::cout << "    RST: " << (tcp.flag_rst() ? "true" : "false") << "\n";
    std::cout << "    URG: " << (tcp.flag_urg() ? "true" : "false") << "\n";
    
    auto tcp_bytes = tcp.to_bytes();
    std::cout << "  Serialized (" << tcp_bytes.size() << " bytes): " << bytes_to_hex(tcp_bytes) << "\n";
    std::cout << "  Header size: " << tcp.size_bits() << " bits (" << tcp.size_bytes() << " bytes)\n\n";
    
    // Show flag byte details
    std::cout << "4. Detailed Flag Analysis\n";
    std::cout << "-------------------------\n";
    
    uint8_t flags_byte = tcp_bytes[13]; // TCP flags are in byte 13
    std::cout << "  Flags byte: 0x" << std::hex << std::setw(2) << std::setfill('0') 
              << static_cast<int>(flags_byte) << std::dec << "\n";
    std::cout << "  Binary:     ";
    for (int i = 7; i >= 0; --i) {
        std::cout << ((flags_byte >> i) & 1);
    }
    std::cout << "\n";
    std::cout << "  Meaning:    |NS|CWR|ECE|URG|ACK|PSH|RST|SYN|FIN|\n";
    std::cout << "              |" << (tcp.flag_ns() ? "1" : "0") << " |"
              << (tcp.flag_cwr() ? "1" : "0") << " |"
              << (tcp.flag_ece() ? "1" : "0") << " |"
              << (tcp.flag_urg() ? "1" : "0") << " |"
              << (tcp.flag_ack() ? "1" : "0") << " |"
              << (tcp.flag_psh() ? "1" : "0") << " |"
              << (tcp.flag_rst() ? "1" : "0") << " |"
              << (tcp.flag_syn() ? "1" : "0") << " |"
              << (tcp.flag_fin() ? "1" : "0") << "|\n\n";
}

void demonstrate_serialization_roundtrip() {
    std::cout << "5. Serialization Round-trip Test\n";
    std::cout << "--------------------------------\n";
    
    // Create original header
    UDPHeader original;
    original.set_src_port(12345);
    original.set_dst_port(80);
    original.update_computed_fields();
    
    std::cout << "Original UDP Header:\n";
    std::cout << "  Source Port: " << original.src_port() << "\n";
    std::cout << "  Dest Port: " << original.dst_port() << "\n";
    
    // Serialize to bytes
    auto bytes = original.to_bytes();
    std::cout << "  Serialized: " << bytes_to_hex(bytes) << "\n";
    
    // Deserialize back to header
    UDPHeader restored;
    if (restored.from_bytes(bytes)) {
        std::cout << "Restored UDP Header:\n";
        std::cout << "  Source Port: " << restored.src_port() << "\n";
        std::cout << "  Dest Port: " << restored.dst_port() << "\n";
        std::cout << "  Round-trip: " << (original.src_port() == restored.src_port() && 
                                         original.dst_port() == restored.dst_port() ? "SUCCESS" : "FAILED") << "\n\n";
    } else {
        std::cout << "  Deserialization FAILED\n\n";
    }
}

int main() {
    std::cout << "HDL Compiler Demonstration\n";
    std::cout << "==========================\n\n";
    
    std::cout << "This demonstrates C++ headers automatically generated\n";
    std::cout << "from HDL (Header Definition Language) source files.\n\n";
    
    demonstrate_generated_headers();
    demonstrate_serialization_roundtrip();
    
    std::cout << "=== HDL Compiler Benefits ===\n";
    std::cout << "✓ Automatic C++ code generation from declarative HDL\n";
    std::cout << "✓ Type-safe field access with compile-time validation\n";
    std::cout << "✓ Precise bit-level field control (individual TCP flags)\n";
    std::cout << "✓ Automatic serialization/deserialization\n";
    std::cout << "✓ Computed field support (lengths, checksums)\n";
    std::cout << "✓ Enum support with meaningful names\n";
    std::cout << "✓ Self-documenting protocol definitions\n";
    std::cout << "✓ Maintainable and extensible protocol implementations\n\n";
    
    std::cout << "The HDL source file (network_protocols.hdl) is much more\n";
    std::cout << "readable and maintainable than hand-written C++ classes!\n";
    
    return 0;
}
