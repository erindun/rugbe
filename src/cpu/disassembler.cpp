#include <iostream>
#include <iomanip>
#include <bitset>
#include "cpu.hpp"

using std::cout;
using std::endl;

const bool DEBUG_MODE = true;

// Helper classes to provide proper hexadecimal formatting
// This is done specifically for each int type rather than using a template
// because when using a template, signed ints may output strangely
// due to sign extension.
struct U16Helper {
    uint16_t u16;

    U16Helper(uint16_t u16) : u16 {u16} {}
};
struct U8Helper {
    uint8_t u8;

    U8Helper(uint8_t u8) : u8 {u8} {}
};
struct I8Helper {
    int8_t i8;

    I8Helper(int8_t i8) : i8 {i8} {}
};

// Formatting for u16, u8, and i8 in hex
inline std::ostream& operator<<(std::ostream& out, const U16Helper& h) {
    return out << "$" << std::hex << std::setw(4) << std::setfill('0')
               << static_cast<int>(h.u16);
}
inline std::ostream& operator<<(std::ostream& out, const U8Helper& h) {
    return out << "$" << std::hex << std::setw(4) << std::setfill('0')
               << static_cast<int>(h.u8);
}
inline std::ostream& operator<<(std::ostream& out, const I8Helper& h) {
    return out << "$" << std::hex << std::setw(4) << std::setfill('0')
               << static_cast<int>(h.i8);
}

// Converts value into its respective type's helper class
// so that it will be proper formatted by cout
U16Helper hex(uint16_t u16) {
    return U16Helper(u16);
}
U8Helper hex(uint8_t u8) {
    return U8Helper(u8);
}
U8Helper hex(int8_t i8) {
    return U8Helper(i8);
}

void Cpu::disassemble_op() {
    // Because get_n(), get_nn(), and get_i() change the value of PC,
    // they are not reused here.

    // Get the next byte in memory
    auto n = [=]() -> uint8_t {
        return mmu->at(pc + 1);
    };

    // Get the next word in memory
    auto nn = [=]() -> uint16_t {
        uint16_t b = mmu->at(pc + 1);
        return b | (mmu->at(pc + 2) << 8);
    };

    // Get the next byte and turn it into a signed int
    auto i = [=]() -> int8_t {
        uint8_t u8 = n();
        return static_cast<int8_t>(u8);
    };

    // Print current memory address
    cout << hex(pc) << "        ";

    switch (mmu->at(pc)) {
        case 0x00: cout << "NOP" << endl; break;
        case 0x01: cout << "LD    BC," << hex(nn()) << endl; break;
        case 0x02: cout << "LD    (BC),A" << endl; break;
        case 0x03: cout << "INC   BC" << endl; break;
        case 0x04: cout << "INC   B" << endl; break;
        case 0x05: cout << "DEC   B" << endl; break;
        case 0x06: cout << "LD    B," << hex(n()) << endl; break;
        case 0x07: cout << "RLCA" << endl; break;
        case 0x08: cout << "LD    (" << hex(nn()) << "),SP" << endl; break;
        case 0x09: cout << "ADD   HL,BC" << endl; break;
        case 0x0a: cout << "LD    A,(BC)" << endl; break;
        case 0x0b: cout << "DEC   BC" << endl; break;
        case 0x0c: cout << "INC   C" << endl; break;
        case 0x0d: cout << "DEC   C" << endl; break;
        case 0x0e: cout << "LD    C," << hex(n()) << endl; break;
        case 0x0f: cout << "RRCA" << endl; break;
        case 0x10: cout << "STOP  0" << endl; break;
        case 0x11: cout << "LD    DE," << hex(nn()) << endl; break;
        case 0x12: cout << "LD    (DE),A" << endl; break;
        case 0x13: cout << "INC   DE" << endl; break;
        case 0x14: cout << "INC   D" << endl; break;
        case 0x15: cout << "DEC   D" << endl; break;
        case 0x16: cout << "LD    D," << hex(n()) << endl; break;
        case 0x17: cout << "RLA" << endl; break;
        case 0x18: cout << "JR    " << hex(i()) << endl; break;
        case 0x19: cout << "ADD   HL,DE" << endl; break;
        case 0x1a: cout << "LD    A,(DE)" << endl; break;
        case 0x1b: cout << "DEC   DE" << endl; break;
        case 0x1c: cout << "INC   E" << endl; break;
        case 0x1d: cout << "DEC   E" << endl; break;
        case 0x1e: cout << "LD    E," << hex(n()) << endl; break;
        case 0x1f: cout << "RRA" << endl; break;
        case 0x20: cout << "JR    NZ," << hex(i()) << endl; break;
        case 0x21: cout << "LD    HL," << hex(nn()) << endl; break;
        case 0x22: cout << "LD    (HL+),A" << endl; break;
        case 0x23: cout << "INC   HL" << endl; break;
        case 0x24: cout << "INC   H" << endl; break;
        case 0x25: cout << "DEC   H" << endl; break;
        case 0x26: cout << "LD    H," << hex(n()) << endl; break;
        case 0x27: cout << "DAA" << endl; break;
        case 0x28: cout << "JR    Z," << hex(i()) << endl; break;
        case 0x29: cout << "ADD   HL,HL" << endl; break;
        case 0x2a: cout << "LD    A,(HL+)" << endl; break;
        case 0x2b: cout << "DEC   HL" << endl; break;
        case 0x2c: cout << "INC   L" << endl; break;
        case 0x2d: cout << "DEC   L" << endl; break;
        case 0x2e: cout << "LD    L," << hex(n()) << endl; break;
        case 0x2f: cout << "CPL" << endl; break;
        case 0x30: cout << "JR    NC," << hex(i()) << endl; break;
        case 0x31: cout << "LD    SP," << hex(nn()) << endl; break;
        case 0x32: cout << "LD    (HL-),A" << endl; break;
        case 0x33: cout << "INC   SP" << endl; break;
        case 0x34: cout << "INC   (HL)" << endl; break;
        case 0x35: cout << "DEC   (HL)" << endl; break;
        case 0x36: cout << "LD    (HL)," << hex(n()) << endl; break;
        case 0x37: cout << "SCF" << endl; break;
        case 0x38: cout << "JR    C," << hex(i()) << endl; break;
        case 0x39: cout << "ADD   HL,SP" << endl; break;
        case 0x3a: cout << "LD    A,(HL-)" << endl; break;
        case 0x3b: cout << "DEC   SP" << endl; break;
        case 0x3c: cout << "INC   A" << endl; break;
        case 0x3d: cout << "DEC   A" << endl; break;
        case 0x3e: cout << "LD    A," << hex(n()) << endl; break;
        case 0x3f: cout << "CCF" << endl; break;
        case 0x40: cout << "LD    B,B" << endl; break;
        case 0x41: cout << "LD    B,C" << endl; break;
        case 0x42: cout << "LD    B,D" << endl; break;
        case 0x43: cout << "LD    B,E" << endl; break;
        case 0x44: cout << "LD    B,H" << endl; break;
        case 0x45: cout << "LD    B,L" << endl; break;
        case 0x46: cout << "LD    B,(HL)" << endl; break;
        case 0x47: cout << "LD    B,A" << endl; break;
        case 0x48: cout << "LD    C,B" << endl; break;
        case 0x49: cout << "LD    C,C" << endl; break;
        case 0x4a: cout << "LD    C,D" << endl; break;
        case 0x4b: cout << "LD    C,E" << endl; break;
        case 0x4c: cout << "LD    C,H" << endl; break;
        case 0x4d: cout << "LD    C,L" << endl; break;
        case 0x4e: cout << "LD    C,(HL)" << endl; break;
        case 0x4f: cout << "LD    C,A" << endl; break;
        case 0x50: cout << "LD    D,B" << endl; break;
        case 0x51: cout << "LD    D,C" << endl; break;
        case 0x52: cout << "LD    D,D" << endl; break;
        case 0x53: cout << "LD    D,E" << endl; break;
        case 0x54: cout << "LD    D,H" << endl; break;
        case 0x55: cout << "LD    D,L" << endl; break;
        case 0x56: cout << "LD    D,(HL)" << endl; break;
        case 0x57: cout << "LD    D,A" << endl; break;
        case 0x58: cout << "LD    E,B" << endl; break;
        case 0x59: cout << "LD    E,C" << endl; break;
        case 0x5a: cout << "LD    E,D" << endl; break;
        case 0x5b: cout << "LD    E,E" << endl; break;
        case 0x5c: cout << "LD    E,H" << endl; break;
        case 0x5d: cout << "LD    E,L" << endl; break;
        case 0x5e: cout << "LD    E,(HL)" << endl; break;
        case 0x5f: cout << "LD    E,A" << endl; break;
        case 0x60: cout << "LD    H,B" << endl; break;
        case 0x61: cout << "LD    H,C" << endl; break;
        case 0x62: cout << "LD    H,D" << endl; break;
        case 0x63: cout << "LD    H,E" << endl; break;
        case 0x64: cout << "LD    H,H" << endl; break;
        case 0x65: cout << "LD    H,L" << endl; break;
        case 0x66: cout << "LD    H,(HL)" << endl; break;
        case 0x67: cout << "LD    H,A" << endl; break;
        case 0x68: cout << "LD    L,B" << endl; break;
        case 0x69: cout << "LD    L,C" << endl; break;
        case 0x6a: cout << "LD    L,D" << endl; break;
        case 0x6b: cout << "LD    L,E" << endl; break;
        case 0x6c: cout << "LD    L,H" << endl; break;
        case 0x6d: cout << "LD    L,L" << endl; break;
        case 0x6e: cout << "LD    L,(HL)" << endl; break;
        case 0x6f: cout << "LD    L,A" << endl; break;
        case 0x70: cout << "LD    (HL),B" << endl; break;
        case 0x71: cout << "LD    (HL),C" << endl; break;
        case 0x72: cout << "LD    (HL),D" << endl; break;
        case 0x73: cout << "LD    (HL),E" << endl; break;
        case 0x74: cout << "LD    (HL),H" << endl; break;
        case 0x75: cout << "LD    (HL),L" << endl; break;
        case 0x76: cout << "HALT" << endl; break;
        case 0x77: cout << "LD    (HL),A" << endl; break;
        case 0x78: cout << "LD    A,B" << endl; break;
        case 0x79: cout << "LD    A,C" << endl; break;
        case 0x7a: cout << "LD    A,D" << endl; break;
        case 0x7b: cout << "LD    A,E" << endl; break;
        case 0x7c: cout << "LD    A,H" << endl; break;
        case 0x7d: cout << "LD    A,L" << endl; break;
        case 0x7e: cout << "LD    A,(HL)" << endl; break;
        case 0x7f: cout << "LD    A,A" << endl; break;
        case 0x80: cout << "ADD   A,B" << endl; break;
        case 0x81: cout << "ADD   A,C" << endl; break;
        case 0x82: cout << "ADD   A,D" << endl; break;
        case 0x83: cout << "ADD   A,E" << endl; break;
        case 0x84: cout << "ADD   A,H" << endl; break;
        case 0x85: cout << "ADD   A,L" << endl; break;
        case 0x86: cout << "ADD   A,(HL)" << endl; break;
        case 0x87: cout << "ADD   A,A" << endl; break;
        case 0x88: cout << "ADC   A,B" << endl; break;
        case 0x89: cout << "ADC   A,C" << endl; break;
        case 0x8a: cout << "ADC   A,D" << endl; break;
        case 0x8b: cout << "ADC   A,E" << endl; break;
        case 0x8c: cout << "ADC   A,H" << endl; break;
        case 0x8d: cout << "ADC   A,L" << endl; break;
        case 0x8e: cout << "ADC   A,(HL)" << endl; break;
        case 0x8f: cout << "ADC   A,A" << endl; break;
        case 0x90: cout << "SUB   B" << endl; break;
        case 0x91: cout << "SUB   C" << endl; break;
        case 0x92: cout << "SUB   D" << endl; break;
        case 0x93: cout << "SUB   E" << endl; break;
        case 0x94: cout << "SUB   H" << endl; break;
        case 0x95: cout << "SUB   L" << endl; break;
        case 0x96: cout << "SUB   (HL)" << endl; break;
        case 0x97: cout << "SUB   A" << endl; break;
        case 0x98: cout << "SBC   A,B" << endl; break;
        case 0x99: cout << "SBC   A,C" << endl; break;
        case 0x9a: cout << "SBC   A,D" << endl; break;
        case 0x9b: cout << "SBC   A,E" << endl; break;
        case 0x9c: cout << "SBC   A,H" << endl; break;
        case 0x9d: cout << "SBC   A,L" << endl; break;
        case 0x9e: cout << "SBC   A,(HL)" << endl; break;
        case 0x9f: cout << "SBC   A,A" << endl; break;
        case 0xa0: cout << "AND   B" << endl; break;
        case 0xa1: cout << "AND   C" << endl; break;
        case 0xa2: cout << "AND   D" << endl; break;
        case 0xa3: cout << "AND   E" << endl; break;
        case 0xa4: cout << "AND   H" << endl; break;
        case 0xa5: cout << "AND   L" << endl; break;
        case 0xa6: cout << "AND   (HL)" << endl; break;
        case 0xa7: cout << "AND   A" << endl; break;
        case 0xa8: cout << "XOR   B" << endl; break;
        case 0xa9: cout << "XOR   C" << endl; break;
        case 0xaa: cout << "XOR   D" << endl; break;
        case 0xab: cout << "XOR   E" << endl; break;
        case 0xac: cout << "XOR   H" << endl; break;
        case 0xad: cout << "XOR   L" << endl; break;
        case 0xae: cout << "XOR   (HL)" << endl; break;
        case 0xaf: cout << "XOR   A" << endl; break;
        case 0xb0: cout << "OR    B" << endl; break;
        case 0xb1: cout << "OR    C" << endl; break;
        case 0xb2: cout << "OR    D" << endl; break;
        case 0xb3: cout << "OR    E" << endl; break;
        case 0xb4: cout << "OR    H" << endl; break;
        case 0xb5: cout << "OR    L" << endl; break;
        case 0xb6: cout << "OR    (HL)" << endl; break;
        case 0xb7: cout << "OR    A" << endl; break;
        case 0xb8: cout << "CP    B" << endl; break;
        case 0xb9: cout << "CP    C" << endl; break;
        case 0xba: cout << "CP    D" << endl; break;
        case 0xbb: cout << "CP    E" << endl; break;
        case 0xbc: cout << "CP    H" << endl; break;
        case 0xbd: cout << "CP    L" << endl; break;
        case 0xbe: cout << "CP    (HL)" << endl; break;
        case 0xbf: cout << "CP    A" << endl; break;
        case 0xc0: cout << "RET   NZ" << endl; break;
        case 0xc1: cout << "POP   BC" << endl; break;
        case 0xc2: cout << "JP    NZ," << hex(nn()) << endl; break;
        case 0xc3: cout << "JP    " << hex(nn()) << endl; break;
        case 0xc4: cout << "CALL  NZ," << hex(nn()) << endl; break;
        case 0xc5: cout << "PUSH  BC" << endl; break;
        case 0xc6: cout << "ADD   A," << hex(n()) << endl; break;
        case 0xc7: cout << "RST   00H" << endl; break;
        case 0xc8: cout << "RET   Z" << endl; break;
        case 0xc9: cout << "RET" << endl; break;
        case 0xca: cout << "JP    Z," << hex(nn()) << endl; break;
        case 0xcb:
            switch (mmu->at(pc + 1)) {
                case 0x00: cout << "RLC   B" << endl; break;
                case 0x01: cout << "RLC   C" << endl; break;
                case 0x02: cout << "RLC   D" << endl; break;
                case 0x03: cout << "RLC   E" << endl; break;
                case 0x04: cout << "RLC   H" << endl; break;
                case 0x05: cout << "RLC   L" << endl; break;
                case 0x06: cout << "RLC   (HL)" << endl; break;
                case 0x07: cout << "RLC   A" << endl; break;
                case 0x08: cout << "RRC   B" << endl; break;
                case 0x09: cout << "RRC   C" << endl; break;
                case 0x0a: cout << "RRC   D" << endl; break;
                case 0x0b: cout << "RRC   E" << endl; break;
                case 0x0c: cout << "RRC   H" << endl; break;
                case 0x0d: cout << "RRC   L" << endl; break;
                case 0x0e: cout << "RRC   (HL)" << endl; break;
                case 0x0f: cout << "RRC   A" << endl; break;
                case 0x10: cout << "RL    B" << endl; break;
                case 0x11: cout << "RL    C" << endl; break;
                case 0x12: cout << "RL    D" << endl; break;
                case 0x13: cout << "RL    E" << endl; break;
                case 0x14: cout << "RL    H" << endl; break;
                case 0x15: cout << "RL    L" << endl; break;
                case 0x16: cout << "RL    (HL)" << endl; break;
                case 0x17: cout << "RL    A" << endl; break;
                case 0x18: cout << "RR    B" << endl; break;
                case 0x19: cout << "RR    C" << endl; break;
                case 0x1a: cout << "RR    D" << endl; break;
                case 0x1b: cout << "RR    E" << endl; break;
                case 0x1c: cout << "RR    H" << endl; break;
                case 0x1d: cout << "RR    L" << endl; break;
                case 0x1e: cout << "RR    (HL)" << endl; break;
                case 0x1f: cout << "RR    A" << endl; break;
                case 0x20: cout << "SLA   B" << endl; break;
                case 0x21: cout << "SLA   C" << endl; break;
                case 0x22: cout << "SLA   D" << endl; break;
                case 0x23: cout << "SLA   E" << endl; break;
                case 0x24: cout << "SLA   H" << endl; break;
                case 0x25: cout << "SLA   L" << endl; break;
                case 0x26: cout << "SLA   (HL)" << endl; break;
                case 0x27: cout << "SLA   A" << endl; break;
                case 0x28: cout << "SRA   B" << endl; break;
                case 0x29: cout << "SRA   C" << endl; break;
                case 0x2a: cout << "SRA   D" << endl; break;
                case 0x2b: cout << "SRA   E" << endl; break;
                case 0x2c: cout << "SRA   H" << endl; break;
                case 0x2d: cout << "SRA   L" << endl; break;
                case 0x2e: cout << "SRA   (HL)" << endl; break;
                case 0x2f: cout << "SRA   A" << endl; break;
                case 0x30: cout << "SWAP  B" << endl; break;
                case 0x31: cout << "SWAP  C" << endl; break;
                case 0x32: cout << "SWAP  D" << endl; break;
                case 0x33: cout << "SWAP  E" << endl; break;
                case 0x34: cout << "SWAP  H" << endl; break;
                case 0x35: cout << "SWAP  L" << endl; break;
                case 0x36: cout << "SWAP  (HL)" << endl; break;
                case 0x37: cout << "SWAP  A" << endl; break;
                case 0x38: cout << "SRL   B" << endl; break;
                case 0x39: cout << "SRL   C" << endl; break;
                case 0x3a: cout << "SRL   D" << endl; break;
                case 0x3b: cout << "SRL   E" << endl; break;
                case 0x3c: cout << "SRL   H" << endl; break;
                case 0x3d: cout << "SRL   L" << endl; break;
                case 0x3e: cout << "SRL   (HL)" << endl; break;
                case 0x3f: cout << "SRL   A" << endl; break;
                case 0x40: cout << "BIT   0,B" << endl; break;
                case 0x41: cout << "BIT   0,C" << endl; break;
                case 0x42: cout << "BIT   0,D" << endl; break;
                case 0x43: cout << "BIT   0,E" << endl; break;
                case 0x44: cout << "BIT   0,H" << endl; break;
                case 0x45: cout << "BIT   0,L" << endl; break;
                case 0x46: cout << "BIT   0,(HL)" << endl; break;
                case 0x47: cout << "BIT   0,A" << endl; break;
                case 0x48: cout << "BIT   1,B" << endl; break;
                case 0x49: cout << "BIT   1,C" << endl; break;
                case 0x4a: cout << "BIT   1,D" << endl; break;
                case 0x4b: cout << "BIT   1,E" << endl; break;
                case 0x4c: cout << "BIT   1,H" << endl; break;
                case 0x4d: cout << "BIT   1,L" << endl; break;
                case 0x4e: cout << "BIT   1,(HL)" << endl; break;
                case 0x4f: cout << "BIT   1,A" << endl; break;
                case 0x50: cout << "BIT   2,B" << endl; break;
                case 0x51: cout << "BIT   2,C" << endl; break;
                case 0x52: cout << "BIT   2,D" << endl; break;
                case 0x53: cout << "BIT   2,E" << endl; break;
                case 0x54: cout << "BIT   2,H" << endl; break;
                case 0x55: cout << "BIT   2,L" << endl; break;
                case 0x56: cout << "BIT   2,(HL)" << endl; break;
                case 0x57: cout << "BIT   2,A" << endl; break;
                case 0x58: cout << "BIT   3,B" << endl; break;
                case 0x59: cout << "BIT   3,C" << endl; break;
                case 0x5a: cout << "BIT   3,D" << endl; break;
                case 0x5b: cout << "BIT   3,E" << endl; break;
                case 0x5c: cout << "BIT   3,H" << endl; break;
                case 0x5d: cout << "BIT   3,L" << endl; break;
                case 0x5e: cout << "BIT   3,(HL)" << endl; break;
                case 0x5f: cout << "BIT   3,A" << endl; break;
                case 0x60: cout << "BIT   4,B" << endl; break;
                case 0x61: cout << "BIT   4,C" << endl; break;
                case 0x62: cout << "BIT   4,D" << endl; break;
                case 0x63: cout << "BIT   4,E" << endl; break;
                case 0x64: cout << "BIT   4,H" << endl; break;
                case 0x65: cout << "BIT   4,L" << endl; break;
                case 0x66: cout << "BIT   4,(HL)" << endl; break;
                case 0x67: cout << "BIT   4,A" << endl; break;
                case 0x68: cout << "BIT   5,B" << endl; break;
                case 0x69: cout << "BIT   5,C" << endl; break;
                case 0x6a: cout << "BIT   5,D" << endl; break;
                case 0x6b: cout << "BIT   5,E" << endl; break;
                case 0x6c: cout << "BIT   5,H" << endl; break;
                case 0x6d: cout << "BIT   5,L" << endl; break;
                case 0x6e: cout << "BIT   5,(HL)" << endl; break;
                case 0x6f: cout << "BIT   5,A" << endl; break;
                case 0x70: cout << "BIT   6,B" << endl; break;
                case 0x71: cout << "BIT   6,C" << endl; break;
                case 0x72: cout << "BIT   6,D" << endl; break;
                case 0x73: cout << "BIT   6,E" << endl; break;
                case 0x74: cout << "BIT   6,H" << endl; break;
                case 0x75: cout << "BIT   6,L" << endl; break;
                case 0x76: cout << "BIT   6,(HL)" << endl; break;
                case 0x77: cout << "BIT   6,A" << endl; break;
                case 0x78: cout << "BIT   7,B" << endl; break;
                case 0x79: cout << "BIT   7,C" << endl; break;
                case 0x7a: cout << "BIT   7,D" << endl; break;
                case 0x7b: cout << "BIT   7,E" << endl; break;
                case 0x7c: cout << "BIT   7,H" << endl; break;
                case 0x7d: cout << "BIT   7,L" << endl; break;
                case 0x7e: cout << "BIT   7,(HL)" << endl; break;
                case 0x7f: cout << "BIT   7,A" << endl; break;
                case 0x80: cout << "RES   0,B" << endl; break;
                case 0x81: cout << "RES   0,C" << endl; break;
                case 0x82: cout << "RES   0,D" << endl; break;
                case 0x83: cout << "RES   0,E" << endl; break;
                case 0x84: cout << "RES   0,H" << endl; break;
                case 0x85: cout << "RES   0,L" << endl; break;
                case 0x86: cout << "RES   0,(HL)" << endl; break;
                case 0x87: cout << "RES   0,A" << endl; break;
                case 0x88: cout << "RES   1,B" << endl; break;
                case 0x89: cout << "RES   1,C" << endl; break;
                case 0x8a: cout << "RES   1,D" << endl; break;
                case 0x8b: cout << "RES   1,E" << endl; break;
                case 0x8c: cout << "RES   1,H" << endl; break;
                case 0x8d: cout << "RES   1,L" << endl; break;
                case 0x8e: cout << "RES   1,(HL)" << endl; break;
                case 0x8f: cout << "RES   1,A" << endl; break;
                case 0x90: cout << "RES   2,B" << endl; break;
                case 0x91: cout << "RES   2,C" << endl; break;
                case 0x92: cout << "RES   2,D" << endl; break;
                case 0x93: cout << "RES   2,E" << endl; break;
                case 0x94: cout << "RES   2,H" << endl; break;
                case 0x95: cout << "RES   2,L" << endl; break;
                case 0x96: cout << "RES   2,(HL)" << endl; break;
                case 0x97: cout << "RES   2,A" << endl; break;
                case 0x98: cout << "RES   3,B" << endl; break;
                case 0x99: cout << "RES   3,C" << endl; break;
                case 0x9a: cout << "RES   3,D" << endl; break;
                case 0x9b: cout << "RES   3,E" << endl; break;
                case 0x9c: cout << "RES   3,H" << endl; break;
                case 0x9d: cout << "RES   3,L" << endl; break;
                case 0x9e: cout << "RES   3,(HL)" << endl; break;
                case 0x9f: cout << "RES   3,A" << endl; break;
                case 0xa0: cout << "RES   4,B" << endl; break;
                case 0xa1: cout << "RES   4,C" << endl; break;
                case 0xa2: cout << "RES   4,D" << endl; break;
                case 0xa3: cout << "RES   4,E" << endl; break;
                case 0xa4: cout << "RES   4,H" << endl; break;
                case 0xa5: cout << "RES   4,L" << endl; break;
                case 0xa6: cout << "RES   4,(HL)" << endl; break;
                case 0xa7: cout << "RES   4,A" << endl; break;
                case 0xa8: cout << "RES   5,B" << endl; break;
                case 0xa9: cout << "RES   5,C" << endl; break;
                case 0xaa: cout << "RES   5,D" << endl; break;
                case 0xab: cout << "RES   5,E" << endl; break;
                case 0xac: cout << "RES   5,H" << endl; break;
                case 0xad: cout << "RES   5,L" << endl; break;
                case 0xae: cout << "RES   5,(HL)" << endl; break;
                case 0xaf: cout << "RES   5,A" << endl; break;
                case 0xb0: cout << "RES   6,B" << endl; break;
                case 0xb1: cout << "RES   6,C" << endl; break;
                case 0xb2: cout << "RES   6,D" << endl; break;
                case 0xb3: cout << "RES   6,E" << endl; break;
                case 0xb4: cout << "RES   6,H" << endl; break;
                case 0xb5: cout << "RES   6,L" << endl; break;
                case 0xb6: cout << "RES   6,(HL)" << endl; break;
                case 0xb7: cout << "RES   6,A" << endl; break;
                case 0xb8: cout << "RES   7,B" << endl; break;
                case 0xb9: cout << "RES   7,C" << endl; break;
                case 0xba: cout << "RES   7,D" << endl; break;
                case 0xbb: cout << "RES   7,E" << endl; break;
                case 0xbc: cout << "RES   7,H" << endl; break;
                case 0xbd: cout << "RES   7,L" << endl; break;
                case 0xbe: cout << "RES   7,(HL)" << endl; break;
                case 0xbf: cout << "RES   7,A" << endl; break;
                case 0xc0: cout << "SET   0,B" << endl; break;
                case 0xc1: cout << "SET   0,C" << endl; break;
                case 0xc2: cout << "SET   0,D" << endl; break;
                case 0xc3: cout << "SET   0,E" << endl; break;
                case 0xc4: cout << "SET   0,H" << endl; break;
                case 0xc5: cout << "SET   0,L" << endl; break;
                case 0xc6: cout << "SET   0,(HL)" << endl; break;
                case 0xc7: cout << "SET   0,A" << endl; break;
                case 0xc8: cout << "SET   1,B" << endl; break;
                case 0xc9: cout << "SET   1,C" << endl; break;
                case 0xca: cout << "SET   1,D" << endl; break;
                case 0xcb: cout << "SET   1,E" << endl; break;
                case 0xcc: cout << "SET   1,H" << endl; break;
                case 0xcd: cout << "SET   1,L" << endl; break;
                case 0xce: cout << "SET   1,(HL)" << endl; break;
                case 0xcf: cout << "SET   1,A" << endl; break;
                case 0xd0: cout << "SET   2,B" << endl; break;
                case 0xd1: cout << "SET   2,C" << endl; break;
                case 0xd2: cout << "SET   2,D" << endl; break;
                case 0xd3: cout << "SET   2,E" << endl; break;
                case 0xd4: cout << "SET   2,H" << endl; break;
                case 0xd5: cout << "SET   2,L" << endl; break;
                case 0xd6: cout << "SET   2,(HL)" << endl; break;
                case 0xd7: cout << "SET   2,A" << endl; break;
                case 0xd8: cout << "SET   3,B" << endl; break;
                case 0xd9: cout << "SET   3,C" << endl; break;
                case 0xda: cout << "SET   3,D" << endl; break;
                case 0xdb: cout << "SET   3,E" << endl; break;
                case 0xdc: cout << "SET   3,H" << endl; break;
                case 0xdd: cout << "SET   3,L" << endl; break;
                case 0xde: cout << "SET   3,(HL)" << endl; break;
                case 0xdf: cout << "SET   3,A" << endl; break;
                case 0xe0: cout << "SET   4,B" << endl; break;
                case 0xe1: cout << "SET   4,C" << endl; break;
                case 0xe2: cout << "SET   4,D" << endl; break;
                case 0xe3: cout << "SET   4,E" << endl; break;
                case 0xe4: cout << "SET   4,H" << endl; break;
                case 0xe5: cout << "SET   4,L" << endl; break;
                case 0xe6: cout << "SET   4,(HL)" << endl; break;
                case 0xe7: cout << "SET   4,A" << endl; break;
                case 0xe8: cout << "SET   5,B" << endl; break;
                case 0xe9: cout << "SET   5,C" << endl; break;
                case 0xea: cout << "SET   5,D" << endl; break;
                case 0xeb: cout << "SET   5,E" << endl; break;
                case 0xec: cout << "SET   5,H" << endl; break;
                case 0xed: cout << "SET   5,L" << endl; break;
                case 0xee: cout << "SET   5,(HL)" << endl; break;
                case 0xef: cout << "SET   5,A" << endl; break;
                case 0xf0: cout << "SET   6,B" << endl; break;
                case 0xf1: cout << "SET   6,C" << endl; break;
                case 0xf2: cout << "SET   6,D" << endl; break;
                case 0xf3: cout << "SET   6,E" << endl; break;
                case 0xf4: cout << "SET   6,H" << endl; break;
                case 0xf5: cout << "SET   6,L" << endl; break;
                case 0xf6: cout << "SET   6,(HL)" << endl; break;
                case 0xf7: cout << "SET   6,A" << endl; break;
                case 0xf8: cout << "SET   7,B" << endl; break;
                case 0xf9: cout << "SET   7,C" << endl; break;
                case 0xfa: cout << "SET   7,D" << endl; break;
                case 0xfb: cout << "SET   7,E" << endl; break;
                case 0xfc: cout << "SET   7,H" << endl; break;
                case 0xfd: cout << "SET   7,L" << endl; break;
                case 0xfe: cout << "SET   7,(HL)" << endl; break;
                case 0xff: cout << "SET   7,A" << endl; break;
            }
            break;
        case 0xcc: cout << "CALL  Z," << hex(nn()) << endl; break;
        case 0xcd: cout << "CALL  " << hex(nn()) << endl; break;
        case 0xce: cout << "ADC   A," << hex(n()) << endl; break;
        case 0xcf: cout << "RST   08H" << endl; break;
        case 0xd0: cout << "RET   NC" << endl; break;
        case 0xd1: cout << "POP   DE" << endl; break;
        case 0xd2: cout << "JP    NC," << hex(nn()) << endl; break;
        case 0xd3: cout << "undefined" << endl; break;
        case 0xd4: cout << "CALL  NC," << hex(nn()) << endl; break;
        case 0xd5: cout << "PUSH  DE" << endl; break;
        case 0xd6: cout << "SUB   " << hex(n()) << endl; break;
        case 0xd7: cout << "RST   10H" << endl; break;
        case 0xd8: cout << "RET   C" << endl; break;
        case 0xd9: cout << "RETI" << endl; break;
        case 0xda: cout << "JP    NC," << hex(nn()) << endl; break;
        case 0xdb: cout << "undefined" << endl; break;
        case 0xdc: cout << "CALL  C," << hex(nn()) << endl; break;
        case 0xdd: cout << "undefined" << endl; break;
        case 0xde: cout << "SBC   A," << hex(n()) << endl; break;
        case 0xdf: cout << "RST   18H" << endl; break;
        case 0xe0: cout << "LDH   ($00ff+" << hex(n()) << "),A" << endl; break;
        case 0xe1: cout << "POP   HL" << endl; break;
        case 0xe2: cout << "LD    ($ff00+C),A" << endl; break;
        case 0xe3: cout << "undefined" << endl; break;
        case 0xe4: cout << "undefined" << endl; break;
        case 0xe5: cout << "PUSH  HL" << endl; break;
        case 0xe6: cout << "AND   " << hex(n()) << endl; break;
        case 0xe7: cout << "RST   20H" << endl; break;
        case 0xe8: cout << "ADD   SP," << hex(i()) << endl; break;
        case 0xe9: cout << "JP    (HL)" << endl; break;
        case 0xea: cout << "LD    (" << hex(nn()) << "),A" << endl; break;
        case 0xeb: cout << "undefined" << endl; break;
        case 0xec: cout << "CALL  C," << hex(nn()) << endl; break;
        case 0xed: cout << "undefined" << endl; break;
        case 0xee: cout << "XOR   " << hex(n()) << endl; break;
        case 0xef: cout << "RST   28H" << endl; break;
        case 0xf0: cout << "LDH   A,($ff00+" << hex(n()) << ")" << endl; break;
        case 0xf1: cout << "POP   AF" << endl; break;
        case 0xf2: cout << "LD    A,($ff00+C)" << endl; break;
        case 0xf3: cout << "DI" << endl; break;
        case 0xf4: cout << "undefined" << endl; break;
        case 0xf5: cout << "PUSH  AF" << endl; break;
        case 0xf6: cout << "OR    " << hex(n()) << endl; break;
        case 0xf7: cout << "RST   30H" << endl; break;
        case 0xf8: cout << "LD    HL,SP+" << hex(i()) << endl; break;
        case 0xf9: cout << "LD    SP,HL" << endl; break;
        case 0xfa: cout << "LD    A,(" << hex(nn()) << ")" << endl; break;
        case 0xfb: cout << "EI" << endl; break;
        case 0xfc: cout << "undefined" << endl; break;
        case 0xfd: cout << "undefined" << endl; break;
        case 0xfe: cout << "CP    " << hex(n()) << endl; break;
        case 0xff: cout << "RST   38H" << endl; break;
    }

    if (DEBUG_MODE) {
        cout << "------------------------------------------" << endl;
        cout << "Reg A: " << endl;
        cout << "    Hex: " << hex(reg.a()) << endl;
        cout << "    Binary: " << std::bitset<8>(reg.a()) << endl;
        cout << "Reg F: " << endl;
        cout << "    Hex: " << hex(reg.f()) << endl;
        cout << "    Binary: " << std::bitset<8>(reg.f()) << endl;
        cout << "Reg B: " << endl;
        cout << "    Hex: " << hex(reg.b()) << endl;
        cout << "    Binary: " << std::bitset<8>(reg.b()) << endl;
        cout << "Reg C: " << endl;
        cout << "    Hex: " << hex(reg.c()) << endl;
        cout << "    Binary: " << std::bitset<8>(reg.c()) << endl;
        cout << "Reg D: " << endl;
        cout << "    Hex: " << hex(reg.d()) << endl;
        cout << "    Binary: " << std::bitset<8>(reg.d()) << endl;
        cout << "Reg E: " << endl;
        cout << "    Hex: " << hex(reg.e()) << endl;
        cout << "    Binary: " << std::bitset<8>(reg.e()) << endl;
        cout << "Reg H: " << endl;
        cout << "    Hex: " << hex(reg.h()) << endl;
        cout << "    Binary: " << std::bitset<8>(reg.h()) << endl;
        cout << "Reg L: " << endl;
        cout << "    Hex: " << hex(reg.l()) << endl;
        cout << "    Binary: " << std::bitset<8>(reg.l()) << endl;
        cout << "ZF: " << reg.get_zf() << endl;
        cout << "NF: " << reg.get_nf() << endl;
        cout << "HF: " << reg.get_hf() << endl;
        cout << "CF: " << reg.get_cf() << endl;
        cout << "SP: " << hex(sp) << endl;
        cout << "PC: " << hex(pc) << endl;
        cout << "------------------------------------------" << endl;
    }
}   