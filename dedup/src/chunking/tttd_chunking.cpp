/**
 * @file tttd_chunking.cpp
 * @author WASL
 * @brief Implementations for two threshold two divisor technique
 * @version 0.1
 * @date 2023-3-10
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <fstream>
#include "tttd_chunking.hpp"

/**
 * @brief: Constructor with config. 
 * @return: None
*/
TTTD_Chunking::TTTD_Chunking(const Config & config): Rabins_Chunking(config){
    avg_block_size = config.get_tttd_avg_block_size();
    min_block_size = config.get_tttd_min_block_size();
    max_block_size = config.get_tttd_max_block_size();

    rabin_mask = (1 << (fls32(avg_block_size) - 1)) - 1;
    backup_mask = (1 << (fls32(avg_block_size/2) - 1)) - 1;
    
}

/**
 * @brief: Implements two threshold, two-divisor algorithm
 * @return: Cut point value
*/
uint64_t TTTD_Chunking::find_cutpoint(char *buff, uint64_t size){
    if(size < min_block_size)
        return size;
    
    uint64_t end_pos = (size < max_block_size) ? size: max_block_size;
    uint64_t last_backup_pos = 0;

    for (pos = min_block_size; pos <= end_pos; pos++){
        
        rabin_slide(buff[pos]);
        if((digest & backup_mask) == 0){
            last_backup_pos = pos;
            if((digest & rabin_mask) == 0){
                rabin_reset();
                return pos;
            }
        }
    }
    rabin_reset();
    if(last_backup_pos >= min_block_size)
        return last_backup_pos;
    else
        return size;
}

TTTD_Chunking::~TTTD_Chunking(){};