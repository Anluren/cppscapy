#include "../include/utils.h"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <random>
#include <cctype>

namespace cppscapy {
namespace utils {

// Utility to print packet in hex format
void print_hex(const std::vector<uint8_t>& data, const std::string& description) {
    if (!description.empty()) {
        std::cout << description << ":\n";
    }
    std::cout << "Length: " << data.size() << " bytes\n";
    std::cout << "Hex: ";
    
    for (size_t i = 0; i < data.size(); ++i) {
        std::cout << std::hex << std::setfill('0') << std::setw(2) 
                  << static_cast<int>(data[i]) << " ";
    }
    std::cout << std::dec << "\n";
}

// Utility to print packet in both hex and ASCII
void print_hex_ascii(const std::vector<uint8_t>& data, const std::string& description) {
    if (!description.empty()) {
        std::cout << description << ":\n";
    }
    std::cout << "Length: " << data.size() << " bytes\n";
    
    for (size_t i = 0; i < data.size(); i += 16) {
        // Print offset
        std::cout << std::hex << std::setfill('0') << std::setw(4) << i << ": ";
        
        // Print hex bytes
        for (size_t j = 0; j < 16; ++j) {
            if (i + j < data.size()) {
                std::cout << std::hex << std::setfill('0') << std::setw(2) 
                          << static_cast<int>(data[i + j]) << " ";
            } else {
                std::cout << "   ";
            }
        }
        
        std::cout << " ";
        
        // Print ASCII representation
        for (size_t j = 0; j < 16 && i + j < data.size(); ++j) {
            char c = static_cast<char>(data[i + j]);
            std::cout << (std::isprint(c) ? c : '.');
        }
        
        std::cout << std::dec << "\n";
    }
}

// Convert packet to hex string
std::string to_hex_string(const std::vector<uint8_t>& data) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    
    for (size_t i = 0; i < data.size(); ++i) {
        ss << std::setw(2) << static_cast<int>(data[i]);
    }
    
    return ss.str();
}

// Parse hex string back to packet
std::vector<uint8_t> from_hex_string(const std::string& hex_str) {
    std::vector<uint8_t> result;
    std::string clean_hex;
    
    // Remove spaces and convert to lowercase
    for (char c : hex_str) {
        if (std::isxdigit(c)) {
            clean_hex += std::tolower(c);
        }
    }
    
    // Must have even number of hex characters
    if (clean_hex.length() % 2 != 0) {
        throw std::invalid_argument("Invalid hex string length");
    }
    
    // Convert pairs of hex characters to bytes
    for (size_t i = 0; i < clean_hex.length(); i += 2) {
        std::string byte_str = clean_hex.substr(i, 2);
        uint8_t byte = static_cast<uint8_t>(std::stoul(byte_str, nullptr, 16));
        result.push_back(byte);
    }
    
    return result;
}

// Calculate IP checksum
uint16_t calculate_ip_checksum(const std::vector<uint8_t>& header) {
    uint32_t sum = 0;
    
    // Sum all 16-bit words
    for (size_t i = 0; i < header.size() - 1; i += 2) {
        sum += (header[i] << 8) + header[i + 1];
    }
    
    // Add odd byte if present
    if (header.size() % 2 == 1) {
        sum += header[header.size() - 1] << 8;
    }
    
    // Fold 32-bit sum to 16 bits
    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }
    
    return ~sum;
}

// Calculate IPv4 header checksum (automatically clears checksum field)
uint16_t calculate_ipv4_header_checksum(const std::vector<uint8_t>& header) {
    if (header.size() < 20) {
        throw std::invalid_argument("IPv4 header too short");
    }
    
    // Get the IHL (Internet Header Length) from the header
    uint8_t ihl = (header[0] & 0x0F) * 4; // IHL is in 4-byte words
    
    if (header.size() < ihl || ihl < 20) {
        throw std::invalid_argument("Invalid IPv4 header length");
    }
    
    // Create a copy of the header and clear the checksum field
    std::vector<uint8_t> header_copy = header;
    header_copy[10] = 0; // Clear checksum field
    header_copy[11] = 0;
    
    // Resize to actual header length (in case input is longer)
    header_copy.resize(ihl);
    
    return calculate_ip_checksum(header_copy);
}

// Calculate TCP checksum (simplified - doesn't include pseudo-header)
uint16_t calculate_tcp_checksum(const std::vector<uint8_t>& tcp_header, 
                               const IPv4Address& src_ip, 
                               const IPv4Address& dst_ip,
                               const std::vector<uint8_t>& payload) {
    
    // Create pseudo-header
    std::vector<uint8_t> pseudo_header;
    
    // Source IP (4 bytes)
    auto src_bytes = src_ip.to_bytes();
    pseudo_header.insert(pseudo_header.end(), src_bytes.begin(), src_bytes.end());
    
    // Destination IP (4 bytes)
    auto dst_bytes = dst_ip.to_bytes();
    pseudo_header.insert(pseudo_header.end(), dst_bytes.begin(), dst_bytes.end());
    
    // Reserved (1 byte)
    pseudo_header.push_back(0);
    
    // Protocol (1 byte)
    pseudo_header.push_back(6); // TCP
    
    // TCP length (2 bytes)
    uint16_t tcp_length = tcp_header.size() + payload.size();
    pseudo_header.push_back((tcp_length >> 8) & 0xFF);
    pseudo_header.push_back(tcp_length & 0xFF);
    
    // Combine pseudo-header, TCP header, and payload
    std::vector<uint8_t> combined;
    combined.insert(combined.end(), pseudo_header.begin(), pseudo_header.end());
    combined.insert(combined.end(), tcp_header.begin(), tcp_header.end());
    combined.insert(combined.end(), payload.begin(), payload.end());
    
    return calculate_ip_checksum(combined);
}

// Verify IPv4 header checksum - raw pointer version
bool verify_ipv4_checksum(const uint8_t* data, size_t length) {
    if (!data || length < 20) {
        return false; // Invalid data or length too short for IPv4 header
    }
    
    // Get the IHL (Internet Header Length) from the header
    uint8_t ihl = (data[0] & 0x0F) * 4; // IHL is in 4-byte words
    
    if (length < ihl || ihl < 20) {
        return false; // Invalid header length
    }
    
    // Calculate checksum for the IPv4 header
    uint32_t sum = 0;
    
    // Sum all 16-bit words, skipping the checksum field (bytes 10-11)
    for (size_t i = 0; i < ihl; i += 2) {
        if (i == 10) {
            // Skip the checksum field - treat as zero
            continue;
        }
        
        uint16_t word;
        if (i + 1 < ihl) {
            word = (data[i] << 8) + data[i + 1];
        } else {
            word = data[i] << 8; // Last byte if odd length
        }
        sum += word;
    }
    
    // Fold 32-bit sum to 16 bits
    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }
    
    // Get the checksum from the header
    uint16_t header_checksum = (data[10] << 8) + data[11];
    
    // The calculated checksum should be the complement of the header checksum
    uint16_t calculated_checksum = ~sum;
    
    return calculated_checksum == header_checksum;
}

// Verify IPv4 header checksum - vector version
bool verify_ipv4_checksum(const std::vector<uint8_t>& header) {
    if (header.empty()) {
        return false;
    }
    return verify_ipv4_checksum(header.data(), header.size());
}

// Parse packet and extract information
PacketInfo analyze_packet(const std::vector<uint8_t>& packet) {
    PacketInfo info;
    size_t offset = 0;
    
    // Check if packet is large enough for Ethernet header
    if (packet.size() < 14) {
        return info;
    }
    
    // Parse Ethernet header
    info.has_ethernet = true;
    
    // Extract MAC addresses
    std::array<uint8_t, 6> dst_mac_bytes, src_mac_bytes;
    std::copy(packet.begin(), packet.begin() + 6, dst_mac_bytes.begin());
    std::copy(packet.begin() + 6, packet.begin() + 12, src_mac_bytes.begin());
    
    info.dst_mac = MacAddress(dst_mac_bytes);
    info.src_mac = MacAddress(src_mac_bytes);
    
    // Extract EtherType
    info.ethertype = (packet[12] << 8) | packet[13];
    offset = 14;
    
    // Parse IP header based on EtherType
    if (info.ethertype == 0x0800 && packet.size() >= offset + 20) { // IPv4
        info.has_ipv4 = true;
        
        // Extract protocol
        info.ip_protocol = packet[offset + 9];
        
        // Extract IP addresses
        std::array<uint8_t, 4> src_ip_bytes, dst_ip_bytes;
        std::copy(packet.begin() + offset + 12, packet.begin() + offset + 16, src_ip_bytes.begin());
        std::copy(packet.begin() + offset + 16, packet.begin() + offset + 20, dst_ip_bytes.begin());
        
        info.src_ipv4 = IPv4Address(src_ip_bytes[0], src_ip_bytes[1], src_ip_bytes[2], src_ip_bytes[3]);
        info.dst_ipv4 = IPv4Address(dst_ip_bytes[0], dst_ip_bytes[1], dst_ip_bytes[2], dst_ip_bytes[3]);
        
        // Get IP header length
        uint8_t ihl = (packet[offset] & 0x0F) * 4;
        offset += ihl;
        
        // Parse transport layer
        if (info.ip_protocol == 6 && packet.size() >= offset + 20) { // TCP
            info.has_tcp = true;
            info.src_port = (packet[offset] << 8) | packet[offset + 1];
            info.dst_port = (packet[offset + 2] << 8) | packet[offset + 3];
            
            uint8_t tcp_header_length = ((packet[offset + 12] >> 4) & 0x0F) * 4;
            offset += tcp_header_length;
        } else if (info.ip_protocol == 17 && packet.size() >= offset + 8) { // UDP
            info.has_udp = true;
            info.src_port = (packet[offset] << 8) | packet[offset + 1];
            info.dst_port = (packet[offset + 2] << 8) | packet[offset + 3];
            offset += 8;
        } else if (info.ip_protocol == 1 && packet.size() >= offset + 8) { // ICMP
            info.has_icmp = true;
            offset += 8;
        }
    }
    
    info.payload_offset = offset;
    info.payload_size = (packet.size() > offset) ? packet.size() - offset : 0;
    
    return info;
}

// Generate random payload
std::vector<uint8_t> generate_random_payload(size_t size) {
    std::vector<uint8_t> payload(size);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);
    
    for (size_t i = 0; i < size; ++i) {
        payload[i] = static_cast<uint8_t>(dis(gen));
    }
    
    return payload;
}

// Generate HTTP GET request
std::vector<uint8_t> generate_http_get_request(const std::string& host, const std::string& path) {
    std::stringstream ss;
    ss << "GET " << path << " HTTP/1.1\r\n";
    ss << "Host: " << host << "\r\n";
    ss << "User-Agent: CppScapy/1.0\r\n";
    ss << "Accept: */*\r\n";
    ss << "Connection: close\r\n";
    ss << "\r\n";
    
    std::string request = ss.str();
    return std::vector<uint8_t>(request.begin(), request.end());
}

// Generate DNS query (simplified)
std::vector<uint8_t> generate_dns_query(const std::string& domain) {
    std::vector<uint8_t> query;
    
    // DNS Header (12 bytes)
    query.push_back(0x12); query.push_back(0x34); // Transaction ID
    query.push_back(0x01); query.push_back(0x00); // Flags (standard query)
    query.push_back(0x00); query.push_back(0x01); // Questions: 1
    query.push_back(0x00); query.push_back(0x00); // Answer RRs: 0
    query.push_back(0x00); query.push_back(0x00); // Authority RRs: 0
    query.push_back(0x00); query.push_back(0x00); // Additional RRs: 0
    
    // Question section
    std::stringstream ss(domain);
    std::string label;
    
    while (std::getline(ss, label, '.')) {
        query.push_back(static_cast<uint8_t>(label.length()));
        for (char c : label) {
            query.push_back(static_cast<uint8_t>(c));
        }
    }
    
    query.push_back(0x00); // End of name
    query.push_back(0x00); query.push_back(0x01); // Type: A
    query.push_back(0x00); query.push_back(0x01); // Class: IN
    
    return query;
}

// Random payload generation utilities
namespace random {

// Generate completely random payload
std::vector<uint8_t> random_bytes(size_t size) {
    std::vector<uint8_t> payload(size);
    randomize_bytes(payload);
    return payload;
}

// Generate random payload with specific byte range
std::vector<uint8_t> random_bytes_range(size_t size, uint8_t min_val, uint8_t max_val) {
    std::vector<uint8_t> payload(size);
    randomize_bytes_range(payload, min_val, max_val);
    return payload;
}

// Generate random printable ASCII payload
std::vector<uint8_t> random_ascii(size_t size) {
    std::vector<uint8_t> payload(size);
    randomize_ascii(payload);
    return payload;
}

// Generate random alphanumeric payload
std::vector<uint8_t> random_alphanumeric(size_t size) {
    std::vector<uint8_t> payload(size);
    randomize_alphanumeric(payload);
    return payload;
}

// Generate random hex-like payload (0-9, A-F)
std::vector<uint8_t> random_hex_chars(size_t size) {
    std::vector<uint8_t> payload(size);
    randomize_hex_chars(payload);
    return payload;
}

// Generate random payload with specific pattern
std::vector<uint8_t> random_pattern(size_t size, const std::vector<uint8_t>& pattern_chars) {
    std::vector<uint8_t> payload(size);
    randomize_pattern(payload, pattern_chars);
    return payload;
}

// Generate random payload with seed (for reproducible results)
std::vector<uint8_t> random_bytes_seeded(size_t size, uint32_t seed) {
    std::vector<uint8_t> payload(size);
    randomize_bytes_seeded(payload, seed);
    return payload;
}

// Generate random network-like payload (mix of binary and text)
std::vector<uint8_t> random_network_data(size_t size) {
    std::vector<uint8_t> payload(size);
    randomize_network_data(payload);
    return payload;
}

// Generate random payload with repeating pattern
std::vector<uint8_t> random_repeating_pattern(size_t size, size_t pattern_length) {
    if (pattern_length == 0) {
        pattern_length = 1;
    }
    
    auto pattern = random_bytes(pattern_length);
    std::vector<uint8_t> payload(size);
    
    for (size_t i = 0; i < size; ++i) {
        payload[i] = pattern[i % pattern_length];
    }
    
    return payload;
}

// Generate random payload simulating HTTP-like data
std::vector<uint8_t> random_http_like(size_t size) {
    std::vector<uint8_t> payload(size);
    randomize_http_like(payload);
    return payload;
}

// Generate random payload simulating binary protocol
std::vector<uint8_t> random_binary_protocol(size_t size) {
    std::vector<uint8_t> payload(size);
    randomize_binary_protocol(payload);
    return payload;
}

// Generate incremental pattern payload (0, 1, 2, ..., 255, 0, 1, ...)
std::vector<uint8_t> incremental_pattern(size_t size, uint8_t start_value) {
    std::vector<uint8_t> payload(size);
    fill_incremental_pattern(payload, start_value);
    return payload;
}

// In-place randomization functions for existing vectors
void randomize_bytes(std::vector<uint8_t>& data) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<uint8_t> dis(0, 255);
    
    std::generate(data.begin(), data.end(), []() {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_int_distribution<uint8_t> dis(0, 255);
        return dis(gen);
    });
}

void randomize_bytes_range(std::vector<uint8_t>& data, uint8_t min_val, uint8_t max_val) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<uint8_t> dis(min_val, max_val);
    
    std::generate(data.begin(), data.end(), [&]() { return dis(gen); });
}

void randomize_ascii(std::vector<uint8_t>& data) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<uint8_t> dis(32, 126); // Printable ASCII range
    
    std::generate(data.begin(), data.end(), [&]() { return dis(gen); });
}

void randomize_alphanumeric(std::vector<uint8_t>& data) {
    static const std::string chars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> dis(0, chars.size() - 1);
    
    std::generate(data.begin(), data.end(), [&]() {
        return static_cast<uint8_t>(chars[dis(gen)]);
    });
}

void randomize_hex_chars(std::vector<uint8_t>& data) {
    static const std::string hex_chars = "0123456789ABCDEF";
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> dis(0, hex_chars.size() - 1);
    
    std::generate(data.begin(), data.end(), [&]() {
        return static_cast<uint8_t>(hex_chars[dis(gen)]);
    });
}

void randomize_pattern(std::vector<uint8_t>& data, const std::vector<uint8_t>& pattern_chars) {
    if (pattern_chars.empty()) {
        randomize_bytes(data);
        return;
    }
    
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> dis(0, pattern_chars.size() - 1);
    
    std::generate(data.begin(), data.end(), [&]() {
        return pattern_chars[dis(gen)];
    });
}

void randomize_bytes_seeded(std::vector<uint8_t>& data, uint32_t seed) {
    std::mt19937 gen(seed);
    std::uniform_int_distribution<uint8_t> dis(0, 255);
    
    std::generate(data.begin(), data.end(), [&]() { return dis(gen); });
}

void randomize_network_data(std::vector<uint8_t>& data) {
    if (data.empty()) return;
    
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> mode_dis(0, 2);
    
    size_t pos = 0;
    while (pos < data.size()) {
        int mode = mode_dis(gen);
        size_t chunk_size = std::min(data.size() - pos,
                                   static_cast<size_t>(std::uniform_int_distribution<int>(4, 32)(gen)));
        
        if (mode == 0) {
            // Binary data
            std::uniform_int_distribution<uint8_t> byte_dis(0, 255);
            std::generate(data.begin() + pos, data.begin() + pos + chunk_size,
                         [&]() { return byte_dis(gen); });
        } else if (mode == 1) {
            // ASCII text
            std::uniform_int_distribution<uint8_t> ascii_dis(32, 126);
            std::generate(data.begin() + pos, data.begin() + pos + chunk_size,
                         [&]() { return ascii_dis(gen); });
        } else {
            // Alphanumeric
            static const std::string chars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
            std::uniform_int_distribution<size_t> char_dis(0, chars.size() - 1);
            std::generate(data.begin() + pos, data.begin() + pos + chunk_size,
                         [&]() { return static_cast<uint8_t>(chars[char_dis(gen)]); });
        }
        pos += chunk_size;
    }
}

void randomize_http_like(std::vector<uint8_t>& data) {
    if (data.empty()) return;
    
    static const std::vector<std::string> http_words = {
        "GET", "POST", "PUT", "DELETE", "HTTP", "Host:", "Content-Type:",
        "User-Agent:", "Accept:", "Connection:", "close", "keep-alive",
        "application/json", "text/html", "Mozilla", "Chrome", "Firefox"
    };
    
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> word_dis(0, http_words.size() - 1);
    std::uniform_int_distribution<int> space_dis(0, 3);
    
    size_t pos = 0;
    while (pos < data.size()) {
        const std::string& word = http_words[word_dis(gen)];
        
        // Copy word characters
        for (char c : word) {
            if (pos >= data.size()) break;
            data[pos++] = static_cast<uint8_t>(c);
        }
        
        // Add separator
        if (pos < data.size()) {
            char separator = (space_dis(gen) == 0) ? '\n' :
                           (space_dis(gen) == 1) ? '\r' : ' ';
            data[pos++] = static_cast<uint8_t>(separator);
        }
    }
}

void randomize_binary_protocol(std::vector<uint8_t>& data) {
    if (data.empty()) return;
    
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<uint8_t> byte_dis(0, 255);
    std::uniform_int_distribution<int> structure_dis(0, 4);
    
    size_t pos = 0;
    while (pos < data.size()) {
        int structure = structure_dis(gen);
        
        if (structure == 0 && pos + 4 <= data.size()) {
            // 32-bit length field (big endian)
            uint32_t len = byte_dis(gen) | (byte_dis(gen) << 8) |
                          (byte_dis(gen) << 16) | (byte_dis(gen) << 24);
            data[pos++] = (len >> 24) & 0xFF;
            data[pos++] = (len >> 16) & 0xFF;
            data[pos++] = (len >> 8) & 0xFF;
            data[pos++] = len & 0xFF;
        } else if (structure == 1 && pos + 2 <= data.size()) {
            // 16-bit value (big endian)
            uint16_t val = byte_dis(gen) | (byte_dis(gen) << 8);
            data[pos++] = (val >> 8) & 0xFF;
            data[pos++] = val & 0xFF;
        } else {
            // Random byte
            data[pos++] = byte_dis(gen);
        }
    }
}

void fill_incremental_pattern(std::vector<uint8_t>& data, uint8_t start_value) {
    uint8_t current_value = start_value;
    for (auto& byte : data) {
        byte = current_value;
        current_value = (current_value + 1) % 256; // Wrap around after 255
    }
}

// Partial randomization functions (randomize only a portion of the vector)
void randomize_bytes_partial(std::vector<uint8_t>& data, size_t start_pos, size_t length) {
    if (start_pos >= data.size()) return;
    
    size_t end_pos = std::min(start_pos + length, data.size());
    
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<uint8_t> dis(0, 255);
    
    std::generate(data.begin() + start_pos, data.begin() + end_pos, []() {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_int_distribution<uint8_t> dis(0, 255);
        return dis(gen);
    });
}

void randomize_ascii_partial(std::vector<uint8_t>& data, size_t start_pos, size_t length) {
    if (start_pos >= data.size()) return;
    
    size_t end_pos = std::min(start_pos + length, data.size());
    
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<uint8_t> dis(32, 126); // Printable ASCII range
    
    std::generate(data.begin() + start_pos, data.begin() + end_pos, [&]() { return dis(gen); });
}

void fill_incremental_pattern_partial(std::vector<uint8_t>& data, size_t start_pos, size_t length, uint8_t start_value) {
    if (start_pos >= data.size()) return;
    
    size_t end_pos = std::min(start_pos + length, data.size());
    uint8_t current_value = start_value;
    
    for (size_t i = start_pos; i < end_pos; ++i) {
        data[i] = current_value;
        current_value = (current_value + 1) % 256; // Wrap around after 255
    }
}

} // namespace random

} // namespace utils
} // namespace cppscapy
