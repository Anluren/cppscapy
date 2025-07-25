#include "header_dsl.h"
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

// Print binary representation of a value
std::string to_binary(uint64_t value, size_t bits) {
    std::string result;
    for (int i = bits - 1; i >= 0; --i) {
        result += ((value >> i) & 1) ? '1' : '0';
        if (i % 8 == 0 && i > 0) result += " ";
    }
    return result;
}

void demonstrate_ethernet_header() {
    std::cout << "=== Ethernet Header DSL Demo ===\n\n";
    
    // Create Ethernet header using DSL-generated class
    EthernetHeader eth;
    eth.set_dst_mac(0x001122334455ULL);
    eth.set_src_mac(0x665544332211ULL);
    eth.set_ethertype(EtherType::IPv4);
    
    std::cout << "Field Values:\n";
    std::cout << "Destination MAC: 0x" << std::hex << std::setw(12) << std::setfill('0') 
              << eth.dst_mac() << std::dec << "\n";
    std::cout << "Source MAC: 0x" << std::hex << std::setw(12) << std::setfill('0') 
              << eth.src_mac() << std::dec << "\n";
    std::cout << "EtherType: 0x" << std::hex << std::setw(4) << std::setfill('0') 
              << static_cast<uint16_t>(eth.ethertype()) << std::dec << "\n\n";
    
    // Serialize to bytes
    auto bytes = eth.to_bytes();
    std::cout << "Serialized bytes (" << bytes.size() << " bytes):\n";
    std::cout << bytes_to_hex(bytes) << "\n\n";
    
    // Show bit layout
    std::cout << "Bit Layout:\n";
    std::cout << "DST MAC (48 bits): " << to_binary(eth.dst_mac(), 48) << "\n";
    std::cout << "SRC MAC (48 bits): " << to_binary(eth.src_mac(), 48) << "\n";
    std::cout << "EtherType (16 bits): " << to_binary(static_cast<uint16_t>(eth.ethertype()), 16) << "\n\n";
}

void demonstrate_udp_header() {
    std::cout << "=== UDP Header DSL Demo ===\n\n";
    
    UDPHeader udp;
    udp.set_src_port(12345);
    udp.set_dst_port(80);
    udp.set_payload_size(10); // 10 bytes of payload
    udp.update_computed_fields(); // Calculate length field
    
    std::cout << "Field Values:\n";
    std::cout << "Source Port: " << udp.src_port() << "\n";
    std::cout << "Destination Port: " << udp.dst_port() << "\n";
    std::cout << "Length: " << udp.length() << " (computed)\n";
    std::cout << "Checksum: 0x" << std::hex << std::setw(4) << std::setfill('0') 
              << udp.checksum() << std::dec << " (computed)\n\n";
    
    auto bytes = udp.to_bytes();
    std::cout << "Serialized bytes (" << bytes.size() << " bytes):\n";
    std::cout << bytes_to_hex(bytes) << "\n\n";
    
    std::cout << "Header size: " << udp.size_bits() << " bits (" 
              << udp.size_bytes() << " bytes)\n\n";
}

void demonstrate_tcp_flags() {
    std::cout << "=== TCP Header Flags DSL Demo ===\n\n";
    
    TCPHeader tcp;
    tcp.set_src_port(443);
    tcp.set_dst_port(12345);
    tcp.set_seq_num(0x12345678);
    tcp.set_ack_num(0x87654321);
    
    // Set various flags
    tcp.set_flag_syn(true);
    tcp.set_flag_ack(true);
    tcp.set_window_size(8192);
    tcp.update_computed_fields();
    
    std::cout << "Field Values:\n";
    std::cout << "Source Port: " << tcp.src_port() << "\n";
    std::cout << "Destination Port: " << tcp.dst_port() << "\n";
    std::cout << "Sequence Number: 0x" << std::hex << tcp.seq_num() << std::dec << "\n";
    std::cout << "Acknowledgment Number: 0x" << std::hex << tcp.ack_num() << std::dec << "\n";
    std::cout << "Data Offset: " << static_cast<int>(tcp.data_offset()) << " (computed)\n";
    std::cout << "Window Size: " << tcp.window_size() << "\n\n";
    
    std::cout << "TCP Flags:\n";
    std::cout << "SYN: " << (tcp.flag_syn() ? "true" : "false") << "\n";
    std::cout << "ACK: " << (tcp.flag_ack() ? "true" : "false") << "\n";
    std::cout << "FIN: " << (tcp.flag_fin() ? "true" : "false") << "\n\n";
    
    auto bytes = tcp.to_bytes();
    std::cout << "Serialized bytes (" << bytes.size() << " bytes):\n";
    std::cout << bytes_to_hex(bytes) << "\n\n";
}

void demonstrate_bit_precision() {
    std::cout << "=== Bit-Level Precision Demo ===\n\n";
    
    // Show how individual bits are controlled
    TCPHeader tcp;
    
    std::cout << "Setting individual flag bits:\n";
    
    // Clear all flags first
    auto initial_bytes = tcp.to_bytes();
    std::cout << "Initial flags byte: 0x" << std::hex << std::setw(2) << std::setfill('0') 
              << static_cast<int>(initial_bytes[13]) << std::dec << "\n";
    
    // Set SYN flag (bit 1 from right)
    tcp.set_flag_syn(true);
    auto syn_bytes = tcp.to_bytes();
    std::cout << "After setting SYN: 0x" << std::hex << std::setw(2) << std::setfill('0') 
              << static_cast<int>(syn_bytes[13]) << std::dec << "\n";
    
    // Set ACK flag (bit 4 from right)
    tcp.set_flag_ack(true);
    auto ack_bytes = tcp.to_bytes();
    std::cout << "After setting ACK: 0x" << std::hex << std::setw(2) << std::setfill('0') 
              << static_cast<int>(ack_bytes[13]) << std::dec << "\n";
    
    // Show binary representation
    std::cout << "Flags byte binary: " << to_binary(ack_bytes[13], 8) << "\n";
    std::cout << "                   ^     ^  (ACK, SYN)\n\n";
}

void demonstrate_deserialization() {
    std::cout << "=== Deserialization Demo ===\n\n";
    
    // Create some test data (Ethernet header)
    std::vector<uint8_t> eth_data = {
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55,  // Destination MAC
        0x66, 0x55, 0x44, 0x33, 0x22, 0x11,  // Source MAC
        0x08, 0x00                             // EtherType (IPv4)
    };
    
    std::cout << "Input bytes: " << bytes_to_hex(eth_data) << "\n\n";
    
    // Parse using DSL-generated class
    EthernetHeader eth;
    if (eth.from_bytes(eth_data)) {
        std::cout << "Successfully parsed:\n";
        std::cout << "Destination MAC: 0x" << std::hex << std::setw(12) << std::setfill('0') 
                  << eth.dst_mac() << std::dec << "\n";
        std::cout << "Source MAC: 0x" << std::hex << std::setw(12) << std::setfill('0') 
                  << eth.src_mac() << std::dec << "\n";
        std::cout << "EtherType: 0x" << std::hex << std::setw(4) << std::setfill('0') 
                  << static_cast<uint16_t>(eth.ethertype()) << std::dec;
        
        if (eth.ethertype() == EtherType::IPv4) {
            std::cout << " (IPv4)";
        }
        std::cout << "\n\n";
        
        std::cout << "Header is valid: " << (eth.is_valid() ? "yes" : "no") << "\n\n";
    } else {
        std::cout << "Failed to parse header\n\n";
    }
}

int main() {
    std::cout << "Network Header DSL Demonstration\n";
    std::cout << "=================================\n\n";
    
    std::cout << "This demo shows how a Domain-Specific Language (DSL)\n";
    std::cout << "for network headers would work with precise bit-field control.\n\n";
    
    demonstrate_ethernet_header();
    demonstrate_udp_header();
    demonstrate_tcp_flags();
    demonstrate_bit_precision();
    demonstrate_deserialization();
    
    std::cout << "=== DSL Benefits ===\n";
    std::cout << "✓ Type-safe field access\n";
    std::cout << "✓ Automatic bit packing/unpacking\n";
    std::cout << "✓ Computed field support\n";
    std::cout << "✓ Precise bit-level control\n";
    std::cout << "✓ Validation and error checking\n";
    std::cout << "✓ Clean, readable code generation\n";
    std::cout << "✓ Self-documenting header definitions\n\n";
    
    return 0;
}
