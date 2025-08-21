cmake_minimum_required(VERSION 3.10)
project(CppScapy LANGUAGES CXX)

# Include source file definitions from src directory
add_subdirectory(src)

# Create library using the source files defined in src/
add_library(cppscapy STATIC
    ${CPPSCAPY_SOURCES}
    ${CPPSCAPY_HEADERS}
)

# Set target properties
target_include_directories(cppscapy PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Set C++ standard for this target
target_compile_features(cppscapy PUBLIC cxx_std_17)

# Target-specific compiler flags
target_compile_options(cppscapy PRIVATE 
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra>
    $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

# Create demo executable
add_executable(demo
    examples/demo.cpp
)

# Link demo with library
target_link_libraries(demo cppscapy)

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

# Incremental pattern test
add_executable(incremental_pattern_test
    examples/incremental_pattern_test.cpp
)

target_link_libraries(incremental_pattern_test cppscapy)

# Move semantics test
add_executable(move_semantics_test
    examples/move_semantics_test.cpp
)

target_link_libraries(move_semantics_test cppscapy)

# In-place randomization test
add_executable(inplace_randomization_test
    examples/inplace_randomization_test.cpp
)

target_link_libraries(inplace_randomization_test cppscapy)

# Performance comparison test
add_executable(performance_comparison
    examples/performance_comparison.cpp
)

target_link_libraries(performance_comparison cppscapy)

# PCAP demo
add_executable(pcap_demo
    examples/pcap_demo.cpp
)

target_link_libraries(pcap_demo cppscapy)

# Simple PCAP example
add_executable(simple_pcap_example
    examples/simple_pcap_example.cpp
)

target_link_libraries(simple_pcap_example cppscapy)
