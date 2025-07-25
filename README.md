# CppScapy - Network Header Construction Library

A convenient C++ library for constructing standard network headers (MAC, IPv4, IPv6, TCP, UDP, ICMP, etc.) with a clean, fluent API and comprehensive Header Definition Language (HDL) compiler supporting both HDL and XML formats.

## Features

- **Dual Format Support**: Both HDL and XML formats for defining network protocol headers
- **HDL Compiler**: Generates type-safe C++ classes from HDL or XML definitions
- **XML Schema Validation**: XSD schema support for XML protocol definitions
- **Format Converter**: Convert between HDL and XML formats seamlessly
- **25+ Protocol Headers**: Comprehensive protocol support including IPv4/IPv6, TCP/UDP, DHCP, DNS, VPN protocols
- **MAC Address Support**: Parse, create, and manipulate MAC addresses
- **IPv4/IPv6 Address Support**: Handle IP addresses with various input formats
- **Network Headers**: Ethernet, IPv4, IPv6, TCP, UDP, ICMP header construction
- **Packet Builder**: Fluent API for building complete network packets
- **Convenience Patterns**: Pre-built patterns for common packet types
- **Header Validation**: Automatic checksum calculation and validation
- **Type Safety**: Compile-time type checking and enum validation
- **Bit-Field Manipulation**: Precise control over protocol field layouts
- **Rich Documentation**: Embedded documentation support in XML format
- **VS Code Integration**: Complete debugging and development environment setup

## Requirements

### For Python Package (HDL Compiler Tools)
- **Python 3.7+** (Python 3.8+ recommended)  
- Standard library only (no external dependencies)
- Optional: `xmllint` for XML schema validation

### For C++ Library
- **C++17** compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- CMake 3.10+ for building

## Quick Start

### Building

```bash
mkdir build
cd build
cmake ..
make
```

## Python Package Installation

The HDL compiler tools are now available as a pip-installable Python package:

### Install from Source
```bash
cd python-package
pip install -e .
```

### Usage
```bash
# Compile HDL/XML to C++ headers
hdl-compile examples/network_protocols.xml -o generated_headers.h
hdl-compile examples/network_protocols.hdl -o generated_headers.h

# Convert between formats
hdl-convert protocols.xml -o protocols.hdl
hdl-convert protocols.hdl -o protocols.xml

# Python API usage
python3 -c "
from cppscapy import HDLParser, CPPCodeGenerator
parser = HDLParser('protocols.hdl')
headers, enums = parser.parse()
generator = CPPCodeGenerator(headers, enums)
print(generator.generate())
"
```

### Legacy Tool Usage (Deprecated)

The standalone tools in `tools/` have been moved to the Python package. For legacy usage:

```bash
# Legacy - use Python package instead
python3 tools/hdl_compiler.py examples/network_protocols.xml -o generated_headers.h
```

### Format Conversion

```bash
# Convert HDL to XML
python3 tools/format_converter.py input.hdl -o output.xml

# Convert XML to HDL  
python3 tools/format_converter.py input.xml -o output.hdl
```

### Running Examples

```bash
# Run the comprehensive demo
./demo

# Run unit tests
./simple_test
```

## Format Comparison

### HDL Format (Concise)
```hdl
enum EtherType : uint16_t {
    IPv4 = 0x0800,
    IPv6 = 0x86DD
}

header EthernetHeader {
    dst_mac: 48;
    src_mac: 48;
    ethertype: EtherType;
}
```

### XML Format (Rich Metadata)
```xml
<enum name="EtherType" underlying_type="uint16_t">
    <value name="IPv4" value="0x0800"/>
    <value name="IPv6" value="0x86DD"/>
</enum>

<header name="EthernetHeader">
    <description>Standard Ethernet frame header</description>
    <field name="dst_mac" bit_width="48" type="integer">
        <description>Destination MAC address</description>
    </field>
    <field name="src_mac" bit_width="48" type="integer">
        <description>Source MAC address</description>
    </field>
    <field name="ethertype" bit_width="16" type="enum" enum_type="EtherType">
        <description>EtherType field</description>
    </field>
</header>
```

## New Features

### ✨ Compile-Time Length Deduction

CppScapy now supports automatic length deduction for hex strings at compile time! No need to specify array sizes manually:

```cpp
#include "utils.h"
using namespace cppscapy::utils;

// Automatically deduces array size from hex string length
constexpr auto mac_bytes = from_hex_string_auto("001122334455");    // 6 bytes
constexpr auto ipv4_bytes = from_hex_string_auto("C0A80101");       // 4 bytes
constexpr auto custom_bytes = from_hex_string_auto("ABCD");         // 2 bytes

// Compile-time validation
static_assert(mac_bytes.size() == 6);
static_assert(ipv4_bytes.size() == 4);
static_assert(custom_bytes.size() == 2);

// Works with any hex string length
constexpr auto single_byte = from_hex_string_auto("FF");            // 1 byte
constexpr auto long_key = from_hex_string_auto("0123456789ABCDEF"); // 8 bytes
```

For more details, see [COMPILE_TIME_LENGTH_DEDUCTION.md](COMPILE_TIME_LENGTH_DEDUCTION.md).

## API Overview

### MAC Address

```cpp
#include "network_headers.h"
using namespace cppscapy;

// Create MAC addresses
MacAddress mac1("aa:bb:cc:dd:ee:ff");
MacAddress mac2(0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF);

// Predefined addresses
MacAddress broadcast = MacAddress::broadcast();
MacAddress multicast = MacAddress::multicast_ipv4();

// Check properties
bool is_broadcast = mac1.is_broadcast();
bool is_multicast = mac1.is_multicast();

// Convert to string
std::string mac_str = mac1.to_string(); // "aa:bb:cc:dd:ee:ff"
```

### IPv4 Address

```cpp
// Create IPv4 addresses
IPv4Address ip1("192.168.1.1");
IPv4Address ip2(192, 168, 1, 1);

// Predefined addresses
IPv4Address localhost = IPv4Address::localhost();
IPv4Address any = IPv4Address::any();

// Convert formats
std::string ip_str = ip1.to_string();
uint32_t ip_int = ip1.to_uint32();
std::array<uint8_t, 4> ip_bytes = ip1.to_bytes();
```

### IPv6 Address

```cpp
// Create IPv6 addresses
IPv6Address ipv6_1("2001:db8::1");
IPv6Address localhost = IPv6Address::localhost();

// Convert to string
std::string ipv6_str = ipv6_1.to_string();
```

### Ethernet Header

```cpp
MacAddress src("aa:bb:cc:dd:ee:ff");
MacAddress dst("11:22:33:44:55:66");

EthernetHeader eth(dst, src, EthernetHeader::ETHERTYPE_IPV4);

// Fluent API
eth.dst(dst).src(src).ethertype(EthernetHeader::ETHERTYPE_IPV6);

// Get raw bytes
std::vector<uint8_t> bytes = eth.to_bytes();
```

### IPv4 Header

```cpp
IPv4Address src("192.168.1.1");
IPv4Address dst("10.0.0.1");

IPv4Header ip(src, dst, IPv4Header::PROTOCOL_TCP);

// Fluent API for configuration
ip.ttl(64)
  .id(12345)
  .length(60)
  .flags(IPv4Header::FLAG_DONT_FRAGMENT);

// Common protocols
IPv4Header::PROTOCOL_TCP    // 6
IPv4Header::PROTOCOL_UDP    // 17
IPv4Header::PROTOCOL_ICMP   // 1
```

### TCP Header

```cpp
TCPHeader tcp(80, 8080); // src_port, dst_port

// Configure with fluent API
tcp.seq_num(1000)
   .ack_num(2000)
   .flags(TCPHeader::FLAG_SYN | TCPHeader::FLAG_ACK)
   .window_size(8192);

// Common flags
TCPHeader::FLAG_SYN     // 0x02
TCPHeader::FLAG_ACK     // 0x10
TCPHeader::FLAG_FIN     // 0x01
TCPHeader::FLAG_RST     // 0x04
```

### UDP Header

```cpp
UDPHeader udp(53, 12345, 28); // src_port, dst_port, length

// Simple configuration
udp.src_port(53).dst_port(12345).length(28);
```

### ICMP Header

```cpp
ICMPHeader icmp(ICMPHeader::TYPE_ECHO_REQUEST, 0);

// Configure for ping
icmp.identifier(1).sequence(1);

// Common types
ICMPHeader::TYPE_ECHO_REQUEST   // 8
ICMPHeader::TYPE_ECHO_REPLY     // 0
ICMPHeader::TYPE_DEST_UNREACHABLE // 3
```

### PacketBuilder - Fluent Packet Construction

```cpp
PacketBuilder builder;

// Build a complete packet
auto packet = builder
    .ethernet(EthernetHeader(dst_mac, src_mac, EthernetHeader::ETHERTYPE_IPV4))
    .ipv4(IPv4Header(src_ip, dst_ip, IPv4Header::PROTOCOL_TCP))
    .tcp(TCPHeader(80, 8080).flags(TCPHeader::FLAG_SYN))
    .payload("Hello, World!")
    .build();
```

### Convenience Patterns

```cpp
// Quick packet creation
auto syn_packet = patterns::tcp_syn(src_ip, dst_ip, 12345, 80);
auto udp_packet = patterns::udp_packet(src_ip, dst_ip, 12345, 53, payload);
auto ping_packet = patterns::icmp_ping(src_ip, dst_ip, 1, 1);
auto eth_frame = patterns::ethernet_frame(src_mac, dst_mac, 
                                         EthernetHeader::ETHERTYPE_IPV4, 
                                         payload);
```

## Advanced Usage

### Custom Protocol Headers

You can extend the library by creating custom header classes that follow the same pattern:

```cpp
class CustomHeader {
public:
    CustomHeader& field1(uint32_t value) { field1_ = value; return *this; }
    CustomHeader& field2(uint16_t value) { field2_ = value; return *this; }
    
    std::vector<uint8_t> to_bytes() const {
        std::vector<uint8_t> result;
        // Serialize fields to bytes
        return result;
    }
    
private:
    uint32_t field1_ = 0;
    uint16_t field2_ = 0;
};
```

### Complex Packet Construction

```cpp
// Build a complete Ethernet/IPv4/TCP packet with payload
PacketBuilder builder;

std::string http_request = "GET / HTTP/1.1\r\nHost: example.com\r\n\r\n";

auto packet = builder
    .ethernet(EthernetHeader(dst_mac, src_mac, EthernetHeader::ETHERTYPE_IPV4))
    .ipv4(IPv4Header(src_ip, dst_ip, IPv4Header::PROTOCOL_TCP)
          .length(IPv4Header::MIN_SIZE + TCPHeader::MIN_SIZE + http_request.size())
          .ttl(64))
    .tcp(TCPHeader(12345, 80)
         .flags(TCPHeader::FLAG_PSH | TCPHeader::FLAG_ACK)
         .seq_num(1000)
         .ack_num(2000))
    .payload(http_request)
    .build();
```

## Requirements

- C++14 or later
- CMake 3.10 or later
- Standard POSIX networking headers (for inet_pton/inet_ntop)

## Project Structure

```
cppscapy/
├── include/
│   └── network_headers.h     # Main header file
├── src/
│   ├── network_headers.cpp   # Core implementation
│   └── tcp_udp_icmp.cpp     # Protocol implementations
├── examples/
│   ├── demo.cpp             # Comprehensive demo
│   └── simple_test.cpp      # Unit tests
├── CMakeLists.txt           # Build configuration
└── README.md               # This file
```

## Contributing

Feel free to extend the library with additional protocols, improve checksum calculations, or add more convenience patterns. The design is modular and extensible.

## License

This project is open source. Feel free to use, modify, and distribute as needed.
