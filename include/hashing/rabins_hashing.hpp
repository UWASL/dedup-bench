#pragma once
#ifndef _RABINS_HASHING_
#define _RABINS_HASHING_

#define INT64(n) n##LL
#define MSB64 INT64(0x8000000000000000)

#define DEFAULT_WINDOW_SIZE 32

#include "hashing_common.hpp"

/* Fingerprint value taken from LBFS fingerprint.h. For details on this,
 * refer to the original rabin fingerprint paper.
 */
#define FINGERPRINT_PT 0xbfe6b8a5bf378d83LL

#define INT64(n) n##LL
#define MSB64 INT64(0x8000000000000000)

// find last set bit in a 32-bit word

u_int32_t fls32(u_int32_t x);
// find last set bit in a 64-bit word
char fls64(u_int64_t v);

class Rabins_Hashing : public virtual Hashing_Technique {
    /**
     * @brief Class to implement Rabins Rolling hashing
     *
     */
   private:
    u_int64_t poly;  // Actual polynomial
    int shift;
    u_int64_t T[256];  // Lookup table for mod
    u_int64_t U[256];
    uint64_t window_size;

    unsigned char *circbuf;    // circular buffer of size 'window_size'
    unsigned int circbuf_pos;  // current position in circular buffer

    void calcT();

   public:
    u_int64_t fingerprint;  // current rabin fingerprint

    std::string hash_chunk(File_Chunk file_chunk);
    // Function to hash a given chunk
    u_int64_t polymod(u_int64_t nh, u_int64_t nl, u_int64_t d);

    void polymult(u_int64_t *php, u_int64_t *plp, u_int64_t x, u_int64_t y);

    u_int64_t polymmult(u_int64_t x, u_int64_t y, u_int64_t d);

    u_int64_t slide8(unsigned char m);

    u_int64_t append8(u_int64_t p, unsigned char m);

    void init();

    Rabins_Hashing() {
        poly = FINGERPRINT_PT;
        window_size = DEFAULT_WINDOW_SIZE;
        technique_name = "Rabins Hashing";
        calcT();
        init();
    }
};

#endif