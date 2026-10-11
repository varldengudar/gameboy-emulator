#include "bus.hpp"
#include <iostream>

uint8_t Bus::read(uint16_t addr) const {
    if (addr < 0x8000) return cart.read(addr);        // ROM
    if (addr < 0xA000) return vram[addr - 0x8000];    // video RAM
    if (addr < 0xC000) return 0xFF;                   // cartridge RAM (later)
    if (addr < 0xE000) return wram[addr - 0xC000];    // work RAM
    if (addr < 0xFE00) return wram[addr - 0xE000];    // echo of work RAM
    if (addr < 0xFEA0) return oam[addr - 0xFE00];     // sprite table
    if (addr < 0xFF00) return 0xFF;                   // unusable
    if (addr < 0xFF80) return io[addr - 0xFF00];      // I/O registers
    if (addr < 0xFFFF) return hram[addr - 0xFF80];    // high RAM
    return ie;
}

void Bus::write(uint16_t addr, uint8_t value) {
    if (addr < 0x8000) return;                        // ROM is read-only (MBCs later)
    if (addr < 0xA000) { vram[addr - 0x8000] = value; return; }
    if (addr < 0xC000) return;                        // cartridge RAM (later)
    if (addr < 0xE000) { wram[addr - 0xC000] = value; return; }
    if (addr < 0xFE00) { wram[addr - 0xE000] = value; return; }
    if (addr < 0xFEA0) { oam[addr - 0xFE00] = value; return; }
    if (addr < 0xFF00) return;
    if (addr < 0xFF80) {
        io[addr - 0xFF00] = value;
        // Test ROMs print results over the serial port; this shows them in your terminal
        if (addr == 0xFF02 && value == 0x81) std::cout << static_cast<char>(io[0x01]) << std::flush;
        return;
    }
    if (addr < 0xFFFF) { hram[addr - 0xFF80] = value; return; }
    ie = value;
}