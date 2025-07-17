# Compile-Time Length Deduction in CppScapy

## Overview

Yes, it's absolutely possible to deduce the length of a hex string at compile time! This feature has been implemented in CppScapy using C++17 template metaprogramming.

## The `from_hex_string_auto` Function

The key function is `from_hex_string_auto`, which automatically deduces the array size based on the hex string length:

```cpp
template<size_t N>
constexpr std::array<uint8_t, N / 2> from_hex_string_auto(const char (&hex_str)[N]) {
    static_assert(N > 1, "Hex string must not be empty");
    static_assert((N - 1) % 2 == 0, "Hex string must have even number of characters");
    
    constexpr size_t array_size = (N - 1) / 2;  // -1 for null terminator
    std::array<uint8_t, array_size> result = {};
    
    // Convert hex characters to bytes at compile time
    for (size_t i = 0; i < array_size; ++i) {
        result[i] = (hex_to_byte(hex_str[i * 2]) << 4) | hex_to_byte(hex_str[i * 2 + 1]);
    }
    
    return result;
}
```

## Key Features

### 1. Automatic Size Deduction
The function takes a character array reference `const char (&hex_str)[N]` where `N` is automatically deduced by the compiler. The resulting array size is `(N-1)/2` bytes.

### 2. Compile-Time Validation
- `static_assert(N > 1, ...)` ensures the string is not empty
- `static_assert((N - 1) % 2 == 0, ...)` ensures even number of hex characters
- All validation happens at compile time

### 3. constexpr Evaluation
The entire function is `constexpr`, meaning it can be evaluated at compile time when given constant expressions.

## Usage Examples

### Basic Usage
```cpp
constexpr auto mac_bytes = from_hex_string_auto("001122334455");    // 6 bytes
constexpr auto ipv4_bytes = from_hex_string_auto("C0A80101");       // 4 bytes
constexpr auto custom_bytes = from_hex_string_auto("ABCD");         // 2 bytes
constexpr auto single_byte = from_hex_string_auto("FF");            // 1 byte
```

### Size Verification
```cpp
static_assert(from_hex_string_auto("12").size() == 1);              // 1 byte
static_assert(from_hex_string_auto("1234").size() == 2);            // 2 bytes
static_assert(from_hex_string_auto("123456").size() == 3);          // 3 bytes
static_assert(from_hex_string_auto("12345678").size() == 4);        // 4 bytes
```

### Integration with Network Headers
```cpp
constexpr auto mac_data = from_hex_string_auto("001122334455");
MacAddress mac_addr(mac_data);  // Works seamlessly

constexpr auto ipv4_data = from_hex_string_auto("C0A80101");
IPv4Address ipv4_addr(ipv4_data[0], ipv4_data[1], ipv4_data[2], ipv4_data[3]);
```

## Convenience Functions

The library also provides convenience functions that leverage automatic length deduction:

```cpp
// Automatically validates string length for MAC addresses (12 hex chars + null)
auto mac_addr = make_mac_address("FFFFFFFFFFFF");

// Automatically validates string length for IPv4 addresses (8 hex chars + null)
auto ipv4_addr = make_ipv4_address("08080808");  // 8.8.8.8
```

## Benefits

### 1. Type Safety
- Array sizes are determined at compile time
- No runtime length calculations
- Compile-time validation prevents errors

### 2. Performance
- Zero runtime overhead for length calculation
- All validation happens at compile time
- Optimal code generation

### 3. Ease of Use
- No need to specify array sizes manually
- Automatic deduction from string literals
- Compile-time error messages for invalid input

### 4. Consistency
- Works with any hex string length
- Handles both uppercase and lowercase
- Validates input format at compile time

## Limitations

1. **String Literals Only**: The function works with string literals (`"ABCD"`) but not with `std::string` or runtime strings.

2. **Even Length Requirement**: The hex string must have an even number of characters (excluding null terminator).

3. **Pure Hex Characters**: The string should contain only hex characters (0-9, A-F, a-f). No separators like colons or spaces.

4. **Constructor Constraints**: While the hex parsing is constexpr, the network address constructors (MacAddress, IPv4Address) are not constexpr, so you can't create fully constexpr address objects.

## Test Results

The implementation has been thoroughly tested:
- ✅ Automatic size deduction works correctly
- ✅ Compile-time validation catches errors
- ✅ Case insensitive parsing works
- ✅ Round-trip conversion (hex → bytes → hex) works
- ✅ Integration with existing network headers works
- ✅ All static assertions pass at compile time

## Conclusion

The compile-time length deduction feature makes working with hex strings much more convenient and safe. It leverages C++17's template system to provide automatic size deduction while maintaining type safety and performance.
