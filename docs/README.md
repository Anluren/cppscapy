# CppScapy Documentation

Welcome to the CppScapy documentation! This C++ library provides a modern, type-safe way to construct and manipulate network packets with compile-time safety and performance optimization.

## 📚 Documentation Index

### 📖 Core Documentation
- **[README.md](../README.md)** - Project overview, installation, and basic usage
- **[API_REFERENCE.md](../API_REFERENCE.md)** - Complete API documentation  
- **[COMPILE_TIME_LENGTH_DEDUCTION.md](../COMPILE_TIME_LENGTH_DEDUCTION.md)** - Advanced compile-time features

### 🌐 Network Protocol References
- **[NETWORK_HEADERS.md](NETWORK_HEADERS.md)** - Comprehensive network header format reference
- **[HEADERS_QUICK_REF.md](HEADERS_QUICK_REF.md)** - Quick reference for common headers

## 🚀 Quick Start

### Basic Packet Creation
```cpp
#include "cppscapy.h"
using namespace cppscapy;

// Create a simple TCP packet
auto packet = PacketBuilder()
    .ethernet(MacAddress("aa:bb:cc:dd:ee:ff"), MacAddress("11:22:33:44:55:66"))
    .ipv4(IPv4Address("192.168.1.1"), IPv4Address("10.0.0.1"))  
    .tcp(80, 8080)
    .payload("HTTP GET request")
    .build();
```

### Advanced Features
```cpp
// Use compile-time length deduction
auto mac = make_mac_address("aabbccddeeff");
auto ip = make_ipv4_address("c0a80101");

// High-performance in-place randomization
std::vector<uint8_t> buffer(1024);
cppscapy::utils::random::randomize_bytes(buffer);
```

## 📋 Feature Overview

### ✅ Implemented Headers
- **Layer 2**: Ethernet, VLAN (planned)
- **Layer 3**: IPv4, IPv6, ICMP, ARP (planned), MPLS
- **Layer 4**: TCP, UDP
- **Application**: DNS (planned), HTTP (planned), DHCP (planned)

### 🎯 Key Features
- **Type Safety**: Compile-time type checking and validation
- **Performance**: Zero-allocation in-place operations, move semantics optimization
- **Flexibility**: Template-based generic programming with `std::is_invocable`
- **Modern C++**: C++17 features, constexpr evaluation, perfect forwarding
- **Comprehensive**: Full packet analysis, checksum validation, hex utilities

### 🔧 Utilities
- **Random Payload Generation**: 14+ functions for creating test data
- **Hex Conversion**: Compile-time and runtime hex string processing
- **Checksum Calculation**: IPv4, TCP, UDP checksum validation
- **Packet Analysis**: Automatic protocol detection and parsing
- **Memory Management**: Both allocation-based and in-place operations

## 📊 Performance Highlights

- **In-place Randomization**: ~6.8x faster than allocation-based approaches
- **Move Semantics**: Optimized return value handling with RVO
- **Compile-time**: Constant expression evaluation for static data
- **Zero-allocation**: Memory-efficient buffer reuse patterns

## 🎓 Learning Resources

### Examples Directory
```
examples/
├── demo.cpp                     # Basic usage demonstration
├── simple_test.cpp             # Simple packet creation
├── utils_test.cpp              # Utility functions showcase  
├── random_payload_test.cpp     # Random data generation
├── move_semantics_test.cpp     # C++ move semantics examples
├── templated_lambda_test.cpp   # Advanced template programming
├── inplace_randomization_test.cpp # High-performance operations
└── performance_comparison.cpp  # Benchmarking utilities
```

### Study Topics
1. **Network Protocol Basics** - Start with [HEADERS_QUICK_REF.md](HEADERS_QUICK_REF.md)
2. **C++ Template Metaprogramming** - See `templated_lambda_test.cpp`
3. **Performance Optimization** - Check `inplace_randomization_test.cpp`
4. **Type Safety** - Explore compile-time length deduction examples
5. **Memory Management** - Study move semantics examples

## 🛠️ Development Guide

### Building the Library
```bash
mkdir build && cd build
cmake ..
make
```

### Running Tests
```bash
# Run all examples
make && ./demo && ./simple_test && ./utils_test

# Performance testing
./random_payload_test
./performance_comparison
./move_semantics_test
```

### Adding New Headers
1. Define header class in `include/network_headers.h`
2. Add packet builder method in `PacketBuilder`
3. Update header format in [NETWORK_HEADERS.md](NETWORK_HEADERS.md)
4. Add test cases in examples
5. Update API reference documentation

## 🤝 Contributing

### Code Style
- Modern C++17 features preferred
- Comprehensive error handling
- Template-based generic programming
- Performance-conscious design
- Extensive documentation

### Testing
- Create example programs for new features
- Include both positive and negative test cases
- Benchmark performance-critical code
- Validate against real network traffic

## 📞 Support

- **Issues**: Report bugs or request features via GitHub issues
- **Examples**: Check the `examples/` directory for usage patterns
- **Documentation**: Comprehensive references in `docs/` directory
- **Performance**: Profiling results and optimization guides included

---

## 📈 Project Roadmap

### Current Status (v1.0)
- ✅ Core packet building framework
- ✅ Essential L2-L4 headers (Ethernet, IPv4/6, TCP/UDP, ICMP, MPLS)
- ✅ Advanced C++ features (templates, constexpr, move semantics)
- ✅ High-performance in-place operations
- ✅ Comprehensive utility functions

### Planned Features (v1.1)
- 🔄 Additional headers (DNS, HTTP, DHCP, VLAN)
- 🔄 Packet capture integration
- 🔄 Advanced parsing capabilities
- 🔄 More protocol validation
- 🔄 Extended performance optimizations

### Future Vision (v2.0)
- 🎯 Plugin architecture for custom protocols
- 🎯 Real-time packet processing
- 🎯 Network simulation capabilities
- 🎯 Integration with popular network tools
- 🎯 Cross-platform packet injection

---

*CppScapy: Modern C++ Network Packet Construction Library*
*Documentation last updated: July 2025*
