#pragma once
#include <cstdint>
#include <string>
#include <vector>

class Cartridge {
public:
    bool load(const std::string& path);
    uint8_t read(uint16_t addr) const;
    std::string title() const;
    uint8_t type() const { return rom.size() > 0x147 ? rom[0x147] : 0; }
    size_t size() const { return rom.size(); }

private:
    std::vector<uint8_t> rom;
};