#include "cartridge.hpp"
#include <fstream>

bool Cartridge::load(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) return false;
    rom.assign(std::istreambuf_iterator<char>(file), {});
    return rom.size() >= 0x150;  // must at least contain the header
}

uint8_t Cartridge::read(uint16_t addr) const {
    return addr < rom.size() ? rom[addr] : 0xFF;
}

std::string Cartridge::title() const {
    std::string t;
    for (uint16_t i = 0x134; i < 0x144; ++i) {
        if (rom[i] == 0) break;
        t += static_cast<char>(rom[i]);
    }
    return t;
}