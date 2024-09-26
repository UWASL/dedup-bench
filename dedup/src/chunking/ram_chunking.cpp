/**
 * @file RAM_Chunking.cpp
 * @author WASL
 * @brief Implementations for AE chunking technique
 * @version 0.1
 * @date 2023-3-10
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "ram_chunking.hpp"
#include <fstream>

RAM_Chunking::RAM_Chunking() {
    avg_block_size = DEFAULT_RAM_AVG_BLOCK_SIZE;
    technique_name = "RAM Chunking";

    window_size = avg_block_size - 256;
}

RAM_Chunking::RAM_Chunking(const Config& config) {
    avg_block_size = config.get_ram_avg_block_size();
    max_block_size = config.get_ram_max_block_size();
    window_size = avg_block_size - 256;
    // window_size = avg_block_size / (exp(1) - 1);  // avg_block size / e-1
    
    technique_name = "RAM Chunking";
}

RAM_Chunking::~RAM_Chunking() {
}

uint64_t RAM_Chunking::find_cutpoint(char* buff, uint64_t size) {
    uint32_t i = 0;
    uint8_t max_value = (uint8_t)buff[i];
    i++;
    
    // Max size check
    if (size > max_block_size)
        size = max_block_size;

    // Min Size == window_size
    else if(size < window_size)
        return size;

    // Find max in window
    for(i = 0; i < window_size; i++){
        if ((uint8_t)buff[i] >= max_value)
            max_value = (uint8_t)buff[i];
        }

    // Return first byte outside window >= max
    for (i = window_size; i < size; i++) {
        if ((uint8_t)buff[i] >= max_value)
            return i;
    }

    return size;
}
