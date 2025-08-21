#pragma once

#include "header_dsl.h"
#include <chrono>
#include <cstdint>
#include <fstream>
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

// Create a UDP packet with Ethernet header
inline Packet create_udp_packet(const dsl::EthernetHeader &eth_header,
                                const dsl::UDPHeader &udp_header,
                                const std::vector<uint8_t> &payload = {}) {
  Packet packet;
  packet.add_header(eth_header);
  packet.add_header(udp_header);
  if (!payload.empty()) {
    packet.set_payload(payload);
  }
  return packet;
}

// Create a TCP packet with Ethernet header
inline Packet create_tcp_packet(const dsl::EthernetHeader &eth_header,
                                const dsl::TCPHeader &tcp_header,
                                const std::vector<uint8_t> &payload = {}) {
  Packet packet;
  packet.add_header(eth_header);
  packet.add_header(tcp_header);
  if (!payload.empty()) {
    packet.set_payload(payload);
  }
  return packet;
}

} // namespace utils

} // namespace cppscapy::pcap
