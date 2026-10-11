#include <iostream>
#include "cartridge.hpp"
#include "bus.hpp"
#include "cpu.hpp"

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: gbemu <rom.gb>\n";
        return 1;
    }

    Cartridge cart;
    if (!cart.load(argv[1])) {
        std::cerr << "Failed to load ROM\n";
        return 1;
    }

    std::cout << "Title: " << cart.title() << "\n"
              << "Cartridge type: 0x" << std::hex << int(cart.type()) << "\n"
              << "ROM size: " << std::dec << cart.size() << " bytes\n";
                  Bus bus(cart);
                CPU cpu(bus);
                for (;;) cpu.step();
    return 0;
}