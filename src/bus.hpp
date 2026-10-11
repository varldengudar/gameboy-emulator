#pragma once
#include <array>
#include <cstdint>
#include "cartridge.hpp"

class Bus {
public:
    explicit Bus(Cartridge& cart) : cart(cart) {}

    uint8_t read(uint16_t addr) const;
    void write(uint16_t addr, uint8_t value);

private:
    Cartridge& cart;
    std::array<uint8_t, 0x2000> vram{};
    std::array<uint8_t, 0x2000> wram{};
    std::array<uint8_t, 0xA0>   oam{};
    std::array<uint8_t, 0x80>   io{};
    std::array<uint8_t, 0x7F>   hram{};
    uint8_t ie = 0;  // interrupt enable register (0xFFFF)
};