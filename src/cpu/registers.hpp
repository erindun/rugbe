#include <array>
#include <cstdint>

/*********************************************************************
 * Wrapper class for a union that acts as the CPU's registers.
 * 
 * Note that the method of accessing the registers as 8-bit or
 * 16-bit is by type-punning a union, which is *technically* 
 * undefined behavior in C++, but functionality is provided by g++.
 *********************************************************************/

class Registers {
    private:
        union RegUnion {
            std::array<uint8_t, 8> r;
            std::array<uint16_t, 4> rr;
        } reg;

    public:
        // Initialize all registers to 0
        Registers() { reg.rr.fill(0); }

        // Functions to access/modify each register

        uint8_t& f() {
            return reg.r.at(0);
        }

        uint8_t& a() {
            return reg.r.at(1);
        }

        uint8_t& c() {
            return reg.r.at(2);
        }

        uint8_t& b() {
            return reg.r.at(3);
        }

        uint8_t& e() {
            return reg.r.at(4);
        }

        uint8_t& d() {
            return reg.r.at(5);
        }

        uint8_t& l() {
            return reg.r.at(6);
        }

        uint8_t& h() {
            return reg.r.at(7);
        }

        uint16_t& af() {
            return reg.rr.at(0);
        }

        uint16_t& bc() {
            return reg.rr.at(1);
        }

        uint16_t& de() {
            return reg.rr.at(2);
        }

        uint16_t& hl() {
            return reg.rr.at(3);
        }


        // Functions to access/modify each flag

        // Carry flag

        bool get_cf() {
            return (reg.r.at(0) & 0b00010000) >> 4;
        }

        void set_cf(bool val) {
            switch (val) {
                case 0:
                    reg.r.at(0) &= 0b11101111;
                    break;
                case 1:
                    reg.r.at(0) |= 0b00010000;
                    break;
            }
        }

        // Set carry flag if there is a carry (add)
        // or if there is no borrow (subtract)
        template <typename S, typename T>
        void calc_cf(S val1, T val2) {
            // If operation is subtraction
            if (get_nf()) {
                if (val1 < val2) {
                    set_cf(0);
                } else {
                    set_cf(1);
                }
            // If operation is addition
            } else {
                if ((val1 + val2) > 0xff) {
                    set_cf(1);
                } else {
                    set_cf(0);
                }
            }
        }

        // Half-carry flag

        bool get_hf() {
            return (reg.r.at(0) & 0b00100000) >> 5;
        }

        void set_hf(bool val) {
            switch (val) {
                case 0:
                    reg.r.at(0) &= 0b11011111;
                    break;
                case 1:
                    reg.r.at(0) |= 0b00100000;
                    break;
            }
        }

        // Set half-carry flag if there is a carry (add) 
        // or if there is no borrow (subtract)
        template <typename S, typename T>
        void calc_hf(S val1, T val2) {
            // If operation is subtraction
            if (get_nf()) {
                if (((val1 & 0xf) < (val2 & 0xf))) {
                    set_hf(0);
                } else {
                    set_hf(1);
                }
            // If operation is addition
            } else {
                if ((((val1 & 0xf) + (val2 & 0xf)) & 0x10) == 0x10) {
                    set_hf(1);
                } else {
                    set_hf(0);
                }
            }
        }

        // Subtract flag

        bool get_nf() {
            return (reg.r.at(0) & 0b01000000) >> 6;
        }

        void set_nf(bool val) {
            switch (val) {
                case 0:
                    reg.r.at(0) &= 0b10111111;
                    break;
                case 1:
                    reg.r.at(0) |= 0b01000000;
                    break;
            }
        }

        // Zero flag

        bool get_zf() {
            return (reg.r.at(0) & 0b10000000) >> 7;
        }

        void set_zf(bool val) {
            switch (val) {
                case 0:
                    reg.r.at(0) &= 0b01111111;
                    break;
                case 1:
                    reg.r.at(0) |= 0b10000000;
                    break;
            }
        }

        // Set zero flag if the result = 0
        template <typename T>
        void calc_zf(T result) { result == 0 ? set_zf(1) : set_zf(0); }
};