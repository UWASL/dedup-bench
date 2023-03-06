#include "rabins_hashing.hpp"

#include <string>

Rabins_Hashing::Rabins_Hashing() {
    poly = FINGERPRINT_PT;
    calcT();
}

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

void Rabins_Hashing::init(int window_size) {

    this->window_size = window_size;
    circbuf = (unsigned char *)malloc(window_size * sizeof(unsigned char));
    fingerprint = 0;
    circbuf_pos = -1;
    bzero((char *)circbuf, window_size * sizeof(unsigned char));
}

u_int64_t Rabins_Hashing::polymod(u_int64_t nh, u_int64_t nl, u_int64_t d) {
    int i;
    int k = fls64(d) - 1;
    d <<= 63 - k;

    if (nh) {
        if (nh & MSB64) nh ^= d;  // XXX unreachable? (on 32 bit platform?)
        for (i = 62; i >= 0; i--)
            if (nh & ((u_int64_t)1) << i) {
                nh ^= d >> (63 - i);
                nl ^= d << (i + 1);
            }
    }
    for (i = 63; i >= k; i--) {
        if (nl & INT64(1) << i) nl ^= d >> (63 - i);
    }
    return nl;
}

void Rabins_Hashing::polymult(u_int64_t *php, u_int64_t *plp, u_int64_t x,
                              u_int64_t y) {
    int i;
    u_int64_t ph = 0, pl = 0;
    if (x & 1) pl = y;
    for (i = 1; i < 64; i++)
        if (x & (INT64(1) << i)) {
            ph ^= y >> (64 - i);
            pl ^= y << i;
        }
    if (php) *php = ph;
    if (plp) *plp = pl;
}

u_int64_t Rabins_Hashing::polymmult(u_int64_t x, u_int64_t y, u_int64_t d) {
    u_int64_t h, l;
    polymult(&h, &l, x, y);
    return polymod(h, l, d);
}

void Rabins_Hashing::calcT() {

    unsigned int i;
    int xshift = fls64(poly) - 1;
    shift = xshift - 8;

    u_int64_t T1 = polymod(0, INT64(1) << xshift, poly);
    for (i = 0; i < 256; i++) {
        T[i] = polymmult(i, T1, poly) | ((u_int64_t)i << xshift);
    }

    u_int64_t sizeshift = 1;
    for (i = 1; i < window_size; i++) {
        sizeshift = append8(sizeshift, 0);
    }

    for (i = 0; i < 256; i++) {
        U[i] = polymmult(i, sizeshift, poly);
    }
}

u_int64_t Rabins_Hashing::slide8(unsigned char m) {

    circbuf_pos++;
    if (circbuf_pos >= window_size) {
        circbuf_pos = 0;
    }
    unsigned char om = circbuf[circbuf_pos];
    circbuf[circbuf_pos] = m;
    return fingerprint = append8(fingerprint ^ U[om], m);
}

u_int64_t Rabins_Hashing::append8(u_int64_t p, unsigned char m) {

    return ((p << 8) | m) ^ T[p >> shift];
}