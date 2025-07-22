#include "network_headers.h"
#include "utils.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>
#include <array>
#include <functional>

using namespace cppscapy;
using namespace cppscapy::utils;

int main() {
    std::cout << "=== Testing Templated Lambda with if constexpr ===\n\n";
    
    // Test 1: Basic type detection lambda
    std::cout << "Test 1: Basic Type Detection\n";
    
    auto detect_type = [](auto&& param) {
        using T = std::decay_t<decltype(param)>;
        using U = std::remove_reference_t<decltype(param)>;
        
        std::cout << "Parameter: ";
        if constexpr (std::is_array_v<U>) {
            using ElementType = std::remove_extent_t<U>;
            if constexpr (std::is_same_v<ElementType, char> || 
                         std::is_same_v<ElementType, const char>) {
                std::cout << "\"" << param << "\" -> const char[" << std::extent_v<U> << "] (string literal)\n";
                return std::string("STRING_LITERAL");
            } else if constexpr (std::is_same_v<ElementType, uint8_t>) {
                std::cout << "byte array[" << std::extent_v<U> << "]\n";
                return std::string("BYTE_ARRAY");
            }
        } else if constexpr (std::is_same_v<T, const char*> || std::is_same_v<T, char*>) {
            std::cout << "\"" << param << "\" -> const char* (C-string pointer)\n";
            return std::string("CSTRING_PTR");
        } else if constexpr (std::is_same_v<T, std::string>) {
            std::cout << "\"" << param << "\" -> std::string\n";
            return std::string("STD_STRING");
        } else if constexpr (std::is_same_v<T, std::string_view>) {
            std::cout << "\"" << param << "\" -> std::string_view\n";
            return std::string("STRING_VIEW");
        } else if constexpr (std::is_arithmetic_v<T>) {
            std::cout << param << " -> " << typeid(T).name() << " (arithmetic)\n";
            return std::string("ARITHMETIC");
        } else {
            std::cout << "unknown type -> " << typeid(T).name() << "\n";
            return std::string("UNKNOWN");
        }
    };
    
    // Test different types
    auto result1 = detect_type("Hello World");           // const char[12]
    auto result2 = detect_type(std::string("Test"));     // std::string
    auto result3 = detect_type(std::string_view("View")); // std::string_view
    const char* cstr = "Pointer";
    auto result4 = detect_type(cstr);                    // const char*
    auto result5 = detect_type(42);                      // int
    auto result6 = detect_type(3.14);                    // double
    
    uint8_t byte_array[] = {0x01, 0x02, 0x03, 0x04};
    auto result7 = detect_type(byte_array);              // uint8_t[4]
    
    std::cout << "\n";
    
    // Test 2: Smart payload processor for network library
    std::cout << "Test 2: Smart Payload Processor\n";
    
    auto smart_payload = [](auto&& payload_data) -> std::vector<uint8_t> {
        using T = std::decay_t<decltype(payload_data)>;
        using U = std::remove_reference_t<decltype(payload_data)>;
        
        std::cout << "Processing payload: ";
        
        if constexpr (std::is_array_v<U>) {
            using ElementType = std::remove_extent_t<U>;
            if constexpr (std::is_same_v<ElementType, char> || 
                         std::is_same_v<ElementType, const char>) {
                // Handle string literals like "hello"
                std::cout << "string literal -> ";
                std::string str(payload_data);
                std::vector<uint8_t> result(str.begin(), str.end());
                std::cout << result.size() << " bytes\n";
                return result;
            } else if constexpr (std::is_same_v<ElementType, uint8_t> || 
                               std::is_same_v<ElementType, const uint8_t>) {
                // Handle byte arrays like uint8_t data[] = {0x01, 0x02}
                std::cout << "byte array -> ";
                constexpr size_t size = std::extent_v<U>;
                std::vector<uint8_t> result(payload_data, payload_data + size);
                std::cout << size << " bytes\n";
                return result;
            }
        } else if constexpr (std::is_same_v<T, std::string>) {
            // Handle std::string
            std::cout << "std::string -> ";
            std::vector<uint8_t> result(payload_data.begin(), payload_data.end());
            std::cout << result.size() << " bytes\n";
            return result;
        } else if constexpr (std::is_same_v<T, std::string_view>) {
            // Handle std::string_view
            std::cout << "std::string_view -> ";
            std::vector<uint8_t> result(payload_data.begin(), payload_data.end());
            std::cout << result.size() << " bytes\n";
            return result;
        } else if constexpr (std::is_same_v<T, std::vector<uint8_t>>) {
            // Handle vector<uint8_t> directly
            std::cout << "std::vector<uint8_t> -> ";
            std::cout << payload_data.size() << " bytes\n";
            return payload_data;
        } else if constexpr (std::is_same_v<T, const char*> || std::is_same_v<T, char*>) {
            // Handle C-string pointers
            std::cout << "C-string pointer -> ";
            std::string str(payload_data);
            std::vector<uint8_t> result(str.begin(), str.end());
            std::cout << result.size() << " bytes\n";
            return result;
        } else if constexpr (std::is_arithmetic_v<T>) {
            // Handle single numeric values
            std::cout << "arithmetic (" << typeid(T).name() << ") -> ";
            const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&payload_data);
            std::vector<uint8_t> result(ptr, ptr + sizeof(T));
            std::cout << sizeof(T) << " bytes\n";
            return result;
        } else {
            // For other types, try to serialize as bytes
            static_assert(std::is_trivially_copyable_v<T>, 
                         "Payload type must be trivially copyable");
            std::cout << "trivial type -> ";
            const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&payload_data);
            std::vector<uint8_t> result(ptr, ptr + sizeof(T));
            std::cout << sizeof(T) << " bytes\n";
            return result;
        }
        
        return {}; // Should never reach here
    };
    
    // Test different payload types
    auto payload1 = smart_payload("Network Packet");        // const char[15]
    auto payload2 = smart_payload(std::string("UDP Data")); // std::string
    auto payload3 = smart_payload(std::string_view("TCP")); // std::string_view
    const char* msg = "ICMP Message";
    auto payload4 = smart_payload(msg);                     // const char*
    auto payload5 = smart_payload(std::vector<uint8_t>{0xDE, 0xAD, 0xBE, 0xEF}); // vector
    auto payload6 = smart_payload(uint32_t(0x12345678));    // uint32_t
    
    uint8_t raw_data[] = {0xCA, 0xFE, 0xBA, 0xBE};
    auto payload7 = smart_payload(raw_data);                // uint8_t[4]
    
    // Print some results
    std::cout << "\nPayload Results:\n";
    std::cout << "payload1 (string literal): ";
    print_hex_ascii(payload1, "");
    
    std::cout << "payload5 (vector): ";
    for (auto byte : payload5) {
        std::cout << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(byte) << " ";
    }
    std::cout << std::dec << "\n";
    
    std::cout << "payload6 (uint32_t): ";
    for (auto byte : payload6) {
        std::cout << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(byte) << " ";
    }
    std::cout << std::dec << "\n\n";
    
    // Test 3: Network header type detection
    std::cout << "Test 3: Network Header Type Detection\n";
    
    auto process_header = [](auto&& header) {
        using T = std::decay_t<decltype(header)>;
        
        std::cout << "Header type: ";
        if constexpr (std::is_same_v<T, EthernetHeader>) {
            std::cout << "EthernetHeader - EtherType: 0x" << std::hex << header.ethertype() << std::dec << "\n";
        } else if constexpr (std::is_same_v<T, IPv4Header>) {
            std::cout << "IPv4Header - Protocol: " << static_cast<int>(header.protocol()) << "\n";
        } else if constexpr (std::is_same_v<T, IPv6Header>) {
            std::cout << "IPv6Header - Next Header: " << static_cast<int>(header.next_header()) << "\n";
        } else if constexpr (std::is_same_v<T, MPLSHeader>) {
            std::cout << "MPLSHeader - Label: " << header.label() << "\n";
        } else if constexpr (std::is_same_v<T, TCPHeader>) {
            std::cout << "TCPHeader - Dst Port: " << header.dst_port() << "\n";
        } else if constexpr (std::is_same_v<T, UDPHeader>) {
            std::cout << "UDPHeader - Dst Port: " << header.dst_port() << "\n";
        } else if constexpr (std::is_same_v<T, ICMPHeader>) {
            std::cout << "ICMPHeader - Type: " << static_cast<int>(header.type()) << "\n";
        } else {
            std::cout << "Unknown header type\n";
        }
    };
    
    // Test with different header types
    EthernetHeader eth(MacAddress("aa:bb:cc:dd:ee:ff"), MacAddress("11:22:33:44:55:66"), EthernetHeader::ETHERTYPE_IPV4);
    IPv4Header ipv4(IPv4Address("192.168.1.1"), IPv4Address("10.0.0.1"), IPv4Header::PROTOCOL_TCP);
    IPv6Header ipv6(IPv6Address("2001:db8::1"), IPv6Address("2001:db8::2"), IPv6Header::NEXT_HEADER_UDP);
    MPLSHeader mpls(1000, 3, true, 64);
    TCPHeader tcp(80, 8080);
    UDPHeader udp(53, 12345);
    ICMPHeader icmp(ICMPHeader::TYPE_ECHO_REQUEST, 0);
    
    process_header(eth);
    process_header(ipv4);
    process_header(ipv6);
    process_header(mpls);
    process_header(tcp);
    process_header(udp);
    process_header(icmp);
    
    // Test 4: Universal packet builder using templated lambda
    std::cout << "\nTest 4: Universal Packet Builder\n";
    
    auto build_packet = [](auto&& first_header, auto&&... additional_headers) {
        PacketBuilder builder;
        
        // Process the first header
        auto add_header = [&builder](auto&& header) {
            using T = std::decay_t<decltype(header)>;
            if constexpr (std::is_same_v<T, EthernetHeader>) {
                builder.ethernet(header);
            } else if constexpr (std::is_same_v<T, IPv4Header>) {
                builder.ipv4(header);
            } else if constexpr (std::is_same_v<T, IPv6Header>) {
                builder.ipv6(header);
            } else if constexpr (std::is_same_v<T, MPLSHeader>) {
                builder.mpls(header);
            } else if constexpr (std::is_same_v<T, TCPHeader>) {
                builder.tcp(header);
            } else if constexpr (std::is_same_v<T, UDPHeader>) {
                builder.udp(header);
            } else if constexpr (std::is_same_v<T, ICMPHeader>) {
                builder.icmp(header);
            }
        };
        
        add_header(first_header);
        
        // Process additional headers using fold expression
        if constexpr (sizeof...(additional_headers) > 0) {
            (add_header(additional_headers), ...);
        }
        
        return builder;
    };
    
    // Test building different packets
    auto packet1 = build_packet(eth, ipv4, tcp).payload("HTTP GET").build();
    auto packet2 = build_packet(eth, mpls, ipv4, udp).payload("DNS Query").build();
    auto packet3 = build_packet(ipv4, icmp).payload("Ping").build();
    
    std::cout << "Packet 1 (Eth+IPv4+TCP): " << packet1.size() << " bytes\n";
    std::cout << "Packet 2 (Eth+MPLS+IPv4+UDP): " << packet2.size() << " bytes\n";
    std::cout << "Packet 3 (IPv4+ICMP): " << packet3.size() << " bytes\n";
    
    // Test 5: Constexpr validation
    std::cout << "\nTest 5: Compile-time Type Validation\n";
    
    auto validate_string = [](auto&& param) {
        using U = std::remove_reference_t<decltype(param)>;
        
        if constexpr (std::is_array_v<U> && 
                     (std::is_same_v<std::remove_extent_t<U>, char> ||
                      std::is_same_v<std::remove_extent_t<U>, const char>)) {
            std::cout << "✓ Valid string literal: \"" << param << "\" [" << std::extent_v<U> << " chars]\n";
            return true;
        } else {
            std::cout << "✗ Not a string literal\n";
            return false;
        }
    };
    
    // Validation tests
    validate_string("Valid literal");
    validate_string(std::string("Not a literal"));
    const char* not_literal = "Not a literal either";
    validate_string(not_literal);
    
    // Test 6: Member Function Invocability Checking using std::is_invocable
    std::cout << "\nTest 6: Member Function Invocability Checking\n";
    
    // Create a test class with various methods
    class NetworkProcessor {
    public:
        void process_packet(const std::vector<uint8_t>& data) {
            std::cout << "Processing " << data.size() << " byte packet\n";
        }
        
        void set_header(const std::string& header_type, int value) {
            std::cout << "Setting " << header_type << " header to " << value << "\n";
        }
        
        int calculate_checksum(const std::vector<uint8_t>& data) {
            return data.size() % 256;  // Simple checksum
        }
        
        bool validate_address(const IPv4Address& addr) {
            (void)addr;  // Suppress unused parameter warning
            return true;  // Always valid for demo
        }
        
        void log_message(const char* msg) {
            std::cout << "Log: " << msg << "\n";
        }
        
    private:
        void internal_method(int x) { (void)x; }  // Private - not invocable from outside
    };
    
    // Template to check if we can call a method with given arguments
    auto check_method_invocable = [](auto method_ptr, auto&& obj, auto&&... args) {
        using MethodType = decltype(method_ptr);
        using ObjectType = std::decay_t<decltype(obj)>;
        
        constexpr bool is_callable = std::is_invocable_v<MethodType, ObjectType&, decltype(args)...>;
        
        std::cout << "Method invocable: " << std::boolalpha << is_callable;
        
        if constexpr (is_callable) {
            using ReturnType = std::invoke_result_t<MethodType, ObjectType&, decltype(args)...>;
            std::cout << " -> returns " << typeid(ReturnType).name();
        }
        std::cout << "\n";
        
        return is_callable;
    };
    
    NetworkProcessor processor;
    std::vector<uint8_t> test_data = {0x01, 0x02, 0x03, 0x04};
    IPv4Address test_addr("192.168.1.1");
    
    std::cout << "Testing NetworkProcessor::process_packet:\n";
    std::cout << "  With vector<uint8_t>: ";
    check_method_invocable(&NetworkProcessor::process_packet, processor, test_data);
    
    std::cout << "  With string (invalid): ";
    check_method_invocable(&NetworkProcessor::process_packet, processor, std::string("invalid"));
    
    std::cout << "\nTesting NetworkProcessor::set_header:\n";
    std::cout << "  With (string, int): ";
    check_method_invocable(&NetworkProcessor::set_header, processor, std::string("TCP"), 80);
    
    std::cout << "  With (const char*, int): ";
    check_method_invocable(&NetworkProcessor::set_header, processor, "UDP", 53);
    
    std::cout << "  With (string, string) - invalid: ";
    check_method_invocable(&NetworkProcessor::set_header, processor, std::string("ICMP"), std::string("invalid"));
    
    std::cout << "\nTesting NetworkProcessor::calculate_checksum:\n";
    std::cout << "  With vector<uint8_t>: ";
    check_method_invocable(&NetworkProcessor::calculate_checksum, processor, test_data);
    
    std::cout << "  With int (invalid): ";
    check_method_invocable(&NetworkProcessor::calculate_checksum, processor, 42);
    
    std::cout << "\nTesting NetworkProcessor::validate_address:\n";
    std::cout << "  With IPv4Address: ";
    check_method_invocable(&NetworkProcessor::validate_address, processor, test_addr);
    
    std::cout << "  With string (invalid): ";
    check_method_invocable(&NetworkProcessor::validate_address, processor, std::string("192.168.1.1"));
    
    std::cout << "\nTesting NetworkProcessor::log_message:\n";
    std::cout << "  With const char*: ";
    check_method_invocable(&NetworkProcessor::log_message, processor, "Test message");
    
    std::cout << "  With string literal: ";
    check_method_invocable(&NetworkProcessor::log_message, processor, "Literal message");
    
    std::cout << "  With std::string: ";
    check_method_invocable(&NetworkProcessor::log_message, processor, std::string("String object"));
    
    // Test with return type checking using std::is_invocable_r
    auto check_method_with_return = [](auto expected_return_type, auto method_ptr, auto&& obj, auto&&... args) {
        using MethodType = decltype(method_ptr);
        using ObjectType = std::decay_t<decltype(obj)>;
        using ExpectedReturn = decltype(expected_return_type);
        
        constexpr bool is_callable_with_return = std::is_invocable_r_v<ExpectedReturn, MethodType, ObjectType&, decltype(args)...>;
        
        std::cout << "Method returns expected type (" << typeid(ExpectedReturn).name() << "): " 
                  << std::boolalpha << is_callable_with_return << "\n";
        
        return is_callable_with_return;
    };
    
    std::cout << "\nTesting return type compatibility:\n";
    std::cout << "  calculate_checksum returns int: ";
    check_method_with_return(int{}, &NetworkProcessor::calculate_checksum, processor, test_data);
    
    std::cout << "  calculate_checksum returns double (convertible): ";
    check_method_with_return(double{}, &NetworkProcessor::calculate_checksum, processor, test_data);
    
    std::cout << "  calculate_checksum returns string (invalid): ";
    check_method_with_return(std::string{}, &NetworkProcessor::calculate_checksum, processor, test_data);
    
    std::cout << "  validate_address returns bool: ";
    check_method_with_return(bool{}, &NetworkProcessor::validate_address, processor, test_addr);
    
    // Special check for void return type
    auto check_void_return = [](auto method_ptr, auto&& obj, auto&&... args) {
        using MethodType = decltype(method_ptr);
        using ObjectType = std::decay_t<decltype(obj)>;
        
        constexpr bool returns_void = std::is_invocable_r_v<void, MethodType, ObjectType&, decltype(args)...>;
        
        std::cout << "Method returns void: " << std::boolalpha << returns_void << "\n";
        return returns_void;
    };
    
    std::cout << "  process_packet returns void: ";
    check_void_return(&NetworkProcessor::process_packet, processor, test_data);
    
    std::cout << "  set_header returns void: ";
    check_void_return(&NetworkProcessor::set_header, processor, std::string("TCP"), 80);
    
    // Test actual method calls using std::invoke when they're valid
    std::cout << "\nActual method calls using std::invoke:\n";
    
    if constexpr (std::is_invocable_v<decltype(&NetworkProcessor::process_packet), NetworkProcessor&, std::vector<uint8_t>>) {
        std::cout << "  Calling process_packet: ";
        std::invoke(&NetworkProcessor::process_packet, processor, test_data);
    }
    
    if constexpr (std::is_invocable_v<decltype(&NetworkProcessor::set_header), NetworkProcessor&, std::string, int>) {
        std::cout << "  Calling set_header: ";
        std::invoke(&NetworkProcessor::set_header, processor, std::string("HTTP"), 80);
    }
    
    if constexpr (std::is_invocable_v<decltype(&NetworkProcessor::calculate_checksum), NetworkProcessor&, std::vector<uint8_t>>) {
        std::cout << "  Calling calculate_checksum: ";
        int checksum = std::invoke(&NetworkProcessor::calculate_checksum, processor, test_data);
        std::cout << "Result: " << checksum << "\n";
    }
    
    if constexpr (std::is_invocable_v<decltype(&NetworkProcessor::validate_address), NetworkProcessor&, IPv4Address>) {
        std::cout << "  Calling validate_address: ";
        bool valid = std::invoke(&NetworkProcessor::validate_address, processor, test_addr);
        std::cout << "Result: " << std::boolalpha << valid << "\n";
    }
    
    std::cout << "\n=== Templated Lambda Tests Complete ===\n";
    
    return 0;
}
