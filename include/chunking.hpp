/**
 * @file chunking.hpp
 * @author WASL
 * @brief Declarations for all chunking techniques
 * @version 0.1
 * @date 2023-01-31
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef _HEADER_CHUNKING_
#define _HEADER_CHUNKING_

#include <iostream>
#include <string>
#include <vector>

#define DEFAULT_FIXED_CHUNK_SIZE 4096

#endif


typedef struct File_Chunk{
    /**
     * @brief Structure representing file chunks. Common structure across all chunking techniques
     * 
     */

    // Chunk Size
    uint64_t chunk_size;

    // Chunk Data
    char *chunk_data;

    // Constructor
    File_Chunk(char *_chunk_data, uint32_t _chunk_size){
        chunk_size = _chunk_size;
        chunk_data = _chunk_data;
    }
   
}File_Chunk;

class Chunking_Techniques{
    /**
     * @brief Class to hold all supported chunking techniques
     * 
     */
   
    public:
        static uint64_t fixed_chunk_size;

        static bool set_fixed_chunk_size(uint64_t _chunk_size);
        static uint64_t get_fixed_chunk_size();
        static std::vector<File_Chunk> fixed_size_chunks(std::string file_path);
};

void print_chunks(std::vector<File_Chunk> file_chunks);