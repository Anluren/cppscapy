#include "header_dsl.h"
#include "pcap_support.h"
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

using namespace cppscapy;

void demo_pcap_writing() {
  std::cout << "=== PCAP Writing Demo ===" << std::endl;

  // Create a PCAP writer
  pcap::PcapWriter writer("demo_output.pcap");

  if (!writer.open()) {
    std::cerr << "Failed to open pcap file for writing" << std::endl;
    return;
  }

  // Create sample packets
  for (int i = 0; i < 5; ++i) {
    // Create Ethernet header
    dsl::EthernetHeader eth;
    eth.set_dst_mac(0x001122334455ULL + i);
    eth.set_src_mac(0x665544332211ULL + i);
    eth.set_ethertype(dsl::EtherType::IPv4);

    // Create UDP header
    dsl::UDPHeader udp;
    udp.set_src_port(12345 + i);
    udp.set_dst_port(80);
    udp.set_payload_size(10);
    udp.update_computed_fields();

    // Create payload
    std::vector<uint8_t> payload;
    for (int j = 0; j < 10; ++j) {
      payload.push_back(static_cast<uint8_t>('A' + (i * 10 + j) % 26));
    }

    // Create packet
    auto packet = pcap::utils::create_udp_packet(eth, udp, payload);

    // Write to pcap file
    if (writer.write_packet(packet)) {
      std::cout << "Wrote packet " << i + 1 << " to pcap file" << std::endl;
    } else {
      std::cerr << "Failed to write packet " << i + 1 << std::endl;
    }

    // Add small delay between packets
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  writer.close();
  std::cout << "PCAP file 'demo_output.pcap' created successfully" << std::endl;
}

void demo_pcap_reading() {
  std::cout << "\n=== PCAP Reading Demo ===" << std::endl;

  // Create a PCAP reader
  pcap::PcapReader reader("demo_output.pcap");

  if (!reader.open()) {
    std::cerr << "Failed to open pcap file for reading" << std::endl;
    return;
  }

  std::cout << "Link type: " << static_cast<int>(reader.get_link_type())
            << std::endl;
  std::cout << "Snaplen: " << reader.get_snaplen() << std::endl;

  pcap::Packet packet;
  int packet_count = 0;

  while (reader.read_packet(packet)) {
    packet_count++;
    std::cout << "\n--- Packet " << packet_count << " ---" << std::endl;

    // Print packet info
    pcap::utils::print_packet_info(packet);

    // Try to parse Ethernet header
    dsl::EthernetHeader eth;
    if (packet.parse_header(eth, 0)) {
      std::cout << "Ethernet Header:" << std::endl;
      std::cout << "  Dst MAC: 0x" << std::hex << eth.dst_mac() << std::dec
                << std::endl;
      std::cout << "  Src MAC: 0x" << std::hex << eth.src_mac() << std::dec
                << std::endl;
      std::cout << "  EtherType: 0x" << std::hex
                << static_cast<uint16_t>(eth.ethertype()) << std::dec
                << std::endl;

      // Try to parse UDP header (assuming it's after Ethernet)
      dsl::UDPHeader udp;
      if (packet.parse_header(udp, 14)) { // 14 bytes for Ethernet header
        std::cout << "UDP Header:" << std::endl;
        std::cout << "  Src Port: " << udp.src_port() << std::endl;
        std::cout << "  Dst Port: " << udp.dst_port() << std::endl;
        std::cout << "  Length: " << udp.length() << std::endl;
        std::cout << "  Checksum: 0x" << std::hex << udp.checksum() << std::dec
                  << std::endl;
      }
    }

    // Print hex dump (first 64 bytes)
    pcap::utils::hex_dump(packet, 64);
  }

  reader.close();
  std::cout << "\nRead " << packet_count << " packets from pcap file"
            << std::endl;
}

void demo_tcp_packet() {
  std::cout << "\n=== TCP Packet Demo ===" << std::endl;

  // Create TCP packet
  dsl::EthernetHeader eth;
  eth.set_dst_mac(0xffffffffffff); // Broadcast
  eth.set_src_mac(0x001122334455);
  eth.set_ethertype(dsl::EtherType::IPv4);

  dsl::TCPHeader tcp;
  tcp.set_src_port(443);
  tcp.set_dst_port(8080);
  tcp.set_seq_num(0x12345678);
  tcp.set_ack_num(0x87654321);
  tcp.set_flag_syn(true);
  tcp.set_flag_ack(true);
  tcp.set_window_size(8192);
  tcp.update_computed_fields();

  std::vector<uint8_t> payload = {'T', 'C', 'P', ' ', 'D', 'a', 't', 'a'};
  auto packet = pcap::utils::create_tcp_packet(eth, tcp, payload);

  std::cout << "Created TCP packet:" << std::endl;
  pcap::utils::print_packet_info(packet);
  pcap::utils::hex_dump(packet, 64);

  // Write single TCP packet to file
  pcap::PcapWriter tcp_writer("tcp_demo.pcap");
  if (tcp_writer.open()) {
    tcp_writer.write_packet(packet);
    tcp_writer.close();
    std::cout << "TCP packet written to 'tcp_demo.pcap'" << std::endl;
  }
}

int main() {
  try {
    std::cout << "CPPScapy PCAP Support Demo" << std::endl;
    std::cout << "==========================" << std::endl;

    // Demo writing packets to pcap
    demo_pcap_writing();

    // Demo reading packets from pcap
    demo_pcap_reading();

    // Demo TCP packet creation
    demo_tcp_packet();

    std::cout << "\nDemo completed successfully!" << std::endl;

  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}
