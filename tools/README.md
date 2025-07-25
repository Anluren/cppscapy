# HDL Compiler - Header Definition Language Compiler

The HDL Compiler is a tool that generates C++ header classes from declarative Header Definition Language (HDL) files. It's designed specifically for network protocol implementation, providing precise bit-level field control and type-safe access.

## Features

- **Declarative Syntax**: Define headers using simple, readable HDL syntax
- **Bit-Level Precision**: Exact control over field bit widths and positions
- **Type Safety**: Generated C++ code provides compile-time type checking
- **Automatic Code Generation**: Produces efficient C++ classes with serialization
- **Enum Support**: Named constants with underlying type specification
- **Computed Fields**: Automatic calculation of lengths, checksums, etc.
- **Individual Flag Control**: Set and get individual bits in flag fields
- **Validation**: Built-in field validation and constraint checking

## Installation

The HDL compiler is a Python 3 script with no external dependencies:

```bash
chmod +x tools/hdl_compiler.py
```

## Usage

### Basic Usage

```bash
python3 tools/hdl_compiler.py input.hdl -o output.h
```

### Command Line Options

```bash
python3 tools/hdl_compiler.py --help
```

Options:
- `-o, --output`: Output C++ header file
- `--namespace`: C++ namespace for generated code (default: cppscapy::dsl)
- `-v, --verbose`: Enable verbose output

### Example

```bash
# Compile network protocols
python3 tools/hdl_compiler.py examples/network_protocols.hdl -o include/generated_headers.h --verbose

# Use in your C++ code
#include "generated_headers.h"
using namespace cppscapy::dsl;
```

## HDL Syntax

### Basic Header Definition

```hdl
header EthernetHeader {
    dst_mac: 48;        // 48-bit destination MAC
    src_mac: 48;        // 48-bit source MAC  
    ethertype: 16;      // 16-bit EtherType
}
```

### Enums

```hdl
enum EtherType : uint16_t {
    IPv4 = 0x0800,
    IPv6 = 0x86DD,
    ARP = 0x0806,
    MPLS = 0x8847
}

header EthernetHeader {
    dst_mac: 48;
    src_mac: 48;
    ethertype: EtherType;  // Use enum type
}
```

### Default Values

```hdl
header IPv4Header {
    version: 4 = 4;      // Default value of 4
    ihl: 4 = 5;          // Default IHL of 5
    ttl: 8 = 64;         // Default TTL of 64
    protocol: 8;
}
```

### Computed Fields

```hdl
header UDPHeader {
    src_port: 16;
    dst_port: 16;
    length: 16 [computed];    // Auto-calculated
    checksum: 16 [computed];  // Auto-calculated
}
```

### Individual Flag Fields

```hdl
header TCPHeader {
    src_port: 16;
    dst_port: 16;
    seq_num: 32;
    ack_num: 32;
    data_offset: 4 = 5;
    reserved: 3 = 0;
    flag_ns: 1;           // Individual flag bits
    flag_cwr: 1;
    flag_ece: 1;
    flag_urg: 1;
    flag_ack: 1;
    flag_psh: 1;
    flag_rst: 1;
    flag_syn: 1;
    flag_fin: 1;
    window_size: 16;
    checksum: 16 [computed];
    urgent_ptr: 16;
}
```

### Header Attributes

```hdl
header MPLSHeader [stackable] {
    label: 20;
    traffic_class: 3;
    bottom_of_stack: 1;
    ttl: 8;
}
```

## Generated C++ Interface

Each HDL header generates a C++ class with:

### Basic Interface

```cpp
class HeaderName : public HeaderBase {
public:
    // Constructors
    HeaderName();
    
    // Field accessors
    uint32_t field_name() const;
    void set_field_name(uint32_t value);
    
    // Enum field accessors
    EnumType enum_field() const;
    void set_enum_field(EnumType value);
    
    // HeaderBase implementation
    std::vector<uint8_t> to_bytes() const override;
    bool from_bytes(const std::vector<uint8_t>& data) override;
    size_t size_bits() const override;
    size_t size_bytes() const override;
    bool is_valid() const override;
    void update_computed_fields() override;
};
```

### Usage Example

```cpp
#include "generated_headers.h"
using namespace cppscapy::dsl;

// Create and configure headers
EthernetHeader eth;
eth.set_dst_mac(0x001122334455ULL);
eth.set_src_mac(0x665544332211ULL);
eth.set_ethertype(EtherType::IPv4);

TCPHeader tcp;
tcp.set_src_port(443);
tcp.set_dst_port(12345);
tcp.set_flag_syn(true);  // Set individual flags
tcp.set_flag_ack(true);
tcp.update_computed_fields();

// Serialize to bytes
auto eth_bytes = eth.to_bytes();
auto tcp_bytes = tcp.to_bytes();

// Deserialize from bytes
TCPHeader restored_tcp;
if (restored_tcp.from_bytes(tcp_bytes)) {
    std::cout << "Port: " << restored_tcp.src_port() << std::endl;
    std::cout << "SYN: " << restored_tcp.flag_syn() << std::endl;
}
```

## Examples

### Complete Network Packet

```cpp
// Build a complete TCP/IP packet
EthernetHeader eth;
eth.set_dst_mac(0x001122334455ULL);
eth.set_src_mac(0x665544332211ULL);
eth.set_ethertype(EtherType::IPv4);

IPv4Header ipv4;
ipv4.set_src_ip(0xC0A80101);  // 192.168.1.1
ipv4.set_dst_ip(0xC0A80102);  // 192.168.1.2
ipv4.set_protocol(IPProtocol::TCP);
ipv4.set_ttl(64);

TCPHeader tcp;
tcp.set_src_port(443);
tcp.set_dst_port(12345);
tcp.set_flag_syn(true);
tcp.set_flag_ack(true);

// Serialize all headers
auto packet = eth.to_bytes();
auto ipv4_bytes = ipv4.to_bytes();
auto tcp_bytes = tcp.to_bytes();

packet.insert(packet.end(), ipv4_bytes.begin(), ipv4_bytes.end());
packet.insert(packet.end(), tcp_bytes.begin(), tcp_bytes.end());
```

## Benefits Over Hand-Written Code

### Readability

**HDL Definition:**
```hdl
header TCPHeader {
    src_port: 16;
    dst_port: 16;
    seq_num: 32;
    flag_syn: 1;
    flag_ack: 1;
    checksum: 16 [computed];
}
```

**vs. Hand-Written C++:**
```cpp
class TCPHeader {
    uint16_t src_port_;
    uint16_t dst_port_;
    uint32_t seq_num_;
    uint8_t flags_;  // How to access individual bits?
    uint16_t checksum_;
    
    // Tons of bit manipulation code...
    bool get_syn_flag() const { return (flags_ >> 1) & 1; }
    void set_syn_flag(bool val) { /* complex bit math */ }
    // etc...
};
```

### Maintainability

- **Single Source of Truth**: HDL file serves as both documentation and implementation
- **Protocol Changes**: Modify HDL file, recompile, done
- **No Manual Bit Math**: Compiler handles all bit manipulation
- **Type Safety**: Enum usage prevents invalid values
- **Validation**: Built-in field validation

### Performance

- **Optimized Code**: Generated code uses efficient bit manipulation
- **No Runtime Overhead**: All field access is compile-time optimized
- **Memory Efficient**: Packed binary representation
- **Cache Friendly**: Contiguous memory layout

## Running the Demo

```bash
# Compile HDL to C++
python3 tools/hdl_compiler.py examples/network_protocols.hdl -o include/generated_headers.h

# Compile and run demo
g++ -I include -std=c++17 examples/generated_headers_demo.cpp -o demo
./demo
```

## File Organization

```
tools/
├── hdl_compiler.py          # HDL compiler implementation
examples/
├── network_protocols.hdl    # Example HDL definitions
├── generated_headers_demo.cpp  # Demo using generated headers
include/
├── generated_headers.h      # Generated C++ headers
```

## Language Reference

See [docs/HEADER_DSL.md](docs/HEADER_DSL.md) for complete language specification.

## Future Enhancements

- **Conditional Fields**: Fields present based on other field values
- **Variable Length Fields**: Dynamic field sizes
- **Union Support**: Discriminated unions for protocol variants
- **Advanced Validation**: Complex field validation rules
- **Optimization**: SIMD operations for checksum calculation
- **Debugging**: Debug output and field visualization

## Contributing

When adding new features:

1. Update the HDL parser in `tools/hdl_compiler.py`
2. Update the C++ code generator
3. Add test cases in HDL files
4. Update documentation
5. Test with existing protocols

## License

This HDL compiler is part of the cppscapy project.
