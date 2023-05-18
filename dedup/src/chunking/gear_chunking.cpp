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

#include "gear_chunking.hpp"

#include <cmath>
#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>

Gear_Chunking::Gear_Chunking(const Config& config) {
    min_block_size = config.get_gear_min_block_size();
    max_block_size = config.get_gear_max_block_size();
    avg_block_size = config.get_gear_avg_block_size();

    int number_of_ones = log2(avg_block_size);

    // to produce chunks that are 8KB. Using the
    // formula log2(8192) = 13, we can use a mask with the 13 most-significant
    // bits set to 1
    mask = 0x0000000000000000;
    for (int i = 0; i < 64; i++) {
        if (number_of_ones-- >= 0) {
            mask += 1;
        }
        mask = mask << 1;
    }
}

uint64_t Gear_Chunking::ghash(uint64_t h, unsigned char ch) {
    return ((h << 1) + GEAR_TABLE[ch]);
}

uint64_t Gear_Chunking::find_cutpoint(char* data, uint64_t size) {
    uint64_t hash = 0;
    uint64_t idx = min_block_size;

    // If given data is lower than the minimum chunk size, return data length.
    if (size <= min_block_size) {
        return size;
    }

    while (idx < size && idx < max_block_size) {
        hash = ghash(hash, data[idx]);
        if (!(hash & mask)) {
            return idx;
        }
        idx += 1;
    }

    return idx;
}

