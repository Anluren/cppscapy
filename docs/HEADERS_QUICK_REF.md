# Network Headers Quick Reference

A concise reference for the most commonly encountered network headers.

## Essential Headers Summary

| Header | Layer | Size | Key Fields | Purpose |
|--------|-------|------|------------|---------|
| **Ethernet** | L2 | 14 bytes | Dst MAC, Src MAC, EtherType | Frame delivery |
| **IPv4** | L3 | 20-60 bytes | Src IP, Dst IP, Protocol, TTL | Packet routing |
| **IPv6** | L3 | 40 bytes | Src IP, Dst IP, Next Header, Hop Limit | IPv6 routing |
| **TCP** | L4 | 20-60 bytes | Src Port, Dst Port, Seq, Ack, Flags | Reliable transport |
| **UDP** | L4 | 8 bytes | Src Port, Dst Port, Length | Fast transport |
| **ICMP** | L3 | 8+ bytes | Type, Code, Checksum | Network diagnostics |

## Protocol Numbers & EtherTypes

### IPv4 Protocol Numbers
```cpp
const uint8_t PROTOCOL_ICMP = 1;
const uint8_t PROTOCOL_TCP = 6;
const uint8_t PROTOCOL_UDP = 17;
const uint8_t PROTOCOL_IPV6 = 41;
const uint8_t PROTOCOL_OSPF = 89;
```

### EtherTypes
```cpp
const uint16_t ETHERTYPE_IPV4 = 0x0800;
const uint16_t ETHERTYPE_ARP = 0x0806;
const uint16_t ETHERTYPE_VLAN = 0x8100;
const uint16_t ETHERTYPE_IPV6 = 0x86DD;
const uint16_t ETHERTYPE_MPLS = 0x8847;
```

### Well-Known Ports
```cpp
const uint16_t PORT_HTTP = 80;
const uint16_t PORT_HTTPS = 443;
const uint16_t PORT_SSH = 22;
const uint16_t PORT_DNS = 53;
const uint16_t PORT_DHCP_SERVER = 67;
const uint16_t PORT_DHCP_CLIENT = 68;
```

## Header Field Bit Layouts

### TCP Flags (9 bits)
```
Bit:  8 7 6 5 4 3 2 1 0
Flag: N C E U A P R S F
      S W R R C S S Y I
      |   |   | H T N N
      |   ECN | | | | |
      Nonce   | | | | +-- FIN
              | | | +---- SYN  
              | | +------ RST
              | +-------- PSH
              +---------- ACK
```

### IPv4 Flags (3 bits)
```
Bit: 2 1 0
     | | +-- Reserved (0)
     | +---- Don't Fragment (DF)
     +------ More Fragments (MF)
```

## Common Header Combinations

### Web Traffic
```
Ethernet + IPv4 + TCP + HTTP
14 + 20 + 20 + variable bytes
```

### DNS Query
```
Ethernet + IPv4 + UDP + DNS
14 + 20 + 8 + 12+ bytes
```

### Ping
```
Ethernet + IPv4 + ICMP
14 + 20 + 8+ bytes
```

### VLAN Tagged
```
Ethernet + VLAN + IPv4 + TCP
14 + 4 + 20 + 20 bytes
```

### MPLS VPN
```
Ethernet + MPLS + IPv4 + UDP
14 + 4+ + 20 + 8 bytes
```

## Checksum Calculations

### IPv4 Header Checksum
```cpp
uint16_t calculate_ipv4_checksum(const uint8_t* header, size_t len) {
    uint32_t sum = 0;
    
    // Sum 16-bit words (skip checksum field)
    for (size_t i = 0; i < len; i += 2) {
        if (i == 10) continue; // Skip checksum field
        sum += (header[i] << 8) + header[i+1];
    }
    
    // Fold carry bits
    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }
    
    return ~sum; // One's complement
}
```

### TCP/UDP Pseudo-Header Checksum
```cpp
// Include: Src IP (4) + Dst IP (4) + Zero (1) + Protocol (1) + Length (2)
// Then: TCP/UDP header + payload
```

## Bit Manipulation Macros

```cpp
// Extract fields from network byte order
#define GET_IPV4_VERSION(data)    (((data)[0] >> 4) & 0x0F)
#define GET_IPV4_IHL(data)        ((data)[0] & 0x0F)  
#define GET_IPV4_PROTOCOL(data)   ((data)[9])
#define GET_TCP_FLAGS(data)       ((data)[13])
#define GET_UDP_LENGTH(data)      (((data)[4] << 8) | (data)[5])

// Set fields in network byte order  
#define SET_IPV4_VERSION_IHL(data, ver, ihl) \
    ((data)[0] = ((ver) << 4) | ((ihl) & 0x0F))
```

## Validation Checklist

### IPv4 Header
- [ ] Version == 4
- [ ] IHL >= 5 (minimum 20 bytes)
- [ ] Total Length >= IHL * 4
- [ ] TTL > 0
- [ ] Checksum valid

### TCP Header  
- [ ] Data Offset >= 5 (minimum 20 bytes)
- [ ] Valid flag combinations
- [ ] Window size reasonable
- [ ] Checksum valid

### UDP Header
- [ ] Length >= 8 (minimum header size)
- [ ] Length matches actual packet
- [ ] Checksum valid (if non-zero)

## Performance Tips

1. **Use bit fields for flags**:
```cpp
struct TCPFlags {
    uint8_t fin : 1;
    uint8_t syn : 1; 
    uint8_t rst : 1;
    uint8_t psh : 1;
    uint8_t ack : 1;
    uint8_t urg : 1;
    uint8_t ece : 1;
    uint8_t cwr : 1;
    uint8_t ns  : 1;
};
```

2. **Pack structures**:
```cpp
#pragma pack(push, 1)
struct IPv4Header {
    uint8_t version_ihl;
    uint8_t tos;
    uint16_t total_length;
    // ...
};
#pragma pack(pop)
```

3. **Use constexpr for constants**:
```cpp
constexpr uint16_t ETHERTYPE_IPV4 = 0x0800;
constexpr uint8_t PROTOCOL_TCP = 6;
```

4. **Efficient byte order conversion**:
```cpp
// Use compiler intrinsics when available
#ifdef __GNUC__
    #define SWAP16(x) __builtin_bswap16(x)
    #define SWAP32(x) __builtin_bswap32(x)
#endif
```

---

*Quick reference for network protocol implementation*
