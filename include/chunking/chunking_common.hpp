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
#ifndef _COMMON_CHUNKING_
#define _COMMON_CHUNKING_

#include <iostream>
#include <string>
#include <cstring>
#include <vector>

typedef struct File_Chunk{
    /**
     * @brief Structure representing file chunks. Common structure across all chunking techniques
     * 
     */

    // Chunk Size
    uint64_t chunk_size;

    // Chunk Data
    const char *chunk_data;

    // Constructor
    File_Chunk(char *_chunk_data, uint32_t _chunk_size){
        chunk_size = _chunk_size;
        chunk_data = _chunk_data;
    }

}File_Chunk;

class Chunking_Technique{
    /**
     * @brief Interface for all chunking techniques
     * 
     */
    public:
        std::string technique_name;
        virtual std::vector<File_Chunk> chunk_file(std::string file_path) = 0;
        /**
         * @brief Chunk a file using a chunking technique and return the struct File_Chunks from this operation
         * @param file_path: String containing path to file
         * @return: Vector of struct File_Chunk
         */

        virtual ~Chunking_Technique(){}

};

// Function to print all given chunks
void print_chunks(std::vector<File_Chunk> file_chunks);
void cleanup_chunks(std::vector<File_Chunk> file_chunks);

#endif