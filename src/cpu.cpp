#include "cpu.hpp"
#include <cstdio>
#include <cstdlib>

uint8_t CPU::fetch8() {
    return bus.read(pc++);
}

uint16_t CPU::fetch16() {
    uint8_t lo = fetch8();
    uint8_t hi = fetch8();
    return static_cast<uint16_t>((hi << 8) | lo);
}

void CPU::push16(uint16_t v) {
    bus.write(--sp, static_cast<uint8_t>(v >> 8));
    bus.write(--sp, static_cast<uint8_t>(v & 0xFF));
}

uint16_t CPU::pop16() {
    uint8_t lo = bus.read(sp++);
    uint8_t hi = bus.read(sp++);
    return static_cast<uint16_t>((hi << 8) | lo);
}

int CPU::step() {
    uint8_t op = fetch8();

    // LD r, r'  (0x40-0x7F, except 0x76 which is HALT)
    if (op >= 0x40 && op <= 0x7F && op != 0x76) {
        int dst = (op >> 3) & 7;
        int src = op & 7;
        set_r8(dst, get_r8(src));
        return (dst == 6 || src == 6) ? 8 : 4;
    }

    // LD r, n  (0x06, 0x0E, 0x16, ... 0x3E)
    if ((op & 0xC7) == 0x06) {
        int dst = (op >> 3) & 7;
        set_r8(dst, fetch8());
        return dst == 6 ? 12 : 8;
    }

    // INC r  (0x04, 0x0C, ... 0x3C)
    if ((op & 0xC7) == 0x04) {
        int r = (op >> 3) & 7;
        uint8_t v = get_r8(r);
        uint8_t res = v + 1;
        set_r8(r, res);
        set_flag(FLAG_Z, res == 0);
        set_flag(FLAG_N, false);
        set_flag(FLAG_H, (v & 0x0F) == 0x0F);
        return r == 6 ? 12 : 4;
    }

    // DEC r  (0x05, 0x0D, ... 0x3D)
    if ((op & 0xC7) == 0x05) {
        int r = (op >> 3) & 7;
        uint8_t v = get_r8(r);
        uint8_t res = v - 1;
        set_r8(r, res);
        set_flag(FLAG_Z, res == 0);
        set_flag(FLAG_N, true);
        set_flag(FLAG_H, (v & 0x0F) == 0x00);
        return r == 6 ? 12 : 4;
    }

    // JR cc, e  (0x20 NZ, 0x28 Z, 0x30 NC, 0x38 C)
    if ((op & 0xE7) == 0x20) {
        int8_t off = static_cast<int8_t>(fetch8());
        bool take = false;
        switch ((op >> 3) & 3) {
            case 0: take = !(f & FLAG_Z); break;
            case 1: take =  (f & FLAG_Z); break;
            case 2: take = !(f & FLAG_C); break;
            case 3: take =  (f & FLAG_C); break;
        }
    if (take) {
        pc = static_cast<uint16_t>(pc + off);
        return 12;
    }
    return 8;
}

    switch (op) {
        case 0x00: return 4;                         // NOP
        case 0x01: set_bc(fetch16()); return 12;     // LD BC, nn
        case 0x11: set_de(fetch16()); return 12;     // LD DE, nn
        case 0x21: set_hl(fetch16()); return 12;     // LD HL, nn
        case 0x31: sp = fetch16(); return 12;        // LD SP, nn
        case 0xC3: pc = fetch16(); return 16;        // JP nn
        case 0xF3: ime = false; return 4;            // DI
        case 0xEA:                // LD (nn), A
            bus.write(fetch16(), a);
            return 16;
        case 0xE0:               // LDH (n), A  (write A to 0xFF00 + n)
            bus.write(0xFF00 + fetch8(), a);
            return 12;
        case 0xF0:               // LDH A, (n)  (read 0xFF00 + n into A)
            a = bus.read(0xFF00 + fetch8());
            return 12;
        case 0xFA:               // LD A, (nn)
            a = bus.read(fetch16());
            return 16;
        
        case 0x18: {             // JR e
            int8_t off = static_cast<int8_t>(fetch8());
            pc = static_cast<uint16_t>(pc + off);
            return 12;
        }
            
        case 0xCD: {             // CALL nn
            uint16_t addr = fetch16();
            push16(pc);
            pc = addr;
            return 24;
        }
        
        case 0xC9:               // RET
            pc = pop16();
            return 16;

        case 0xC5: push16(bc()); return 16;   // PUSH BC
        case 0xD5: push16(de()); return 16;   // PUSH DE
        case 0xE5: push16(hl()); return 16;   // PUSH HL
        case 0xF5:                            // PUSH AF
            push16(static_cast<uint16_t>((a << 8) | f));
            return 16;

        case 0xC1: set_bc(pop16()); return 12;  // POP BC
        case 0xD1: set_de(pop16()); return 12;  // POP DE
        case 0xE1: set_hl(pop16()); return 12;  // POP HL
        case 0xF1: {                            // POP AF
            uint16_t v = pop16();
            a = static_cast<uint8_t>(v >> 8);
            f = static_cast<uint8_t>(v & 0xF0);  // low 4 bits of F are always 0
            return 12;
}
            
        default:
            std::fprintf(stderr, "Unimplemented opcode 0x%02X at 0x%04X\n",
                         op, static_cast<unsigned>(pc - 1));
            std::exit(1);
    }
}

uint8_t CPU::get_r8(int i) {
    switch (i) {
        case 0: return b;
        case 1: return c;
        case 2: return d;
        case 3: return e;
        case 4: return h;
        case 5: return l;
        case 6: return bus.read(hl());
        default: return a;
    }
}

void CPU::set_r8(int i, uint8_t v) {
    switch (i) {
        case 0: b = v; break;
        case 1: c = v; break;
        case 2: d = v; break;
        case 3: e = v; break;
        case 4: h = v; break;
        case 5: l = v; break;
        case 6: bus.write(hl(), v); break;
        default: a = v; break;
    }
}