#pragma once

#include <cstdint>
#include <vector>
#include <bitset>
#include <string>
#include <type_traits>

namespace cppscapy::dsl {

// Base class for all generated headers
class HeaderBase {
public:
    virtual ~HeaderBase() = default;
    
    virtual std::vector<uint8_t> to_bytes() const = 0;
    virtual bool from_bytes(const std::vector<uint8_t>& data) = 0;
    virtual size_t size_bits() const = 0;
    virtual size_t size_bytes() const { return (size_bits() + 7) / 8; }
    virtual bool is_valid() const = 0;
    virtual void update_computed_fields() = 0;
};

// Bit field manipulation utilities
template<typename T>
class BitField {
public:
    BitField(std::vector<uint8_t>& data, size_t bit_offset, size_t bit_width)
        : data_(data), bit_offset_(bit_offset), bit_width_(bit_width) {}
    
    T get() const {
        T value = 0;
        for (size_t i = 0; i < bit_width_; ++i) {
            size_t byte_idx = (bit_offset_ + i) / 8;
            size_t bit_idx = (bit_offset_ + i) % 8;
            
            if (byte_idx < data_.size()) {
                uint8_t bit = (data_[byte_idx] >> (7 - bit_idx)) & 1;
                value |= (static_cast<T>(bit) << (bit_width_ - 1 - i));
            }
        }
        return value;
    }
    
    void set(T value) {
        // Ensure data vector is large enough
        size_t required_bytes = (bit_offset_ + bit_width_ + 7) / 8;
        if (data_.size() < required_bytes) {
            data_.resize(required_bytes, 0);
        }
        
        for (size_t i = 0; i < bit_width_; ++i) {
            size_t byte_idx = (bit_offset_ + i) / 8;
            size_t bit_idx = (bit_offset_ + i) % 8;
            
            uint8_t bit = (value >> (bit_width_ - 1 - i)) & 1;
            
            // Clear the bit first
            data_[byte_idx] &= ~(1 << (7 - bit_idx));
            // Set the bit if needed
            data_[byte_idx] |= (bit << (7 - bit_idx));
        }
    }
    
    operator T() const { return get(); }
    BitField& operator=(T value) { set(value); return *this; }
    
private:
    std::vector<uint8_t>& data_;
    size_t bit_offset_;
    size_t bit_width_;
};

// Enum support
template<typename T, typename UnderlyingType = uint16_t>
class EnumField {
public:
    EnumField(std::vector<uint8_t>& data, size_t bit_offset, size_t bit_width)
        : field_(data, bit_offset, bit_width) {}
    
    T get() const { return static_cast<T>(field_.get()); }
    void set(T value) { field_.set(static_cast<UnderlyingType>(value)); }
    
    operator T() const { return get(); }
    EnumField& operator=(T value) { set(value); return *this; }
    
private:
    BitField<UnderlyingType> field_;
};

// Example generated header classes
enum class EtherType : uint16_t {
    IPv4 = 0x0800,
    IPv6 = 0x86DD,
    ARP = 0x0806,
    MPLS = 0x8847,
    VLAN = 0x8100
};

enum class IPProtocol : uint8_t {
    ICMP = 1,
    TCP = 6,
    UDP = 17,
    OSPF = 89
};

// Generated from DSL: header EthernetHeader { dst_mac: 48; src_mac: 48; ethertype: 16; }
class EthernetHeader : public HeaderBase {
public:
    EthernetHeader() : data_(14, 0) {}
    
    // Field accessors
    uint64_t dst_mac() const { return BitField<uint64_t>(const_cast<std::vector<uint8_t>&>(data_), 0, 48).get(); }
    void set_dst_mac(uint64_t value) { BitField<uint64_t>(data_, 0, 48).set(value); }
    
    uint64_t src_mac() const { return BitField<uint64_t>(const_cast<std::vector<uint8_t>&>(data_), 48, 48).get(); }
    void set_src_mac(uint64_t value) { BitField<uint64_t>(data_, 48, 48).set(value); }
    
    EtherType ethertype() const { return static_cast<EtherType>(BitField<uint16_t>(const_cast<std::vector<uint8_t>&>(data_), 96, 16).get()); }
    void set_ethertype(EtherType value) { BitField<uint16_t>(data_, 96, 16).set(static_cast<uint16_t>(value)); }
    
    // HeaderBase implementation
    std::vector<uint8_t> to_bytes() const override { return data_; }
    
    bool from_bytes(const std::vector<uint8_t>& data) override {
        if (data.size() < 14) return false;
        data_ = data;
        data_.resize(14);
        return true;
    }
    
    size_t size_bits() const override { return 112; } // 48 + 48 + 16
    bool is_valid() const override { return data_.size() == 14; }
    void update_computed_fields() override {} // No computed fields
    
private:
    std::vector<uint8_t> data_;
};

// Generated from DSL: header UDPHeader with computed fields
class UDPHeader : public HeaderBase {
public:
    UDPHeader() : data_(8, 0), payload_size_(0) {}
    
    // Field accessors
    uint16_t src_port() const { return BitField<uint16_t>(const_cast<std::vector<uint8_t>&>(data_), 0, 16).get(); }
    void set_src_port(uint16_t value) { BitField<uint16_t>(data_, 0, 16).set(value); }
    
    uint16_t dst_port() const { return BitField<uint16_t>(const_cast<std::vector<uint8_t>&>(data_), 16, 16).get(); }
    void set_dst_port(uint16_t value) { BitField<uint16_t>(data_, 16, 16).set(value); }
    
    uint16_t length() const { return BitField<uint16_t>(const_cast<std::vector<uint8_t>&>(data_), 32, 16).get(); }
    void set_payload_size(uint16_t size) { payload_size_ = size; }
    
    uint16_t checksum() const { return BitField<uint16_t>(const_cast<std::vector<uint8_t>&>(data_), 48, 16).get(); }
    
    // HeaderBase implementation
    std::vector<uint8_t> to_bytes() const override { return data_; }
    
    bool from_bytes(const std::vector<uint8_t>& data) override {
        if (data.size() < 8) return false;
        data_ = data;
        data_.resize(8);
        return true;
    }
    
    size_t size_bits() const override { return 64; } // 16 + 16 + 16 + 16
    bool is_valid() const override { return data_.size() == 8; }
    
    void update_computed_fields() override {
        // Update length field
        BitField<uint16_t>(data_, 32, 16).set(8 + payload_size_);
        // TODO: Calculate checksum
        BitField<uint16_t>(data_, 48, 16).set(0); // Placeholder
    }
    
private:
    std::vector<uint8_t> data_;
    uint16_t payload_size_;
};

// TCP flags as bit field structure
struct TCPFlags {
    bool ns : 1;    // ECN-nonce
    bool cwr : 1;   // Congestion Window Reduced  
    bool ece : 1;   // ECN-Echo
    bool urg : 1;   // Urgent
    bool ack : 1;   // Acknowledgment
    bool psh : 1;   // Push
    bool rst : 1;   // Reset
    bool syn : 1;   // Synchronize
    bool fin : 1;   // Finish
};

// Generated from complex DSL with flag fields
class TCPHeader : public HeaderBase {
public:
    TCPHeader() : data_(20, 0) {}
    
    uint16_t src_port() const { return BitField<uint16_t>(const_cast<std::vector<uint8_t>&>(data_), 0, 16).get(); }
    void set_src_port(uint16_t value) { BitField<uint16_t>(data_, 0, 16).set(value); }
    
    uint16_t dst_port() const { return BitField<uint16_t>(const_cast<std::vector<uint8_t>&>(data_), 16, 16).get(); }
    void set_dst_port(uint16_t value) { BitField<uint16_t>(data_, 16, 16).set(value); }
    
    uint32_t seq_num() const { return BitField<uint32_t>(const_cast<std::vector<uint8_t>&>(data_), 32, 32).get(); }
    void set_seq_num(uint32_t value) { BitField<uint32_t>(data_, 32, 32).set(value); }
    
    uint32_t ack_num() const { return BitField<uint32_t>(const_cast<std::vector<uint8_t>&>(data_), 64, 32).get(); }
    void set_ack_num(uint32_t value) { BitField<uint32_t>(data_, 64, 32).set(value); }
    
    uint8_t data_offset() const { return BitField<uint8_t>(const_cast<std::vector<uint8_t>&>(data_), 96, 4).get(); }
    void set_data_offset(uint8_t value) { BitField<uint8_t>(data_, 96, 4).set(value); }
    
    // Flag accessors
    bool flag_syn() const { return BitField<uint8_t>(const_cast<std::vector<uint8_t>&>(data_), 110, 1).get(); }
    void set_flag_syn(bool value) { BitField<uint8_t>(data_, 110, 1).set(value); }
    
    bool flag_ack() const { return BitField<uint8_t>(const_cast<std::vector<uint8_t>&>(data_), 107, 1).get(); }
    void set_flag_ack(bool value) { BitField<uint8_t>(data_, 107, 1).set(value); }
    
    bool flag_fin() const { return BitField<uint8_t>(const_cast<std::vector<uint8_t>&>(data_), 111, 1).get(); }
    void set_flag_fin(bool value) { BitField<uint8_t>(data_, 111, 1).set(value); }
    
    uint16_t window_size() const { return BitField<uint16_t>(const_cast<std::vector<uint8_t>&>(data_), 112, 16).get(); }
    void set_window_size(uint16_t value) { BitField<uint16_t>(data_, 112, 16).set(value); }
    
    uint16_t checksum() const { return BitField<uint16_t>(const_cast<std::vector<uint8_t>&>(data_), 128, 16).get(); }
    
    uint16_t urgent_ptr() const { return BitField<uint16_t>(const_cast<std::vector<uint8_t>&>(data_), 144, 16).get(); }
    void set_urgent_ptr(uint16_t value) { BitField<uint16_t>(data_, 144, 16).set(value); }
    
    // HeaderBase implementation
    std::vector<uint8_t> to_bytes() const override { return data_; }
    
    bool from_bytes(const std::vector<uint8_t>& data) override {
        if (data.size() < 20) return false;
        data_ = data;
        if (data_.size() > 20) data_.resize(20); // Basic header only for now
        return true;
    }
    
    size_t size_bits() const override { return 160; } // 20 bytes
    bool is_valid() const override { 
        return data_.size() >= 20 && data_offset() >= 5; 
    }
    
    void update_computed_fields() override {
        // Set default data offset if not set
        if (data_offset() == 0) {
            set_data_offset(5); // 20 bytes = 5 * 4-byte words
        }
        // TODO: Calculate checksum
        BitField<uint16_t>(data_, 128, 16).set(0); // Placeholder
    }
    
private:
    std::vector<uint8_t> data_;
};

} // namespace cppscapy::dsl
