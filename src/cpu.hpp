#pragma once
#include <cstdint>
#include "bus.hpp"

class CPU {
public:
    explicit CPU(Bus& bus) : bus(bus) {}

    int step();  // runs one instruction, returns how many cycles it took

private:
    uint8_t  fetch8();
    uint16_t fetch16();

    Bus& bus;

    // 8-bit registers (values after the Game Boy boot ROM finishes)
    uint8_t a = 0x01, f = 0xB0;
    uint8_t b = 0x00, c = 0x13;
    uint8_t d = 0x00, e = 0xD8;
    uint8_t h = 0x01, l = 0x4D;

    uint16_t sp = 0xFFFE;
    uint16_t pc = 0x0100;  // execution starts here
    
    
    bool ime = false;

    // Flag bits inside register f
    static constexpr uint8_t FLAG_Z = 0x80;
    static constexpr uint8_t FLAG_N = 0x40;
    static constexpr uint8_t FLAG_H = 0x20;
    static constexpr uint8_t FLAG_C = 0x10;

    uint16_t hl() const { return static_cast<uint16_t>((h << 8) | l); }
    uint16_t bc() const { return static_cast<uint16_t>((b << 8) | c); }
    uint16_t de() const { return static_cast<uint16_t>((d << 8) | e); }
    void set_hl(uint16_t v) { h = v >> 8; l = v & 0xFF; }
    void set_bc(uint16_t v) { b = v >> 8; c = v & 0xFF; }
    void set_de(uint16_t v) { d = v >> 8; e = v & 0xFF; }

    void set_flag(uint8_t flag, bool on) { if (on) f |= flag; else f &= ~flag; }

    uint8_t get_r8(int i);
    void set_r8(int i, uint8_t v);
    void push16(uint16_t v);
    uint16_t pop16();
};