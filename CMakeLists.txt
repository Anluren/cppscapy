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
