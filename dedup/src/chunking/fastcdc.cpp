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
#include <cmath>
#include <cstring>
#include <fstream>
#include <iostream>
#include <limits>
#include <memory>

FastCDC::FastCDC(const Config& config) {
    min_block_size = config.get_fastcdc_min_block_size();
    avg_block_size = config.get_fastcdc_avg_block_size();
    max_block_size = config.get_fastcdc_max_block_size();
    bool disable_normalization = config.get_fastcdc_disable_normalization();
    if (disable_normalization) {
        notmalization_level = 0;
    } else {
        notmalization_level = config.get_fastcdc_normalization_level();
    }
    int mask_bits = int(round(log2(avg_block_size)));
    small_mask = (1 << (mask_bits + notmalization_level)) - 1 ;
    large_mask = (1 << (mask_bits - notmalization_level)) - 1 ;
}

uint64_t FastCDC::find_cutpoint(char* data, uint64_t len) {
    uint64_t fp = 0;
    int i = min_block_size;  // skip min block size
    if (len < min_block_size) {
        return len;
    }
    uint64_t length = std::min(len, max_block_size);
    uint64_t first_phase = std::min(length, avg_block_size);

    for (; i < first_phase; i++) {
        fp = (fp << 1) + GEAR_TABLE[data[i]];
        if ((fp & small_mask) == 0) {
            return i ;
        }
    }

    for (; i < length; i++) {
        fp = (fp << 1) + GEAR_TABLE[data[i]];
        if ((fp & large_mask) == 0) {
            return i;
        }
    }
}

FastCDC::~FastCDC() {}
