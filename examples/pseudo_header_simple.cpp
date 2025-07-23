#include <iostream>
#include <iomanip>
#include <vector>
#include <cstdint>

// Simple demonstration of why UDP checksum includes IP addresses
class SimpleDemo {
public:
    // Simulate simple checksum calculation (not the real algorithm)
    static uint16_t simple_checksum(const std::vector<uint8_t>& data) {
        uint32_t sum = 0;
        for (size_t i = 0; i < data.size(); i += 2) {
            if (i + 1 < data.size()) {
                sum += (data[i] << 8) | data[i + 1];
            } else {
                sum += data[i] << 8;
            }
        }
        while (sum >> 16) {
            sum = (sum & 0xFFFF) + (sum >> 16);
        }
        return ~sum;
    }
    
    static void demonstrate() {
        std::cout << "=== Why UDP Checksum Includes IP Header Info ===\n\n";
        
        // Simulate UDP header + payload (same for both scenarios)
        std::vector<uint8_t> udp_data = {
            0x30, 0x39,  // Source port 12345
            0x00, 0x50,  // Dest port 80
            0x00, 0x11,  // Length 17 (8 header + 9 payload)
            0x00, 0x00,  // Checksum (set to 0 for calculation)
            'H', 'e', 'l', 'l', 'o', ' ', 'U', 'D', 'P'  // Payload
        };
        
        std::cout << "Scenario 1: Correct destination\n";
        std::cout << "UDP Header + Payload: ";
        for (auto byte : udp_data) {
            std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)byte << " ";
        }
        std::cout << std::dec << "\n";
        
        // Scenario 1: Include correct IP addresses in checksum
        std::vector<uint8_t> data_with_correct_ips = {
            // Pseudo-header for 192.168.1.10 -> 192.168.1.20
            192, 168, 1, 10,   // Source IP
            192, 168, 1, 20,   // Dest IP
            0, 17,             // Zero + Protocol (UDP=17)
            0, 17              // UDP Length
        };
        data_with_correct_ips.insert(data_with_correct_ips.end(), udp_data.begin(), udp_data.end());
        
        uint16_t correct_checksum = simple_checksum(data_with_correct_ips);
        std::cout << "Source IP: 192.168.1.10, Dest IP: 192.168.1.20\n";
        std::cout << "Checksum: 0x" << std::hex << std::setw(4) << std::setfill('0') << correct_checksum << std::dec << "\n\n";
        
        // Scenario 2: Same UDP data, but wrong destination IP
        std::cout << "Scenario 2: Misdelivered packet (wrong destination)\n";
        std::cout << "UDP Header + Payload: ";
        for (auto byte : udp_data) {
            std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)byte << " ";
        }
        std::cout << std::dec << " (IDENTICAL!)\n";
        
        std::vector<uint8_t> data_with_wrong_ip = {
            // Pseudo-header for 192.168.1.10 -> 192.168.1.30 (WRONG!)
            192, 168, 1, 10,   // Source IP (same)
            192, 168, 1, 30,   // Dest IP (DIFFERENT!)
            0, 17,             // Zero + Protocol (UDP=17)
            0, 17              // UDP Length
        };
        data_with_wrong_ip.insert(data_with_wrong_ip.end(), udp_data.begin(), udp_data.end());
        
        uint16_t wrong_checksum = simple_checksum(data_with_wrong_ip);
        std::cout << "Source IP: 192.168.1.10, Dest IP: 192.168.1.30 (WRONG!)\n";
        std::cout << "Checksum: 0x" << std::hex << std::setw(4) << std::setfill('0') << wrong_checksum << std::dec << "\n\n";
        
        // Analysis
        std::cout << "=== Analysis ===\n";
        std::cout << "✓ UDP header and payload are IDENTICAL\n";
        std::cout << "✓ Only destination IP differs\n";
        std::cout << "✓ Checksums are DIFFERENT: 0x" << std::hex << correct_checksum 
                  << " vs 0x" << wrong_checksum << std::dec << "\n";
        std::cout << "✓ Receiver can detect misdelivered packet!\n\n";
        
        // What happens without IP addresses
        std::cout << "=== Without IP Addresses in Checksum ===\n";
        uint16_t udp_only_checksum = simple_checksum(udp_data);
        std::cout << "Checksum of UDP header + payload only: 0x" 
                  << std::hex << std::setw(4) << std::setfill('0') << udp_only_checksum << std::dec << "\n";
        std::cout << "❌ Both correct and misdelivered packets would have same checksum!\n";
        std::cout << "❌ No way to detect the delivery error!\n\n";
    }
};

int main() {
    SimpleDemo::demonstrate();
    
    std::cout << "=== Key Takeaways ===\n";
    std::cout << "1. UDP pseudo-header includes IP addresses for good reason\n";
    std::cout << "2. Protects against misdelivered packets\n";
    std::cout << "3. Provides end-to-end integrity across network layers\n";
    std::cout << "4. Helps detect source IP spoofing\n";
    std::cout << "5. Essential for reliable networking despite seeming 'wrong'\n\n";
    
    std::cout << "The pseudo-header is NOT sent on the wire - it's only used\n";
    std::cout << "for checksum calculation by both sender and receiver.\n";
    
    return 0;
}
