/**
 * @file hashing_common.cpp
 * @author WASL
 * @brief File to hold implementations of functions common across hashing techniques
 * @version 0.1
 * @date 2023-02-06
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "hashing_common.hpp"
#include "hash.hpp"


void Hashing_Technique::hash_chunks(std::vector<File_Chunk>& file_chunks){
    // Iterate over all chunks and generate their hash values
    for (File_Chunk& fc: file_chunks) {
        hash_chunk(fc);
    }
}
