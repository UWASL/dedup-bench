#pragma once
#ifndef _RABINS_HASHING_
#define _RABINS_HASHING_
#include <iostream>

#define INT64(n) n##LL
#define MSB64 INT64(0x8000000000000000)

/* Fingerprint value taken from LBFS fingerprint.h. For details on this,
 * refer to the original rabin fingerprint paper.
 */
#define FINGERPRINT_PT 0xbfe6b8a5bf378d83LL

// Declaring and defining functions in header to allow it be used within TTTD Chunking without needing a local reimplementation
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

inline char fls64(u_int64_t num) {
    /**
    @brief find last set bit in a 64-bit number
    @param num: the number to operate on

    @return: last set bit in a 64-bit number
    */
    u_int32_t h;
    if ((h = num >> 32))
        return 32 + fls32(h);
    else
        return fls32((u_int32_t)num);
}


class Rabins_Hashing {
    /**
     * @brief Class to implement Rabins Rolling hashing
     *
     */
   private:
    u_int64_t poly;  // Actual polynomial
    int shift;
    u_int64_t T[256];      // Lookup table for mod
    u_int64_t U[256];      // Lookup table for mod
    uint64_t window_size;  // sl

    unsigned char *circbuf;    // circular buffer of size 'window_size'
    unsigned int circbuf_pos;  // current position in circular buffer

    /**
     * @brief Initialize the T[] and U[] array for faster computation of
     *  rabin fingerprint.  Called only once from the constructer
     * initialization.
     */
    void calcT();
    u_int64_t polymod(u_int64_t nh, u_int64_t nl, u_int64_t d);

    void polymult(u_int64_t *php, u_int64_t *plp, u_int64_t x, u_int64_t y);

    u_int64_t polymmult(u_int64_t x, u_int64_t y, u_int64_t d);

    /**
     * @brief helper function to append the mod value from U table. to help add
     * more randomization to the fingerprint.
     * @return the current fingerprint
     */
    u_int64_t append8(u_int64_t p, unsigned char m);

   public:
    u_int64_t fingerprint;  // current rabin fingerprint

    /**
     * @brief Feed a new byte into the rabin sliding window and update the rabin
     *       fingerprint.
     * @return the current fingerprint
     */
    u_int64_t slide8(unsigned char m);

    /**
     *   @brief initlize the internal varibles. call after each file to reset
     *   @param window_size: the size of the window to operate on
     */
    void init(int window_size);

    Rabins_Hashing();
};

#endif