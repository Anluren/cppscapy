# Source files for the cppscapy library
# Use absolute paths so they work from parent directory
set(CPPSCAPY_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/network_headers.cpp
    ${CMAKE_CURRENT_LIST_DIR}/tcp_udp_icmp.cpp
    ${CMAKE_CURRENT_LIST_DIR}/utils.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pcap_support.cpp
    PARENT_SCOPE  # Make variable available in parent scope
)

# Optional: Add headers for IDE support
set(CPPSCAPY_HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/../include/network_headers.h
    ${CMAKE_CURRENT_LIST_DIR}/../include/utils.h
    ${CMAKE_CURRENT_LIST_DIR}/../include/header_dsl.h
    ${CMAKE_CURRENT_LIST_DIR}/../include/generated_headers.h
    ${CMAKE_CURRENT_LIST_DIR}/../include/pcap_support.h
    PARENT_SCOPE
)
