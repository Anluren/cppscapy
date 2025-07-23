# Network Headers Reference Guide

A comprehensive reference for common network protocol headers, their formats, and field descriptions.

## Table of Contents

- [Layer 2 - Data Link Layer](#layer-2---data-link-layer)
- [Layer 3 - Network Layer](#layer-3---network-layer)  
- [Layer 4 - Transport Layer](#layer-4---transport-layer)
- [Application Layer Protocols](#application-layer-protocols)
- [Security & VPN Headers](#security--vpn-headers)
- [Cloud & Virtualization](#cloud--virtualization)
- [Header Field Types](#header-field-types)
- [Implementation Notes](#implementation-notes)

---

## Layer 2 - Data Link Layer

### Ethernet Header (IEEE 802.3)
```
 0                   1                   2                   3
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                    Destination MAC Address                    |
|                               +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                               |                               |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+                               +
|                      Source MAC Address                       |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|           EtherType           |           Payload...          |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
```

**Fields:**
- **Destination MAC** (48 bits): Target hardware address
- **Source MAC** (48 bits): Sender hardware address  
- **EtherType** (16 bits): Next layer protocol identifier
  - `0x0800`: IPv4
  - `0x86DD`: IPv6
  - `0x0806`: ARP
  - `0x8100`: 802.1Q VLAN
  - `0x8847`: MPLS Unicast
  - `0x8848`: MPLS Multicast

**Total Size:** 14 bytes (without payload)

### 802.1Q VLAN Header
```
 0                   1                   2                   3
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|            TPID (0x8100)      | PRI |C|        VLAN ID        |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|           EtherType           |           Payload...          |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
```

**Fields:**
- **TPID** (16 bits): Tag Protocol Identifier (0x8100)
- **PRI** (3 bits): Priority Code Point (0-7)
- **CFI** (1 bit): Canonical Format Indicator  
- **VLAN ID** (12 bits): VLAN identifier (0-4095)
- **EtherType** (16 bits): Next protocol

**Total Size:** 4 bytes (inserted after Ethernet header)

---

## Layer 3 - Network Layer

### IPv4 Header (RFC 791)
```
 0                   1                   2                   3
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|Version|  IHL  |Type of Service|          Total Length         |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|         Identification        |Flags|      Fragment Offset    |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|  Time to Live |    Protocol   |         Header Checksum       |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                       Source Address                          |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                    Destination Address                        |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                    Options                    |    Padding    |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
```

**Fields:**
- **Version** (4 bits): IP version (4)
- **IHL** (4 bits): Internet Header Length in 32-bit words (5-15)
- **ToS/DSCP** (8 bits): Type of Service / Differentiated Services
- **Total Length** (16 bits): Total packet length in bytes
- **Identification** (16 bits): Fragment identification
- **Flags** (3 bits): Control flags
  - Bit 0: Reserved (0)
  - Bit 1: Don't Fragment (DF)
  - Bit 2: More Fragments (MF)
- **Fragment Offset** (13 bits): Fragment position
- **TTL** (8 bits): Time To Live (hop count)
- **Protocol** (8 bits): Next layer protocol
  - `1`: ICMP
  - `6`: TCP  
  - `17`: UDP
  - `89`: OSPF
- **Header Checksum** (16 bits): Header integrity check
- **Source Address** (32 bits): Source IP address
- **Destination Address** (32 bits): Destination IP address
- **Options** (0-40 bytes): Optional fields

**Total Size:** 20-60 bytes

### IPv6 Header (RFC 8200)
```
 0                   1                   2                   3
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|Version| Traffic Class |           Flow Label                  |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|         Payload Length        |  Next Header  |   Hop Limit   |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                                                               |
+                                                               +
|                                                               |
+                         Source Address                        +
|                                                               |
+                                                               +
|                                                               |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                                                               |
+                                                               +
|                                                               |
+                      Destination Address                      +
|                                                               |
+                                                               +
|                                                               |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
```

**Fields:**
- **Version** (4 bits): IP version (6)
- **Traffic Class** (8 bits): Traffic priority and QoS
- **Flow Label** (20 bits): Flow identification
- **Payload Length** (16 bits): Payload size in bytes
- **Next Header** (8 bits): Next protocol type
  - `6`: TCP
  - `17`: UDP
  - `58`: ICMPv6
  - `41`: IPv6-in-IPv4 tunnel
- **Hop Limit** (8 bits): Maximum hops (like TTL)
- **Source Address** (128 bits): Source IPv6 address
- **Destination Address** (128 bits): Destination IPv6 address

**Total Size:** 40 bytes (fixed)

### ICMP Header (RFC 792)
```
 0                   1                   2                   3
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|     Type      |     Code      |          Checksum             |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                             Data                              |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
```

**Fields:**
- **Type** (8 bits): ICMP message type
  - `0`: Echo Reply
  - `3`: Destination Unreachable
  - `8`: Echo Request
  - `11`: Time Exceeded
- **Code** (8 bits): Sub-type within message type
- **Checksum** (16 bits): Header and data checksum
- **Data** (variable): Type-specific data

**Common Types:**
- **Echo Request/Reply**: ID (16 bits) + Sequence (16 bits) + Data
- **Destination Unreachable**: Unused (32 bits) + Original IP header + 8 bytes

**Total Size:** 8+ bytes

### ARP Header (RFC 826)
```
 0                   1                   2                   3
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|         Hardware Type         |         Protocol Type         |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|  HW Addr Len | Proto Addr Len|          Operation            |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                    Sender Hardware Address                    |
|                               +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                               |     Sender Protocol Address   |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
| Sender Proto  |                Target Hardware Address        |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                Target HW Addr (cont.)                        |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                    Target Protocol Address                    |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
```

**Fields:**
- **Hardware Type** (16 bits): Link layer type (1 = Ethernet)
- **Protocol Type** (16 bits): Network layer type (0x0800 = IPv4)
- **Hardware Address Length** (8 bits): MAC address length (6)
- **Protocol Address Length** (8 bits): IP address length (4)
- **Operation** (16 bits): Request (1) or Reply (2)
- **Sender Hardware Address** (48 bits): Sender MAC
- **Sender Protocol Address** (32 bits): Sender IP
- **Target Hardware Address** (48 bits): Target MAC
- **Target Protocol Address** (32 bits): Target IP

**Total Size:** 28 bytes (for Ethernet/IPv4)

### MPLS Header (RFC 3032)
```
 0                   1                   2                   3
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                Label                  | TC  |S|       TTL     |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
```

**Fields:**
- **Label** (20 bits): MPLS label value
- **TC** (3 bits): Traffic Class (QoS)
- **S** (1 bit): Bottom of Stack indicator
- **TTL** (8 bits): Time To Live

**Total Size:** 4 bytes (can be stacked)

---

## Layer 4 - Transport Layer

### TCP Header (RFC 793)
```
 0                   1                   2                   3
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|          Source Port          |       Destination Port        |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                        Sequence Number                        |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                    Acknowledgment Number                      |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|  Data |           |U|A|P|R|S|F|                               |
| Offset| Reserved  |R|C|S|S|Y|I|            Window             |
|       |           |G|K|H|T|N|N|                               |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|           Checksum            |         Urgent Pointer        |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                    Options                    |    Padding    |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
```

**Fields:**
- **Source Port** (16 bits): Source application port
- **Destination Port** (16 bits): Destination application port
- **Sequence Number** (32 bits): Stream position
- **Acknowledgment Number** (32 bits): Next expected sequence
- **Data Offset** (4 bits): Header length in 32-bit words
- **Reserved** (3 bits): Must be zero
- **Flags** (9 bits): Control flags
  - **URG**: Urgent pointer valid
  - **ACK**: Acknowledgment valid
  - **PSH**: Push function
  - **RST**: Reset connection
  - **SYN**: Synchronize sequence numbers
  - **FIN**: Finish sending
- **Window** (16 bits): Flow control window size
- **Checksum** (16 bits): Header and data checksum
- **Urgent Pointer** (16 bits): Urgent data offset
- **Options** (0-40 bytes): TCP options

**Total Size:** 20-60 bytes

### UDP Header (RFC 768)
```
 0                   1                   2                   3
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|          Source Port          |       Destination Port        |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|            Length             |           Checksum            |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
```

**Fields:**
- **Source Port** (16 bits): Source application port
- **Destination Port** (16 bits): Destination application port  
- **Length** (16 bits): UDP header + payload length
- **Checksum** (16 bits): Header and data checksum (optional in IPv4)

**Total Size:** 8 bytes

---

## Application Layer Protocols

### DNS Header (RFC 1035)
```
 0                   1                   2                   3
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                              ID                               |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|QR|   Opcode  |AA|TC|RD|RA|   Z    |   RCODE   |               |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                    QDCOUNT                    |               |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                    ANCOUNT                    |               |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                    NSCOUNT                    |               |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                    ARCOUNT                    |               |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
```

**Fields:**
- **ID** (16 bits): Query identifier
- **QR** (1 bit): Query (0) or Response (1)
- **Opcode** (4 bits): Query type
- **AA** (1 bit): Authoritative Answer
- **TC** (1 bit): Truncated
- **RD** (1 bit): Recursion Desired
- **RA** (1 bit): Recursion Available
- **Z** (3 bits): Reserved (must be 0)
- **RCODE** (4 bits): Response code
- **QDCOUNT** (16 bits): Number of questions
- **ANCOUNT** (16 bits): Number of answers
- **NSCOUNT** (16 bits): Number of authority records
- **ARCOUNT** (16 bits): Number of additional records

**Total Size:** 12 bytes + questions + answers + authority + additional

### HTTP Request Header
```
Method SP Request-URI SP HTTP-Version CRLF
*(( general-header | request-header | entity-header ) CRLF)
CRLF
[ message-body ]
```

**Example:**
```
GET /index.html HTTP/1.1\r\n
Host: www.example.com\r\n  
User-Agent: Mozilla/5.0\r\n
Accept: text/html,application/xhtml+xml\r\n
Accept-Language: en-US,en;q=0.5\r\n
Accept-Encoding: gzip, deflate\r\n
Connection: keep-alive\r\n
\r\n
```

**Common Headers:**
- **Host**: Target server hostname
- **User-Agent**: Client software identification
- **Accept**: Acceptable content types
- **Accept-Language**: Preferred languages
- **Accept-Encoding**: Acceptable encodings
- **Connection**: Connection management
- **Authorization**: Authentication credentials
- **Content-Type**: Request body media type
- **Content-Length**: Request body size

### DHCP Header (RFC 2131)
```
 0                   1                   2                   3
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|     op (1)    |   htype (1)   |   hlen (1)    |   hops (1)    |
+---------------+---------------+---------------+---------------+
|                            xid (4)                            |
+-------------------------------+-------------------------------+
|           secs (2)            |           flags (2)           |
+-------------------------------+-------------------------------+
|                          ciaddr  (4)                         |
+---------------------------------------------------------------+
|                          yiaddr  (4)                         |
+---------------------------------------------------------------+
|                          siaddr  (4)                         |
+---------------------------------------------------------------+
|                          giaddr  (4)                         |
+---------------------------------------------------------------+
|                                                               |
|                          chaddr  (16)                        |
|                                                               |
|                                                               |
+---------------------------------------------------------------+
|                                                               |
|                          sname   (64)                        |
+---------------------------------------------------------------+
|                                                               |
|                          file    (128)                       |
+---------------------------------------------------------------+
|                                                               |
|                          options (variable)                  |
+---------------------------------------------------------------+
```

**Fields:**
- **op** (8 bits): Message type (1=request, 2=reply)
- **htype** (8 bits): Hardware address type
- **hlen** (8 bits): Hardware address length
- **hops** (8 bits): Hop count
- **xid** (32 bits): Transaction ID
- **secs** (16 bits): Seconds since process began
- **flags** (16 bits): Flags (broadcast, etc.)
- **ciaddr** (32 bits): Client IP address
- **yiaddr** (32 bits): Your IP address
- **siaddr** (32 bits): Server IP address
- **giaddr** (32 bits): Gateway IP address
- **chaddr** (128 bits): Client hardware address
- **sname** (512 bits): Server hostname
- **file** (1024 bits): Boot filename
- **options** (variable): DHCP options

**Total Size:** 236+ bytes

---

## Security & VPN Headers

### IPSec ESP Header (RFC 4303)
```
 0                   1                   2                   3
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|               Security Parameters Index (SPI)                |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                      Sequence Number                         |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                    Payload Data* (variable)                  |
~                                                               ~
|                                                               |
+               +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|               |     Padding (0-255 bytes)                    |
+-+-+-+-+-+-+-+-+               +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                               |  Pad Length   | Next Header   |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|         Integrity Check Value-ICV   (variable)               |
~                                                               ~
|                                                               |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
```

**Fields:**
- **SPI** (32 bits): Security Parameters Index
- **Sequence Number** (32 bits): Anti-replay sequence
- **Payload Data** (variable): Encrypted data
- **Padding** (0-255 bytes): Encryption alignment
- **Pad Length** (8 bits): Number of padding bytes
- **Next Header** (8 bits): Next protocol type
- **ICV** (variable): Integrity Check Value

### GRE Header (RFC 2784)
```
 0                   1                   2                   3
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|C|       Reserved0       | Ver |         Protocol Type         |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|      Checksum (optional)      |       Reserved1 (Optional)    |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
```

**Fields:**
- **C** (1 bit): Checksum present
- **Reserved0** (12 bits): Must be zero
- **Ver** (3 bits): Version (must be 0)
- **Protocol Type** (16 bits): Encapsulated protocol
- **Checksum** (16 bits): Optional checksum
- **Reserved1** (16 bits): Must be zero

**Total Size:** 4-8 bytes

---

## Cloud & Virtualization

### VXLAN Header (RFC 7348)
```
 0                   1                   2                   3
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|R|R|R|R|I|R|R|R|               Reserved                        |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                VXLAN Network Identifier (VNI)|   Reserved    |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
```

**Fields:**
- **Flags** (8 bits): Control flags (I bit = VNI valid)
- **Reserved** (24 bits): Must be zero
- **VNI** (24 bits): VXLAN Network Identifier
- **Reserved** (8 bits): Must be zero

**Total Size:** 8 bytes

### GENEVE Header (RFC 8926)
```
 0                   1                   2                   3
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|Ver|  Opt Len  |O|C|    Rsvd.  |          Protocol Type        |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|        Virtual Network Identifier (VNI)      |    Reserved   |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                    Variable Length Options                    |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
```

**Fields:**
- **Ver** (2 bits): Version (0)
- **Opt Len** (6 bits): Options length in 4-byte units
- **O** (1 bit): OAM packet
- **C** (1 bit): Critical options present
- **Rsvd** (6 bits): Reserved
- **Protocol Type** (16 bits): Inner protocol
- **VNI** (24 bits): Virtual Network Identifier
- **Reserved** (8 bits): Must be zero
- **Options** (variable): TLV options

**Total Size:** 8+ bytes

---

## Header Field Types

### Common Field Types
- **MAC Address**: 48-bit (6 bytes) hardware identifier
- **IPv4 Address**: 32-bit (4 bytes) network address
- **IPv6 Address**: 128-bit (16 bytes) network address
- **Port Number**: 16-bit application identifier (0-65535)
- **Protocol Number**: 8-bit next header identifier
- **Checksum**: 16-bit error detection code
- **Length**: 16-bit byte count
- **Flags**: Single bits for control options
- **Reserved**: Fields set to zero for future use

### Byte Ordering
- **Network Byte Order**: Big-endian (most significant byte first)
- **Host Byte Order**: Platform dependent (Intel x86 = little-endian)
- **Conversion Functions**: `htons()`, `ntohs()`, `htonl()`, `ntohl()`

### Alignment
- Most headers align fields on natural boundaries
- Some protocols pad to 32-bit or 64-bit boundaries
- Options often padded to maintain alignment

---

## Implementation Notes

### C++ Header Classes
Each header should implement:
```cpp
class HeaderType {
public:
    // Constructors
    HeaderType();
    HeaderType(const std::vector<uint8_t>& data);
    
    // Serialization
    std::vector<uint8_t> to_bytes() const;
    void from_bytes(const std::vector<uint8_t>& data);
    
    // Field accessors
    uint16_t field_name() const;
    void set_field_name(uint16_t value);
    
    // Utility
    size_t size() const;
    bool is_valid() const;
    
private:
    // Header fields as member variables
};
```

### Common Patterns
1. **Fixed Size Headers**: IPv4, IPv6, UDP, ICMP
2. **Variable Size Headers**: TCP (options), IPv4 (options) 
3. **Stacked Headers**: MPLS, VLAN
4. **Tunneling Headers**: GRE, IPSec, VXLAN

### Validation
- Check minimum/maximum field values
- Verify reserved fields are zero
- Validate checksums when present
- Ensure proper field relationships

### Performance Tips
- Use bit fields for flag fields
- Pack structures to avoid padding
- Implement efficient checksum algorithms
- Cache computed values when possible

---

## References

- [RFC 791 - Internet Protocol](https://tools.ietf.org/html/rfc791)
- [RFC 793 - Transmission Control Protocol](https://tools.ietf.org/html/rfc793)  
- [RFC 768 - User Datagram Protocol](https://tools.ietf.org/html/rfc768)
- [RFC 792 - Internet Control Message Protocol](https://tools.ietf.org/html/rfc792)
- [RFC 826 - Address Resolution Protocol](https://tools.ietf.org/html/rfc826)
- [RFC 8200 - Internet Protocol Version 6](https://tools.ietf.org/html/rfc8200)
- [RFC 3032 - MPLS Label Stack Encoding](https://tools.ietf.org/html/rfc3032)
- [RFC 7348 - VXLAN](https://tools.ietf.org/html/rfc7348)
- [IEEE 802.3 - Ethernet](https://standards.ieee.org/standard/802_3-2018.html)
- [IEEE 802.1Q - VLAN](https://standards.ieee.org/standard/802_1Q-2018.html)

---

*Last Updated: July 2025*
