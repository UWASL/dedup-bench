/**
 * @file chunking.cpp
 * @author WASL
 * @brief Implementations for fixed chunking technique
 * @version 0.1
 * @date 2023-01-31
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "fixed_chunking.hpp"
#include <fstream>


bool Fixed_Chunking::set_fixed_chunk_size(uint64_t _chunk_size){
    Fixed_Chunking::fixed_chunk_size = _chunk_size;
    if(fixed_chunk_size == _chunk_size)
        return true;
    else
        return false;
}

uint64_t Fixed_Chunking::get_fixed_chunk_size(){
    return Fixed_Chunking::fixed_chunk_size;
}

uint64_t Fixed_Chunking::find_cutpoint(char *, uint64_t size) {
    return std::min(size, fixed_chunk_size);
}