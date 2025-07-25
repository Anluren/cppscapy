# Network Header DSL Specification

## Header Definition Language (HDL)

A domain-specific language for defining network protocol headers with precise bit-field specifications.

## Syntax Overview

```hdl
header HeaderName {
    field_name: width [attributes];
    ...
}
```

## Basic Examples

### Simple Headers

```hdl
header EthernetHeader {
    dst_mac: 48;
    src_mac: 48;
    ethertype: 16;
}

header IPv4Header {
    version: 4 = 4;           // Default value
    ihl: 4 = 5;               // Internet Header Length
    tos: 8;                   // Type of Service
    total_length: 16;
    identification: 16;
    flags: 3;
    fragment_offset: 13;
    ttl: 8 = 64;              // Default TTL
    protocol: 8;
    header_checksum: 16 [computed];
    src_ip: 32;
    dst_ip: 32;
    options: 0..320 [optional]; // Variable length
}

header UDPHeader {
    src_port: 16;
    dst_port: 16;
    length: 16 [computed];    // Auto-calculated
    checksum: 16 [computed];
}
```

### Complex Headers with Enums

```hdl
enum EtherType : 16 {
    IPv4 = 0x0800,
    IPv6 = 0x86DD,
    ARP = 0x0806,
    MPLS = 0x8847,
    VLAN = 0x8100
}

enum IPProtocol : 8 {
    ICMP = 1,
    TCP = 6,
    UDP = 17,
    OSPF = 89
}

header EthernetHeader {
    dst_mac: 48;
    src_mac: 48;
    ethertype: EtherType;
}

header IPv4Header {
    version: 4 = 4;
    ihl: 4 = 5;
    tos: 8;
    total_length: 16;
    identification: 16;
    reserved: 1 = 0;
    dont_fragment: 1;
    more_fragments: 1;
    fragment_offset: 13;
    ttl: 8 = 64;
    protocol: IPProtocol;
    header_checksum: 16 [computed];
    src_ip: 32;
    dst_ip: 32;
    options: 0..320 [optional, align=32];
}
```

### Headers with Conditional Fields

```hdl
header TCPHeader {
    src_port: 16;
    dst_port: 16;
    seq_num: 32;
    ack_num: 32;
    data_offset: 4 = 5;
    reserved: 3 = 0;
    flags: 9 {
        ns: 1;      // ECN-nonce
        cwr: 1;     // Congestion Window Reduced
        ece: 1;     // ECN-Echo
        urg: 1;     // Urgent
        ack: 1;     // Acknowledgment
        psh: 1;     // Push
        rst: 1;     // Reset
        syn: 1;     // Synchronize
        fin: 1;     // Finish
    };
    window_size: 16;
    checksum: 16 [computed];
    urgent_ptr: 16;
    options: 0..320 [optional, present_if=data_offset > 5];
}
```

### Advanced Features

```hdl
// MPLS with stacking support
header MPLSHeader {
    label: 20;
    traffic_class: 3;
    bottom_of_stack: 1;
    ttl: 8;
} [stackable];

// VLAN with multiple tag support
header VLANHeader {
    tpid: 16 = 0x8100;
    priority: 3;
    cfi: 1;
    vlan_id: 12;
} [stackable, max_stack=2];

// Variable length header
header DNSHeader {
    id: 16;
    flags: 16 {
        qr: 1;          // Query/Response
        opcode: 4;      // Operation code
        aa: 1;          // Authoritative Answer
        tc: 1;          // Truncated
        rd: 1;          // Recursion Desired
        ra: 1;          // Recursion Available
        z: 3 = 0;       // Reserved
        rcode: 4;       // Response code
    };
    qdcount: 16;        // Question count
    ancount: 16;        // Answer count
    nscount: 16;        // Authority count
    arcount: 16;        // Additional count
    
    // Variable sections
    questions: DNSQuestion[qdcount];
    answers: DNSResourceRecord[ancount];
    authority: DNSResourceRecord[nscount];
    additional: DNSResourceRecord[arcount];
}

// Complex nested structure
header IPv6Header {
    version: 4 = 6;
    traffic_class: 8;
    flow_label: 20;
    payload_length: 16;
    next_header: 8;
    hop_limit: 8;
    src_addr: 128;
    dst_addr: 128;
}

// Header with unions for different interpretations
header ICMPHeader {
    type: 8;
    code: 8;
    checksum: 16 [computed];
    
    // Different formats based on type
    union payload [discriminant=type] {
        echo [type in {0, 8}]: {
            identifier: 16;
            sequence: 16;
            data: variable;
        };
        
        dest_unreachable [type == 3]: {
            unused: 32 = 0;
            original_header: variable;
        };
        
        redirect [type == 5]: {
            gateway_addr: 32;
            original_header: variable;
        };
    };
}
```

## Language Features

### Data Types
- **Integers**: Specified by bit width (1-64 bits)
- **Enums**: Named constants with specified underlying type
- **Arrays**: Fixed or variable length sequences
- **Unions**: Discriminated unions based on field values
- **Strings**: Length-prefixed or null-terminated

### Attributes
- `[computed]`: Field calculated automatically (checksums, lengths)
- `[optional]`: Field may or may not be present
- `[present_if=condition]`: Conditional field presence
- `[align=bits]`: Alignment requirement
- `[stackable]`: Header can be stacked multiple times
- `[max_stack=n]`: Maximum stack depth
- `[network_order]`: Big-endian byte order (default)
- `[host_order]`: Host byte order
- `[validate=expression]`: Field validation rule

### Expressions
- Arithmetic: `+`, `-`, `*`, `/`, `%`
- Comparison: `==`, `!=`, `<`, `>`, `<=`, `>=`
- Logical: `&&`, `||`, `!`
- Bitwise: `&`, `|`, `^`, `<<`, `>>`
- Field references: `field_name`, `header.field_name`

### Built-in Functions
- `sizeof(type)`: Size in bits
- `checksum(data, algorithm)`: Calculate checksum
- `crc(data, polynomial)`: Calculate CRC
- `length(array)`: Array length
- `sum(array)`: Array sum

## Generated Code Interface

```cpp
// Generated C++ class interface
class HeaderName {
public:
    // Constructors
    HeaderName();
    HeaderName(const std::vector<uint8_t>& data);
    
    // Serialization
    std::vector<uint8_t> to_bytes() const;
    bool from_bytes(const std::vector<uint8_t>& data);
    
    // Size information
    size_t size_bits() const;
    size_t size_bytes() const;
    bool is_variable_size() const;
    
    // Field accessors (type-safe)
    uint32_t field_name() const;
    void set_field_name(uint32_t value);
    
    // Validation
    bool is_valid() const;
    std::vector<std::string> validate() const;
    
    // Computed fields
    void update_computed_fields();
    
    // Optional field management
    bool has_optional_field() const;
    void set_optional_field_present(bool present);
    
    // Stack operations (for stackable headers)
    void push_stack(const HeaderName& header);
    HeaderName pop_stack();
    size_t stack_depth() const;
    
private:
    // Bit-packed storage
    std::vector<uint8_t> data_;
    
    // Optional field presence flags
    std::bitset<N> optional_present_;
    
    // Helper methods
    template<typename T>
    T get_field(size_t bit_offset, size_t bit_width) const;
    
    template<typename T>
    void set_field(size_t bit_offset, size_t bit_width, T value);
};
```

## Compiler Features

### Code Generation
- Generate type-safe C++ classes
- Automatic bit packing/unpacking
- Computed field calculations
- Validation functions
- Serialization/deserialization

### Optimizations
- Compile-time size calculations
- Efficient bit manipulation
- Memory layout optimization
- SIMD operations for checksums

### Static Analysis
- Field overlap detection
- Size validation
- Dependency analysis
- Dead field elimination

## Usage Examples

```cpp
// Using generated headers
EthernetHeader eth;
eth.set_dst_mac(0x001122334455ULL);
eth.set_src_mac(0x665544332211ULL);
eth.set_ethertype(EtherType::IPv4);

IPv4Header ipv4;
ipv4.set_src_ip(0xC0A80101);  // 192.168.1.1
ipv4.set_dst_ip(0xC0A80102);  // 192.168.1.2
ipv4.set_protocol(IPProtocol::UDP);
ipv4.set_total_length(sizeof(IPv4Header) + sizeof(UDPHeader) + payload_size);
ipv4.update_computed_fields();  // Calculate checksum

UDPHeader udp;
udp.set_src_port(12345);
udp.set_dst_port(80);
udp.set_length(sizeof(UDPHeader) + payload_size);
udp.update_computed_fields();   // Calculate checksum

// Build packet
PacketBuilder builder;
auto packet = builder
    .add_header(eth)
    .add_header(ipv4)
    .add_header(udp)
    .add_payload(payload_data)
    .build();
```

## Advanced DSL Examples

```hdl
// Protocol with versioning
header MyProtocolHeader [version_field=version] {
    version: 4;
    
    union payload [discriminant=version] {
        v1 [version == 1]: {
            field_a: 16;
            field_b: 16;
        };
        
        v2 [version == 2]: {
            field_a: 16;
            field_b: 16;
            field_c: 32;
            options: 0..128 [optional];
        };
    };
}

// Header with complex validation
header CustomHeader {
    type: 8 [validate=type in {1, 2, 3}];
    length: 16 [validate=length >= 12 && length <= 1500];
    flags: 8;
    payload: (length - 4) * 8;  // Dynamic size based on length field
}

// Protocol with extension headers
header ExtensibleHeader {
    base_type: 8;
    length: 8;
    flags: 8;
    reserved: 8 = 0;
    
    extensions: Extension[flags.extension_count] [optional, present_if=flags.has_extensions];
}

header Extension {
    type: 8;
    length: 8;
    data: (length - 2) * 8;
}
```

This DSL would provide:

1. **Type Safety**: Compile-time checking of field sizes and types
2. **Automatic Code Generation**: Generate efficient C++ classes
3. **Bit-level Precision**: Exact control over wire format
4. **Validation**: Built-in field validation and constraints  
5. **Extensibility**: Support for optional fields, unions, and stacking
6. **Performance**: Optimized serialization and bit manipulation
7. **Documentation**: Self-documenting header definitions

The DSL would compile to efficient C++ code that handles all the low-level bit manipulation while providing a clean, type-safe interface for network protocol implementation.
