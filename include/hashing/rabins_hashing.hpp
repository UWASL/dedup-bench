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

u_int32_t fls32(u_int32_t x);
char fls64(u_int64_t v);

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