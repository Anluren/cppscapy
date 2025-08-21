#include "header_dsl.h"
#include "pcap_support.h"
#include <iostream>

using namespace cppscapy;

int main() {
  std::cout << "CPPScapy Simple PCAP Example" << std::endl;
  std::cout << "=============================" << std::endl;

  // Create a simple UDP packet
  dsl::EthernetHeader eth;
  eth.set_dst_mac(0x001122334455ULL);
  eth.set_src_mac(0x665544332211ULL);
  eth.set_ethertype(dsl::EtherType::IPv4);

  dsl::UDPHeader udp;
  udp.set_src_port(12345);
  udp.set_dst_port(80);
  udp.set_payload_size(13); // "Hello, World!" length
  udp.update_computed_fields();

  std::vector<uint8_t> payload = {'H', 'e', 'l', 'l', 'o', ',', ' ',
                                  'W', 'o', 'r', 'l', 'd', '!'};
  auto packet = pcap::utils::create_udp_packet(eth, udp, payload);

  // Write to pcap file
  std::cout << "\n1. Writing packet to 'simple_example.pcap'..." << std::endl;
  pcap::PcapWriter writer("simple_example.pcap");
  if (writer.open()) {
    writer.write_packet(packet);
    writer.close();
    std::cout << "   Success!" << std::endl;
  } else {
    std::cout << "   Failed to open file for writing" << std::endl;
    return 1;
  }

  // Read back from pcap file
  std::cout << "\n2. Reading packet from 'simple_example.pcap'..." << std::endl;
  pcap::PcapReader reader("simple_example.pcap");
  if (reader.open()) {
    pcap::Packet read_packet;
    if (reader.read_packet(read_packet)) {
      std::cout << "   Successfully read packet!" << std::endl;
      std::cout << "   Packet size: " << read_packet.size() << " bytes"
                << std::endl;

      // Parse and display headers
      dsl::EthernetHeader read_eth;
      if (read_packet.parse_header(read_eth, 0)) {
        std::cout << "   Ethernet - Src MAC: 0x" << std::hex
                  << read_eth.src_mac() << ", Dst MAC: 0x" << read_eth.dst_mac()
                  << std::dec << std::endl;

        dsl::UDPHeader read_udp;
        if (read_packet.parse_header(read_udp, 14)) {
          std::cout << "   UDP - Port " << read_udp.src_port() << " -> "
                    << read_udp.dst_port() << ", Length: " << read_udp.length()
                    << std::endl;
        }
      }

      // Extract and display payload
      const auto &data = read_packet.data();
      if (data.size() >= 27) { // Ethernet(14) + UDP(8) + payload(5)
        std::cout << "   Payload: ";
        for (size_t i = 22; i < data.size(); ++i) { // Skip headers
          std::cout << static_cast<char>(data[i]);
        }
        std::cout << std::endl;
      }
    } else {
      std::cout << "   Failed to read packet" << std::endl;
      return 1;
    }
    reader.close();
  } else {
    std::cout << "   Failed to open file for reading" << std::endl;
    return 1;
  }

  std::cout << "\n3. Displaying hex dump..." << std::endl;
  pcap::utils::hex_dump(packet);

  std::cout << "\nExample completed successfully!" << std::endl;
  std::cout << "You can now open 'simple_example.pcap' in Wireshark or analyze "
               "with tcpdump."
            << std::endl;

  return 0;
}
