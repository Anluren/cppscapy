# PCAP Support in CPPScapy

This document describes the PCAP (Packet Capture) functionality added to CPPScapy, which allows you to read from and write to pcap files - the standard format used by tools like Wireshark, tcpdump, and libpcap.

## Features

- **PCAP Writing**: Create pcap files with custom packets
- **PCAP Reading**: Parse existing pcap files
- **Packet Construction**: Build packets using CPPScapy headers
- **Standard Compliance**: Generated files work with Wireshark, tcpdump, etc.
- **Multiple Link Types**: Support for Ethernet, Raw IP, and other link types

## Quick Start

### Writing PCAP Files

```cpp
#include "pcap_support.h"
#include "header_dsl.h"

using namespace cppscapy;

// Create a PCAP writer
pcap::PcapWriter writer("output.pcap");
if (!writer.open()) {
    // Handle error
    return;
}

// Create an Ethernet header
dsl::EthernetHeader eth;
eth.set_dst_mac(0x001122334455ULL);
eth.set_src_mac(0x665544332211ULL);
eth.set_ethertype(dsl::EtherType::IPv4);

// Create a UDP header
dsl::UDPHeader udp;
udp.set_src_port(12345);
udp.set_dst_port(80);
udp.set_payload_size(5);
udp.update_computed_fields();

// Create packet with payload
std::vector<uint8_t> payload = {'H', 'e', 'l', 'l', 'o'};
auto packet = pcap::utils::create_udp_packet(eth, udp, payload);

// Write to pcap file
writer.write_packet(packet);
writer.close();
```

### Reading PCAP Files

```cpp
#include "pcap_support.h"
#include "header_dsl.h"

using namespace cppscapy;

// Create a PCAP reader
pcap::PcapReader reader("input.pcap");
if (!reader.open()) {
    // Handle error
    return;
}

pcap::Packet packet;
while (reader.read_packet(packet)) {
    // Print packet info
    pcap::utils::print_packet_info(packet);
    
    // Parse headers
    dsl::EthernetHeader eth;
    if (packet.parse_header(eth, 0)) {
        std::cout << "Ethernet - Src: " << std::hex << eth.src_mac() 
                  << ", Dst: " << eth.dst_mac() << std::endl;
        
        // Parse next header (e.g., UDP at offset 14)
        dsl::UDPHeader udp;
        if (packet.parse_header(udp, 14)) {
            std::cout << "UDP - Port " << udp.src_port() 
                      << " -> " << udp.dst_port() << std::endl;
        }
    }
    
    // Hex dump
    pcap::utils::hex_dump(packet, 64);  // First 64 bytes
}

reader.close();
```

## API Reference

### Classes

#### `pcap::Packet`
Represents a network packet with timestamp and data.

**Methods:**
- `add_header<T>(const T& header)` - Add a header to the packet
- `parse_header<T>(T& header, size_t offset)` - Parse header from packet data
- `data()` - Get packet data as vector<uint8_t>
- `size()` - Get packet size in bytes
- `timestamp()` - Get packet timestamp
- `append(const std::vector<uint8_t>&)` - Append raw bytes

#### `pcap::PcapWriter`
Writes packets to pcap files.

**Constructor:**
```cpp
PcapWriter(const std::string& filename, 
           LinkType link_type = LinkType::ETHERNET, 
           uint32_t snaplen = 65535)
```

**Methods:**
- `bool open()` - Open file for writing
- `bool write_packet(const Packet& packet)` - Write a packet
- `void close()` - Close the file
- `bool is_open()` - Check if file is open

#### `pcap::PcapReader`
Reads packets from pcap files.

**Constructor:**
```cpp
PcapReader(const std::string& filename)
```

**Methods:**
- `bool open()` - Open file for reading
- `bool read_packet(Packet& packet)` - Read next packet
- `void close()` - Close the file
- `LinkType get_link_type()` - Get link layer type
- `uint32_t get_snaplen()` - Get snapshot length

### Utility Functions

#### `pcap::utils`

- `create_ethernet_packet(eth_header, payload)` - Create Ethernet packet
- `create_udp_packet(eth_header, udp_header, payload)` - Create UDP packet
- `create_tcp_packet(eth_header, tcp_header, payload)` - Create TCP packet
- `print_packet_info(packet)` - Print packet information
- `hex_dump(packet, max_bytes)` - Print hex dump of packet
- `create_sample_packet()` - Create a sample packet for testing

### Link Types

The `LinkType` enum supports various data link layer types:

```cpp
enum class LinkType : uint32_t {
    NULL_LINK = 0,        // BSD loopback
    ETHERNET = 1,         // Ethernet
    IEEE802_5 = 6,        // Token Ring
    PPP = 9,              // PPP
    FDDI = 10,            // FDDI
    RAW = 12,             // Raw IP
    IEEE802_11 = 105,     // WiFi
    LINUX_SLL = 113,      // Linux cooked
    IEEE802_11_RADIOTAP = 127  // Radiotap
};
```

## Examples

### Creating Different Packet Types

#### TCP SYN Packet
```cpp
dsl::EthernetHeader eth;
eth.set_dst_mac(0xffffffffffff);  // Broadcast
eth.set_src_mac(0x001122334455);
eth.set_ethertype(dsl::EtherType::IPv4);

dsl::TCPHeader tcp;
tcp.set_src_port(443);
tcp.set_dst_port(8080);
tcp.set_seq_num(0x12345678);
tcp.set_flag_syn(true);
tcp.set_window_size(8192);
tcp.update_computed_fields();

auto packet = pcap::utils::create_tcp_packet(eth, tcp, {});
```

#### Custom Timestamp
```cpp
pcap::Packet packet;
packet.set_data({0x01, 0x02, 0x03, 0x04});

// Set custom timestamp
auto custom_time = std::chrono::system_clock::now() - std::chrono::hours(1);
packet.set_timestamp(custom_time);
```

### Processing Captured Traffic

```cpp
pcap::PcapReader reader("captured_traffic.pcap");
reader.open();

int tcp_count = 0, udp_count = 0;
pcap::Packet packet;

while (reader.read_packet(packet)) {
    dsl::EthernetHeader eth;
    if (packet.parse_header(eth, 0)) {
        // Check for TCP
        dsl::TCPHeader tcp;
        if (packet.parse_header(tcp, 14)) {
            tcp_count++;
            std::cout << "TCP: " << tcp.src_port() << " -> " << tcp.dst_port() << std::endl;
        }
        
        // Check for UDP
        dsl::UDPHeader udp;
        if (packet.parse_header(udp, 14)) {
            udp_count++;
            std::cout << "UDP: " << udp.src_port() << " -> " << udp.dst_port() << std::endl;
        }
    }
}

std::cout << "Found " << tcp_count << " TCP and " << udp_count << " UDP packets" << std::endl;
```

## Integration with Wireshark

The generated pcap files are fully compatible with Wireshark:

```bash
# View with Wireshark
wireshark demo_output.pcap

# Or analyze with tcpdump
tcpdump -r demo_output.pcap -v

# Or use tshark for text output
tshark -r demo_output.pcap
```

## Building

Make sure to include the pcap support in your CMakeLists.txt:

```cmake
add_executable(my_pcap_app main.cpp)
target_link_libraries(my_pcap_app cppscapy)
```

The pcap support is automatically included when you link with the cppscapy library.

## Error Handling

Always check return values:

```cpp
pcap::PcapWriter writer("output.pcap");
if (!writer.open()) {
    std::cerr << "Failed to open pcap file for writing" << std::endl;
    return -1;
}

if (!writer.write_packet(packet)) {
    std::cerr << "Failed to write packet" << std::endl;
}
```

## Performance Notes

- The pcap implementation uses standard C++ streams
- For high-performance applications, consider buffering multiple packets
- Memory usage scales with packet size and count
- Binary I/O is used for efficiency

## Limitations

- Currently supports pcap format (not pcapng)
- Timestamps are limited to microsecond precision
- Maximum packet size is 65535 bytes (standard snaplen)
- No compression support

## Future Enhancements

- Support for pcapng format
- Nanosecond timestamp precision
- Compression support
- Memory-mapped file I/O for large files
- Async I/O support
