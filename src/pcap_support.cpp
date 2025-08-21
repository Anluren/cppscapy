#include "pcap_support.h"
#include <iomanip>
#include <iostream>

namespace cppscapy::pcap {

namespace utils {

// Print packet information
void print_packet_info(const Packet &packet) {
  auto time_point = packet.timestamp();
  auto time_t = std::chrono::system_clock::to_time_t(time_point);
  auto duration = time_point.time_since_epoch();
  auto micros =
      std::chrono::duration_cast<std::chrono::microseconds>(duration) % 1000000;

  std::cout << "Packet: " << packet.size() << " bytes, "
            << "timestamp: "
            << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S")
            << "." << std::setfill('0') << std::setw(6) << micros.count()
            << std::endl;
}

// Print hex dump of packet data
void hex_dump(const Packet &packet, size_t max_bytes) {
  const auto &data = packet.data();
  size_t bytes_to_print =
      (max_bytes == 0) ? data.size() : std::min(data.size(), max_bytes);

  std::cout << "Hex dump (" << bytes_to_print << " bytes):" << std::endl;

  for (size_t i = 0; i < bytes_to_print; i += 16) {
    // Print offset
    std::cout << std::setfill('0') << std::setw(4) << std::hex << i << ": ";

    // Print hex bytes
    for (size_t j = 0; j < 16; ++j) {
      if (i + j < bytes_to_print) {
        std::cout << std::setfill('0') << std::setw(2) << std::hex
                  << static_cast<unsigned int>(data[i + j]) << " ";
      } else {
        std::cout << "   ";
      }

      if (j == 7)
        std::cout << " "; // Extra space in the middle
    }

    std::cout << " |";

    // Print ASCII representation
    for (size_t j = 0; j < 16 && i + j < bytes_to_print; ++j) {
      char c = static_cast<char>(data[i + j]);
      std::cout << (std::isprint(c) ? c : '.');
    }

    std::cout << "|" << std::endl;
  }
  std::cout << std::dec; // Reset to decimal
}

// Create a sample packet for testing
Packet create_sample_packet() {
  // Create Ethernet header
  dsl::EthernetHeader eth;
  eth.set_dst_mac(0x001122334455ULL);
  eth.set_src_mac(0x665544332211ULL);
  eth.set_ethertype(dsl::EtherType::IPv4);

  // Create UDP header
  dsl::UDPHeader udp;
  udp.set_src_port(12345);
  udp.set_dst_port(80);
  udp.set_payload_size(5);
  udp.update_computed_fields();

  // Sample payload
  std::vector<uint8_t> payload = {'H', 'e', 'l', 'l', 'o'};

  return create_udp_packet(eth, udp, payload);
}

} // namespace utils

} // namespace cppscapy::pcap
