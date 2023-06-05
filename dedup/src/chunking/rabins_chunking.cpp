/**
 * @file rabins_chunking.cpp
 * @author WASL
 * @brief Implementations for raibn chunking technique
 * @version 0.1
 * @date 2023-02-28
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "rabins_chunking.hpp"
#include <iostream>
inline u_int32_t fls32(u_int32_t num) {
    /**
    @brief find last set bit in a 32-bit number
    @param num: the number to operate on

    @return: last set bit in a 32-bit number
    */
    int r = 32;
    if (!num) return 0;
    if (!(num & 0xffff0000u)) {
        num <<= 16;
        r -= 16;
    }
    if (!(num & 0xff000000u)) {
        num <<= 8;
        r -= 8;
    }
    if (!(num & 0xf0000000u)) {
        num <<= 4;
        r -= 4;
    }
    if (!(num & 0xc0000000u)) {
        num <<= 2;
        r -= 2;
    }
    if (!(num & 0x80000000u)) {
        num <<= 1;
        r -= 1;
    }
    return r;
}

struct chunk_t {
    unsigned int start;
    unsigned int length;
    uint64_t cut_fingerprint;
};
struct chunk_t last_chunk;

int Rabins_Chunking::deg(uint64_t p) {
    uint64_t mask = 0x8000000000000000LL;

    for (int i = 0; i < 64; i++) {
        if ((mask & p) > 0) {
            return 63 - i;
        }

        mask >>= 1;
    }
    return -1;
}

// Mod calculates the remainder of x divided by p.
uint64_t Rabins_Chunking::mod(uint64_t x, uint64_t p) {
    while (deg(x) >= deg(p)) {
        unsigned int sshift = deg(x) - deg(p);

        x = x ^ (p << sshift);
    }

    return x;
}


uint64_t Rabins_Chunking::append_byte(uint64_t hash, uint8_t b, uint64_t pol) {
    hash <<= 8;
    hash |= (uint64_t)b;

    return mod(hash, pol);
}

void Rabins_Chunking::calc_tables(void) {
    // calculate table for sliding out bytes. The byte to slide out is used as
    // the index for the table, the value contains the following:
    // out_table[b] = Hash(b || 0 ||        ...        || 0)
    //                          \ windowsize-1 zero bytes /
    // To slide out byte b_0 for window size w with known hash
    // H := H(b_0 || ... || b_w), it is sufficient to add out_table[b_0]:
    //    H(b_0 || ... || b_w) + H(b_0 || 0 || ... || 0)
    //  = H(b_0 + b_0 || b_1 + 0 || ... || b_w + 0)
    //  = H(    0     || b_1 || ...     || b_w)
    //
    // Afterwards a new byte can be shifted in.
    for (int b = 0; b < 256; b++) {
        uint64_t hash = 0;

        hash = append_byte(hash, (uint8_t)b, POLYNOMIAL);
        for (uint64_t i = 0; i < window_size - 1; i++) {
            hash = append_byte(hash, 0, POLYNOMIAL);
        }
        out_table[b] = hash;
    }

    // calculate table for reduction mod Polynomial
    int k = deg(POLYNOMIAL);
    for (int b = 0; b < 256; b++) {
        // mod_table[b] = A | B, where A = (b(x) * x^k mod pol) and  B = b(x) *
        // x^k
        //
        // The 8 bits above deg(Polynomial) determine what happens next and so
        // these bits are used as a lookup to this table. The value is split in
        // two parts: Part A contains the result of the modulus operation, part
        // B is used to cancel out the 8 top bits so that one XOR operation is
        // enough to reduce modulo Polynomial
        mod_table[b] = mod(((uint64_t)b) << k, POLYNOMIAL) | ((uint64_t)b) << k;
    }
}

void Rabins_Chunking::rabin_append(uint8_t b) {
    uint8_t index = (uint8_t)(digest >> POL_SHIFT);
    digest <<= 8;
    digest |= (uint64_t)b;
    digest ^= mod_table[index];
}

void Rabins_Chunking::rabin_slide(uint8_t b) {
    uint8_t out = window[wpos];
    window[wpos] = b;
    digest = (digest ^ out_table[out]);
    wpos = (wpos + 1) % window_size;
    rabin_append(b);
}

void Rabins_Chunking::rabin_reset() {
    for (uint64_t i = 0; i < window_size; i++) window[i] = 0;
    wpos = 0;
    count = 0;
    digest = 0;

    rabin_slide(1);
}

uint64_t Rabins_Chunking::find_cutpoint(char *buf, uint64_t len) {
    for (unsigned int i = 0; i < len; i++) {
        char b = *buf++;

        rabin_slide(b);

        count++;
        pos++;

        if ((count >= min_block_size && ((digest & fingerprint_mask) == 0)) ||
            count >= max_block_size) {
            uint64_t size = count;
            rabin_reset();
            // std::cout << size <<std::endl;
            return size;
        }
    }

    return len;
}

void Rabins_Chunking::rabin_init() {
    if (!tables_initialized) {
        calc_tables();
        tables_initialized = true;
    }
    rabin_reset();
}


Rabins_Chunking::Rabins_Chunking(const Config &config) {
    min_block_size = config.get_rabinc_min_block_size();
    avg_block_size = config.get_rabinc_avg_block_size();
    max_block_size = config.get_rabinc_max_block_size();
    window = new uint8_t[config.get_rabinc_window_size()];
    window_size = config.get_rabinc_window_size();
    fingerprint_mask = (1 << (fls32(avg_block_size) - 1)) - 1;
    rabin_init();
}

Rabins_Chunking::~Rabins_Chunking() { delete[] window; }

