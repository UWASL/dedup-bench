/**
 * @file chunking_common.cpp
 * @author WASL
 * @brief Implementations of functions common across all chunking techniques
 * @version 0.1
 * @date 2023-02-02
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "chunking.hpp"

void print_chunks(std::vector<File_Chunk> file_chunks){
    /**
     * @brief Print details of each chunk in the given vector of chunks
     * 
     * @param file_chunks: vector containing struct File_Chunk
     */

    // Iterate over vector elements and print details
    for(File_Chunk chunk:file_chunks){
        std::cout << "\tChunk Size: " <<  chunk.chunk_size << std::endl;
        std::cout << "\tChunk Data: " << chunk.chunk_data << std::endl;
        std::cout << std::endl << std::endl;
    }
}