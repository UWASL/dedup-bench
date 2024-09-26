/**
 * @file Crc_Chunking.cpp
 * @author WASL
 * @brief Implementations for crc chunking technique
 * @version 0.1
 * @date 2023-4-1
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "crc_chunking.hpp"

#include <cmath>
#include <cstring>
#include <fstream>
#include <iostream>

Crc_Chunking::Crc_Chunking(const Config& config) {
    min_block_size = config.get_crc_min_block_size();
    max_block_size = config.get_crc_max_block_size();
    avg_block_size = config.get_crc_avg_block_size();
    // Process a raw buffer.
    // TODO: use avg chunk size param

    window_size = config.get_crc_window_size();
    window_step = config.get_crc_window_step();

    uint32_t hash_bits = config.get_crc_hash_bits();

    mask = (1 << hash_bits) - 1;
}

uint64_t Crc_Chunking::find_cutpoint(char* data, uint64_t size) {
    
    // uint32_t shift = ((file_size-bytes_left) % step);
    if (size > max_block_size) size = max_block_size;
    if (size < min_block_size) return size;
    uint64_t offs = min_block_size;

    while (offs < size) {
        uint32_t digest = crc32c::Crc32c(&data[offs - window_size], window_size);
        if ((digest & mask) == 0) {
            return offs;
        }
        offs += window_step;
    }

    return size;
}
