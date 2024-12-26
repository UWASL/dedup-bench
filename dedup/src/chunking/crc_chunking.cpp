/**
 * @file SS_CRC_Chunking.cpp
 * @author WASL
 * @brief Native Implementation for CRC chunking from SS-CDC
 * @version 0.1
 * @date 2023-3-10
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <crc_chunking.hpp>
#include <fstream>

/**
 * @brief: Default Constructor
*/

SS_CRC_Chunking::SS_CRC_Chunking(){    
    avg_block_size = DEFAULT_SS_CRC_AVG_BLOCK_SIZE;
    technique_name = "SS-CRC Chunking";
    min_block_size = avg_block_size / 4;
    max_block_size = avg_block_size * 4;
    window_size = 256;
    hash_bits = 14;
    simd_mode = SIMD_Mode::NONE;

    boundary_candidates_bitmask = nullptr;
    total_size_chunked = 0;
}

/**
 * @brief: Parameterized constructor
*/
SS_CRC_Chunking::SS_CRC_Chunking(const Config &config){
    avg_block_size = config.get_crc_avg_block_size();
    max_block_size = config.get_crc_max_block_size();
    min_block_size = config.get_crc_min_block_size();
    window_size = config.get_crc_window_size();
    hash_bits = config.get_crc_hash_bits();

    technique_name = "SS CRC Chunking";
 
    simd_mode = config.get_simd_mode();

    boundary_candidates_bitmask = nullptr;
    total_size_chunked = 0;
}

/**
 * @brief: Returns a single chunk boundary
 * @param buff: Char * array of bytes
 * @param size: Size of buff
*/

uint64_t SS_CRC_Chunking::find_cutpoint(char* buff, uint64_t size){
    if(simd_mode == SIMD_Mode::NONE)
        return find_cutpoint_native(buff, size);
    
    else{
        std::cout << "SIMD mode unsupported for chosen chunking technique" << std::endl;
        exit(1);
    }
        
}

/**
 * @brief: Returns a chunk boundary using native CRC implementation from SS-CDC
 * @param buff: Char * array of bytes
 * @param size: Size of buff
*/

uint64_t SS_CRC_Chunking::find_cutpoint_native(char* buff, uint64_t size){

    if(size < min_block_size){
        return size;
    }
    
    uint32_t hash = 0;

    // Break mask is decided using hash bits - Influences avg chunk size
    // Was set to 12, 13, 14 by SS-CDC for avg sizes of 4K, 8K and 16K
    uint32_t break_mask = (1u << hash_bits) - 1;

    for(uint64_t i = min_block_size - window_size; i < min_block_size; i++)
        hash = (hash >> 8) ^ crct[(hash ^ buff[i]) & 0xff];
    
    
    for(uint64_t i = min_block_size; i < size; i++){

        // Core CRC routine
        hash ^= crcu[(uint8_t)buff[i - window_size]];
        hash = (hash >> 8) ^ crct[(hash ^ buff[i]) & 0xff];

        if((hash & break_mask) == ss_crc_magic_number){
            total_size_chunked += i;
            return i;
        }
    }

    total_size_chunked += size;
    return size;
}

static inline int is_breakpoint(uint8_t* bitmask, uint64_t idx){
	uint8_t b = bitmask[idx/8];
	return ((b>>(idx%8)) & 0x1);
}


SS_CRC_Chunking::~SS_CRC_Chunking(){
}