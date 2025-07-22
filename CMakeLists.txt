cmake_minimum_required(VERSION 3.10)
project(CppScapy)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add include directories
include_directories(include)

# Create library
add_library(cppscapy STATIC
    src/network_headers.cpp
    src/tcp_udp_icmp.cpp
    src/utils.cpp
)

# Create demo executable
add_executable(demo
    examples/demo.cpp
)

# Link demo with library
target_link_libraries(demo cppscapy)

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

# Optional: Create a simple test executable
add_executable(simple_test
    examples/simple_test.cpp
)

target_link_libraries(simple_test cppscapy)

# Quick usage example
add_executable(quick_usage
    examples/quick_usage.cpp
)

target_link_libraries(quick_usage cppscapy)

# Utils test
add_executable(utils_test
    examples/utils_test.cpp
)

target_link_libraries(utils_test cppscapy)

# Constexpr test
add_executable(constexpr_test
    examples/constexpr_test.cpp
)

target_link_libraries(constexpr_test cppscapy)

# Auto hex test
add_executable(auto_hex_test
    examples/auto_hex_test.cpp
)

target_link_libraries(auto_hex_test cppscapy)

# Length deduction demo
add_executable(length_deduction_demo
    examples/length_deduction_demo.cpp
)

target_link_libraries(length_deduction_demo cppscapy)

# Quick deduction example
add_executable(quick_deduction_example
    examples/quick_deduction_example.cpp
)

target_link_libraries(quick_deduction_example cppscapy)

# IPv4 checksum test
add_executable(ipv4_checksum_test
    examples/ipv4_checksum_test.cpp
)

target_link_libraries(ipv4_checksum_test cppscapy)

# Checksum methods test
add_executable(checksum_methods_test
    examples/checksum_methods_test.cpp
)

target_link_libraries(checksum_methods_test cppscapy)

# MPLS test
add_executable(mpls_test
    examples/mpls_test.cpp
)

target_link_libraries(mpls_test cppscapy)

# Templated lambda test
add_executable(templated_lambda_test
    examples/templated_lambda_test.cpp
)

target_link_libraries(templated_lambda_test cppscapy)

# String literal detection test
add_executable(string_literal_test
    examples/string_literal_test.cpp
)

target_link_libraries(string_literal_test cppscapy)

# Random payload generation test
add_executable(random_payload_test
    examples/random_payload_test.cpp
)

target_link_libraries(random_payload_test cppscapy)

# Quick random examples
add_executable(quick_random_examples
    examples/quick_random_examples.cpp
)

target_link_libraries(quick_random_examples cppscapy)
