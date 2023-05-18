/**
 * @file gear_chunking.cpp
 * @author WASL
 * @brief Implementations for gear chunking technique
 * @version 0.1
 * @date 2023-4-1
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "fastcdc.hpp"

#include <cassert>
#include <cstring>
#include <fstream>
#include <iostream>
#include <limits>
#include <memory>

FastCDC::FastCDC(const Config& config) {
    min_block_size = config.get_fastcdc_min_block_size();
    avg_block_size = config.get_fastcdc_avg_block_size();
    max_block_size = config.get_fastcdc_max_block_size();
    threshold = std::numeric_limits<uint64_t>::max() /
                (avg_block_size - min_block_size + 1);
}


uint64_t FastCDC::find_cutpoint(char* data, uint64_t len) {
    if (len <= min_block_size) {
        return len;
    }
    if (len > max_block_size) {
        len = max_block_size;
    }

    // Initialize the regression length to len (the end) and the regression
    // mask to an empty bitmask (match any hash).
    size_t rc_len = len;
    uint64_t rc_mask = 0;

    // Init hash to all 1's to avoid zero-length chunks with min_size=0.
    uint64_t hash = std::numeric_limits<uint64_t>::max();
    // Skip the first min_size bytes, but "warm up" the rolling hash for enough
    // rounds to make sure the hash has gathered full "content history".
    size_t i = min_block_size > kHashBits ? min_block_size - kHashBits : 0;
    for (/*empty*/; i < min_block_size; ++i) {
        hash = (hash << 1) + GEAR_TABLE[data[i]];
    }
    for (/*empty*/; i < len; ++i) {
        if (!(hash & rc_mask)) {
            if (hash <= threshold) {
                // This hash matches the target length hash criteria, return it.
                return i;
            }
            // This is a better regression point. Set it as the new rc_len and
            // update rc_mask to check as many MSBits as this hash would pass.
            rc_len = i;
            rc_mask = std::numeric_limits<uint64_t>::max();
            while (hash & rc_mask) rc_mask <<= 1;
        }
        hash = (hash << 1) + GEAR_TABLE[data[i]];
    }
    // Return best regression point we found or the end if it's better.
    return (hash & rc_mask) ? rc_len : i;
}

FastCDC::~FastCDC() {}

