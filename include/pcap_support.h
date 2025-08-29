#pragma once

#include "header_dsl.h"
#include <chrono>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace cppscapy::pcap {

// PCAP file format constants
constexpr uint32_t PCAP_MAGIC_NUMBER = 0xa1b2c3d4;
constexpr uint32_t PCAP_MAGIC_NUMBER_SWAPPED = 0xd4c3b2a1;
constexpr uint16_t PCAP_VERSION_MAJOR = 2;
constexpr uint16_t PCAP_VERSION_MINOR = 4;

// Link layer types (from pcap-bpf.h)
enum class LinkType : uint32_t {
  NULL_LINK = 0,            // BSD loopback encapsulation
  ETHERNET = 1,             // Ethernet (10Mb, 100Mb, 1000Mb, and up)
  IEEE802_5 = 6,            // IEEE 802.5 Token Ring
  PPP = 9,                  // PPP
  FDDI = 10,                // FDDI
  RAW = 12,                 // Raw IP
  IEEE802_11 = 105,         // IEEE 802.11 wireless
  LINUX_SLL = 113,          // Linux "cooked" capture
  IEEE802_11_RADIOTAP = 127 // Radiotap link-layer information
};

// PCAP Global Header (24 bytes)
struct PcapGlobalHeader {
  uint32_t magic_number;  // Magic number
  uint16_t version_major; // Version major
  uint16_t version_minor; // Version minor
  int32_t thiszone;       // GMT to local correction
  uint32_t sigfigs;       // Accuracy of timestamps
  uint32_t snaplen;       // Max length of captured packets
  uint32_t network;       // Data link type
} __attribute__((packed));

// PCAP Packet Header (16 bytes)
struct PcapPacketHeader {
  uint32_t ts_sec;   // Timestamp seconds
  uint32_t ts_usec;  // Timestamp microseconds
  uint32_t incl_len; // Number of octets of packet saved in file
  uint32_t orig_len; // Actual length of packet
} __attribute__((packed));

// Packet representation
class Packet {
public:
  Packet() = default;

  Packet(const std::vector<uint8_t> &data,
         std::chrono::system_clock::time_point timestamp =
             std::chrono::system_clock::now())
      : data_(data), timestamp_(timestamp) {}

  // Add header to packet
  template <typename HeaderType> void add_header(const HeaderType &header) {
    static_assert(std::is_base_of_v<dsl::HeaderBase, HeaderType>,
                  "HeaderType must inherit from HeaderBase");

    auto header_bytes = header.to_bytes();
    data_.insert(data_.end(), header_bytes.begin(), header_bytes.end());
  }

  // Parse header from packet
  template <typename HeaderType>
  bool parse_header(HeaderType &header, size_t offset = 0) const {
    static_assert(std::is_base_of_v<dsl::HeaderBase, HeaderType>,
                  "HeaderType must inherit from HeaderBase");

    if (offset >= data_.size()) {
      return false;
    }

    std::vector<uint8_t> header_data(data_.begin() + offset, data_.end());
    return header.from_bytes(header_data);
  }

  // Getters and setters
  const std::vector<uint8_t> &data() const { return data_; }
  void set_data(const std::vector<uint8_t> &data) { data_ = data; }

  std::chrono::system_clock::time_point timestamp() const { return timestamp_; }
  void set_timestamp(std::chrono::system_clock::time_point ts) {
    timestamp_ = ts;
  }

  size_t size() const { return data_.size(); }
  bool empty() const { return data_.empty(); }

  // Add raw bytes
  void append(const std::vector<uint8_t> &bytes) {
    data_.insert(data_.end(), bytes.begin(), bytes.end());
  }

  // Add payload
  void set_payload(const std::vector<uint8_t> &payload) {
    data_.insert(data_.end(), payload.begin(), payload.end());
  }

private:
  std::vector<uint8_t> data_;
  std::chrono::system_clock::time_point timestamp_;
};

// PCAP Writer class
class PcapWriter {
public:
  PcapWriter(const std::string &filename,
             LinkType link_type = LinkType::ETHERNET, uint32_t snaplen = 65535)
      : filename_(filename), link_type_(link_type), snaplen_(snaplen) {}

  ~PcapWriter() { close(); }

  bool open() {
    file_.open(filename_, std::ios::binary | std::ios::out);
    if (!file_.is_open()) {
      return false;
    }

    return write_global_header();
  }

  void close() {
    if (file_.is_open()) {
      file_.close();
    }
  }

  bool write_packet(const Packet &packet) {
    if (!file_.is_open()) {
      return false;
    }

    auto time_point = packet.timestamp();
    auto time_t = std::chrono::system_clock::to_time_t(time_point);
    auto duration = time_point.time_since_epoch();
    auto micros =
        std::chrono::duration_cast<std::chrono::microseconds>(duration) %
        1000000;

    PcapPacketHeader pkt_header;
    pkt_header.ts_sec = static_cast<uint32_t>(time_t);
    pkt_header.ts_usec = static_cast<uint32_t>(micros.count());
    pkt_header.incl_len = static_cast<uint32_t>(
        std::min(packet.size(), static_cast<size_t>(snaplen_)));
    pkt_header.orig_len = static_cast<uint32_t>(packet.size());

    // Write packet header
    file_.write(reinterpret_cast<const char *>(&pkt_header),
                sizeof(pkt_header));

    // Write packet data
    size_t bytes_to_write =
        std::min(packet.size(), static_cast<size_t>(snaplen_));
    file_.write(reinterpret_cast<const char *>(packet.data().data()),
                bytes_to_write);

    return file_.good();
  }

  bool is_open() const { return file_.is_open(); }

private:
  bool write_global_header() {
    PcapGlobalHeader global_header;
    global_header.magic_number = PCAP_MAGIC_NUMBER;
    global_header.version_major = PCAP_VERSION_MAJOR;
    global_header.version_minor = PCAP_VERSION_MINOR;
    global_header.thiszone = 0;
    global_header.sigfigs = 0;
    global_header.snaplen = snaplen_;
    global_header.network = static_cast<uint32_t>(link_type_);

    file_.write(reinterpret_cast<const char *>(&global_header),
                sizeof(global_header));
    return file_.good();
  }

  std::string filename_;
  LinkType link_type_;
  uint32_t snaplen_;
  std::ofstream file_;
};

// PCAP Reader class
class PcapReader {
public:
  PcapReader(const std::string &filename) : filename_(filename) {}

  ~PcapReader() { close(); }

  bool open() {
    file_.open(filename_, std::ios::binary | std::ios::in);
    if (!file_.is_open()) {
      return false;
    }

    return read_global_header();
  }

  void close() {
    if (file_.is_open()) {
      file_.close();
    }
  }

  bool read_packet(Packet &packet) {
    if (!file_.is_open() || file_.eof()) {
      return false;
    }

    PcapPacketHeader pkt_header;
    file_.read(reinterpret_cast<char *>(&pkt_header), sizeof(pkt_header));

    if (file_.gcount() != sizeof(pkt_header)) {
      return false; // End of file or error
    }

    // Handle byte order if needed
    if (swapped_) {
      pkt_header.ts_sec = swap_bytes(pkt_header.ts_sec);
      pkt_header.ts_usec = swap_bytes(pkt_header.ts_usec);
      pkt_header.incl_len = swap_bytes(pkt_header.incl_len);
      pkt_header.orig_len = swap_bytes(pkt_header.orig_len);
    }

    // Read packet data
    std::vector<uint8_t> data(pkt_header.incl_len);
    file_.read(reinterpret_cast<char *>(data.data()), pkt_header.incl_len);

    if (static_cast<uint32_t>(file_.gcount()) != pkt_header.incl_len) {
      return false;
    }

    // Create timestamp
    auto time_point = std::chrono::system_clock::from_time_t(pkt_header.ts_sec);
    time_point += std::chrono::microseconds(pkt_header.ts_usec);

    packet = Packet(data, time_point);
    return true;
  }

  bool is_open() const { return file_.is_open(); }

  LinkType get_link_type() const { return link_type_; }

  uint32_t get_snaplen() const { return snaplen_; }

private:
  bool read_global_header() {
    PcapGlobalHeader global_header;
    file_.read(reinterpret_cast<char *>(&global_header), sizeof(global_header));

    if (file_.gcount() != sizeof(global_header)) {
      return false;
    }

    // Check magic number and detect byte order
    if (global_header.magic_number == PCAP_MAGIC_NUMBER) {
      swapped_ = false;
    } else if (global_header.magic_number == PCAP_MAGIC_NUMBER_SWAPPED) {
      swapped_ = true;
      global_header.version_major = swap_bytes(global_header.version_major);
      global_header.version_minor = swap_bytes(global_header.version_minor);
      global_header.snaplen = swap_bytes(global_header.snaplen);
      global_header.network = swap_bytes(global_header.network);
    } else {
      return false; // Invalid magic number
    }

    link_type_ = static_cast<LinkType>(global_header.network);
    snaplen_ = global_header.snaplen;

    return true;
  }

  template <typename T> T swap_bytes(T value) {
    static_assert(sizeof(T) == 2 || sizeof(T) == 4,
                  "Only 16-bit and 32-bit types supported");

    if constexpr (sizeof(T) == 2) {
      return ((value & 0xFF) << 8) | ((value >> 8) & 0xFF);
    } else if constexpr (sizeof(T) == 4) {
      return ((value & 0xFF) << 24) | (((value >> 8) & 0xFF) << 16) |
             (((value >> 16) & 0xFF) << 8) | ((value >> 24) & 0xFF);
    }
    return value;
  }

  std::string filename_;
  LinkType link_type_;
  uint32_t snaplen_;
  bool swapped_ = false;
  std::ifstream file_;
};

// Utility functions for creating common packet types
namespace utils {

// Print packet information
void print_packet_info(const Packet &packet);

// Print hex dump of packet data
void hex_dump(const Packet &packet, size_t max_bytes = 0);

// Print hex dump in specific format (8-digit offset, 4-byte groups, ASCII)
void hex_dump_formatted(const Packet &packet, size_t max_bytes = 0);

// Print hex dump of raw data in specific format
void hex_dump_data(const std::vector<uint8_t> &data, size_t max_bytes = 0);

// Print hex dump of raw data pointer in specific format
void hex_dump_data(const uint8_t *data, size_t data_size, size_t max_bytes = 0);

// Create a sample packet for testing
Packet create_sample_packet();

// Create an Ethernet packet
inline Packet create_ethernet_packet(const dsl::EthernetHeader &eth_header,
                                     const std::vector<uint8_t> &payload = {}) {
  Packet packet;
  packet.add_header(eth_header);
  if (!payload.empty()) {
    packet.set_payload(payload);
  }
  return packet;
}

// Create a UDP packet with Ethernet and IPv4 headers
inline Packet create_udp_packet(const dsl::EthernetHeader &eth_header,
                                const dsl::UDPHeader &udp_header,
                                const std::vector<uint8_t> &payload = {}) {
  Packet packet;

  // Add Ethernet header
  packet.add_header(eth_header);

  // Create IPv4 header
  dsl::IPv4Header ip;
  ip.set_src_ip("192.168.1.1");
  ip.set_dst_ip("192.168.1.100");
  ip.set_protocol(17); // UDP protocol
  ip.set_total_length(20 + 8 +
                      payload.size()); // IP header + UDP header + payload
  ip.set_identification(0x1234);
  ip.update_computed_fields();
  packet.add_header(ip);

  // Add UDP header
  packet.add_header(udp_header);

  if (!payload.empty()) {
    packet.set_payload(payload);
  }
  return packet;
}

// Create a TCP packet with Ethernet and IPv4 headers
inline Packet create_tcp_packet(const dsl::EthernetHeader &eth_header,
                                const dsl::TCPHeader &tcp_header,
                                const std::vector<uint8_t> &payload = {}) {
  Packet packet;

  // Add Ethernet header
  packet.add_header(eth_header);

  // Create IPv4 header
  dsl::IPv4Header ip;
  ip.set_src_ip("192.168.1.1");
  ip.set_dst_ip("192.168.1.100");
  ip.set_protocol(6); // TCP protocol
  ip.set_total_length(20 + 20 +
                      payload.size()); // IP header + TCP header + payload
  ip.set_identification(0x5678);
  ip.update_computed_fields();
  packet.add_header(ip);

  // Add TCP header
  packet.add_header(tcp_header);

  if (!payload.empty()) {
    packet.set_payload(payload);
  }
  return packet;
}

// Packet Decoder - Automatically decode Ethernet-based packets
struct DecodedPacket {
  // Layer information
  bool has_ethernet = false;
  bool has_ipv4 = false;
  bool has_udp = false;
  bool has_tcp = false;

  // Parsed headers
  dsl::EthernetHeader ethernet;
  dsl::IPv4Header ipv4;
  dsl::UDPHeader udp;
  dsl::TCPHeader tcp;

  // Payload and metadata
  std::vector<uint8_t> payload;
  size_t payload_offset = 0;

  // Error information
  bool decode_error = false;
  std::string error_message;

  // Helper methods
  bool is_ipv4_packet() const { return has_ethernet && has_ipv4; }
  bool is_udp_packet() const { return is_ipv4_packet() && has_udp; }
  bool is_tcp_packet() const { return is_ipv4_packet() && has_tcp; }

  std::string get_protocol_string() const {
    if (is_tcp_packet())
      return "TCP";
    if (is_udp_packet())
      return "UDP";
    if (is_ipv4_packet())
      return "IPv4";
    if (has_ethernet)
      return "Ethernet";
    return "Unknown";
  }

  // Get source/destination info as strings
  std::string get_src_ip() const {
    if (!has_ipv4)
      return "";
    uint32_t ip = ipv4.src_ip();
    return std::to_string((ip >> 24) & 0xFF) + "." +
           std::to_string((ip >> 16) & 0xFF) + "." +
           std::to_string((ip >> 8) & 0xFF) + "." + std::to_string(ip & 0xFF);
  }

  std::string get_dst_ip() const {
    if (!has_ipv4)
      return "";
    uint32_t ip = ipv4.dst_ip();
    return std::to_string((ip >> 24) & 0xFF) + "." +
           std::to_string((ip >> 16) & 0xFF) + "." +
           std::to_string((ip >> 8) & 0xFF) + "." + std::to_string(ip & 0xFF);
  }

  uint16_t get_src_port() const {
    if (is_udp_packet())
      return udp.src_port();
    if (is_tcp_packet())
      return tcp.src_port();
    return 0;
  }

  uint16_t get_dst_port() const {
    if (is_udp_packet())
      return udp.dst_port();
    if (is_tcp_packet())
      return tcp.dst_port();
    return 0;
  }
};

// Decode an Ethernet-based packet
inline DecodedPacket decode_packet(const Packet &packet) {
  DecodedPacket decoded;
  const auto &data = packet.data();
  size_t offset = 0;

  // Check minimum size for Ethernet header
  if (data.size() < 14) {
    decoded.decode_error = true;
    decoded.error_message = "Packet too small for Ethernet header";
    return decoded;
  }

  // Parse Ethernet header
  if (!packet.parse_header(decoded.ethernet, offset)) {
    decoded.decode_error = true;
    decoded.error_message = "Failed to parse Ethernet header";
    return decoded;
  }
  decoded.has_ethernet = true;
  offset += 14; // Ethernet header size

  // Check if it's IPv4
  if (decoded.ethernet.ethertype() == dsl::EtherType::IPv4) {
    // Check minimum size for IPv4 header
    if (data.size() < offset + 20) {
      decoded.decode_error = true;
      decoded.error_message = "Packet too small for IPv4 header";
      return decoded;
    }

    // Parse IPv4 header
    if (!packet.parse_header(decoded.ipv4, offset)) {
      decoded.decode_error = true;
      decoded.error_message = "Failed to parse IPv4 header";
      return decoded;
    }
    decoded.has_ipv4 = true;

    // Get IPv4 header length (IHL * 4 bytes)
    size_t ipv4_header_len = decoded.ipv4.ihl() * 4;
    offset += ipv4_header_len;

    // Check protocol field for UDP or TCP
    uint8_t protocol = decoded.ipv4.protocol();

    if (protocol == 17) { // UDP
      // Check minimum size for UDP header
      if (data.size() < offset + 8) {
        decoded.decode_error = true;
        decoded.error_message = "Packet too small for UDP header";
        return decoded;
      }

      // Parse UDP header
      if (!packet.parse_header(decoded.udp, offset)) {
        decoded.decode_error = true;
        decoded.error_message = "Failed to parse UDP header";
        return decoded;
      }
      decoded.has_udp = true;
      offset += 8; // UDP header size

    } else if (protocol == 6) { // TCP
      // Check minimum size for TCP header
      if (data.size() < offset + 20) {
        decoded.decode_error = true;
        decoded.error_message = "Packet too small for TCP header";
        return decoded;
      }

      // Parse TCP header
      if (!packet.parse_header(decoded.tcp, offset)) {
        decoded.decode_error = true;
        decoded.error_message = "Failed to parse TCP header";
        return decoded;
      }
      decoded.has_tcp = true;

      // Get TCP header length (data offset * 4 bytes)
      size_t tcp_header_len = decoded.tcp.data_offset() * 4;
      offset += tcp_header_len;
    }
  }

  // Extract payload
  decoded.payload_offset = offset;
  if (offset < data.size()) {
    decoded.payload.assign(data.begin() + offset, data.end());
  }

  return decoded;
}

// Print decoded packet information
inline void print_decoded_packet(const DecodedPacket &decoded) {
  std::cout << "=== Decoded Packet Information ===" << std::endl;

  if (decoded.decode_error) {
    std::cout << "❌ Decode Error: " << decoded.error_message << std::endl;
    return;
  }

  std::cout << "Protocol Stack: " << decoded.get_protocol_string() << std::endl;

  if (decoded.has_ethernet) {
    std::cout << "┌─ Ethernet Header:" << std::endl;
    std::cout << "│  Dst MAC: " << std::hex << decoded.ethernet.dst_mac()
              << std::dec << std::endl;
    std::cout << "│  Src MAC: " << std::hex << decoded.ethernet.src_mac()
              << std::dec << std::endl;
    std::cout << "│  Type: 0x" << std::hex
              << static_cast<uint16_t>(decoded.ethernet.ethertype())
              << std::dec;
    if (decoded.ethernet.ethertype() == dsl::EtherType::IPv4) {
      std::cout << " (IPv4)";
    }
    std::cout << std::endl;
  }

  if (decoded.has_ipv4) {
    std::cout << "├─ IPv4 Header:" << std::endl;
    std::cout << "│  Version: " << static_cast<int>(decoded.ipv4.version())
              << std::endl;
    std::cout << "│  Header Length: "
              << static_cast<int>(decoded.ipv4.ihl() * 4) << " bytes"
              << std::endl;
    std::cout << "│  Total Length: " << decoded.ipv4.total_length()
              << std::endl;
    std::cout << "│  TTL: " << static_cast<int>(decoded.ipv4.ttl())
              << std::endl;
    std::cout << "│  Protocol: " << static_cast<int>(decoded.ipv4.protocol());
    if (decoded.ipv4.protocol() == 17)
      std::cout << " (UDP)";
    else if (decoded.ipv4.protocol() == 6)
      std::cout << " (TCP)";
    std::cout << std::endl;
    std::cout << "│  Src IP: " << decoded.get_src_ip() << std::endl;
    std::cout << "│  Dst IP: " << decoded.get_dst_ip() << std::endl;
    std::cout << "│  Checksum: 0x" << std::hex << decoded.ipv4.header_checksum()
              << std::dec << std::endl;
  }

  if (decoded.has_udp) {
    std::cout << "├─ UDP Header:" << std::endl;
    std::cout << "│  Src Port: " << decoded.udp.src_port() << std::endl;
    std::cout << "│  Dst Port: " << decoded.udp.dst_port() << std::endl;
    std::cout << "│  Length: " << decoded.udp.length() << " bytes" << std::endl;
    std::cout << "│  Checksum: 0x" << std::hex << decoded.udp.checksum()
              << std::dec << std::endl;
  }

  if (decoded.has_tcp) {
    std::cout << "├─ TCP Header:" << std::endl;
    std::cout << "│  Src Port: " << decoded.tcp.src_port() << std::endl;
    std::cout << "│  Dst Port: " << decoded.tcp.dst_port() << std::endl;
    std::cout << "│  Seq Num: " << decoded.tcp.seq_num() << std::endl;
    std::cout << "│  Ack Num: " << decoded.tcp.ack_num() << std::endl;
    std::cout << "│  Flags: ";
    if (decoded.tcp.flag_syn())
      std::cout << "SYN ";
    if (decoded.tcp.flag_ack())
      std::cout << "ACK ";
    if (decoded.tcp.flag_fin())
      std::cout << "FIN ";
    std::cout << std::endl;
    std::cout << "│  Window: " << decoded.tcp.window_size() << std::endl;
  }

  if (!decoded.payload.empty()) {
    std::cout << "└─ Payload: " << decoded.payload.size() << " bytes"
              << std::endl;
    if (decoded.payload.size() <= 64) {
      std::cout << "   Data: ";
      for (size_t i = 0; i < std::min(decoded.payload.size(), size_t(32));
           ++i) {
        std::cout << std::hex << std::setfill('0') << std::setw(2)
                  << static_cast<unsigned>(decoded.payload[i]) << " ";
      }
      if (decoded.payload.size() > 32)
        std::cout << "...";
      std::cout << std::dec << std::endl;

      // Try to print as ASCII if printable
      std::cout << "   ASCII: ";
      for (size_t i = 0; i < std::min(decoded.payload.size(), size_t(32));
           ++i) {
        char c = static_cast<char>(decoded.payload[i]);
        std::cout << (std::isprint(c) ? c : '.');
      }
      if (decoded.payload.size() > 32)
        std::cout << "...";
      std::cout << std::endl;
    }
  } else {
    std::cout << "└─ No payload data" << std::endl;
  }
}

// Decode and print packet in one call
inline void decode_and_print_packet(const Packet &packet) {
  auto decoded = decode_packet(packet);
  print_decoded_packet(decoded);
}

} // namespace utils

} // namespace cppscapy::pcap
