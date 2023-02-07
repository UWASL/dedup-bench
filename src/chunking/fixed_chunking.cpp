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
    /**
     * @brief Sets fixed chunk size. Always call before using fixed chunking technique
     * @param _chunk_size: New fixed chunk size
     * @return: True if success, False otherwise
     */
    Fixed_Chunking::fixed_chunk_size = _chunk_size;

    if(fixed_chunk_size == _chunk_size)
        return true;
    else
        return false;
}

uint64_t Fixed_Chunking:: get_fixed_chunk_size(){
    /**
     * @brief Returns fixed chunk size
     * @return: uint64_t with fixed_chunk_size value
     */
    return Fixed_Chunking::fixed_chunk_size;
}

std::vector<File_Chunk> Fixed_Chunking::chunk_file(std::string file_path){
    /**
        @brief Divides a file into fixed size chunks and returns a vector with these chunks
        @param file_path: Path to input file
        @param chunk_size: Size of each chunk

        @return: Vector containing fixed size chunks from file   
    */


    std::ifstream file_ptr;
    file_ptr.open(file_path, std::ios::in);

    // Get chunk size from object
    uint64_t chunk_size = get_fixed_chunk_size();

    std::vector<File_Chunk> file_chunks;

    // Get size of file
    file_ptr.seekg(0,std::ios_base::end);
    uint64_t file_size_bytes = file_ptr.tellg();

    // Seek back to beginning and set up bytes_to_read
    file_ptr.seekg(0,std::ios_base::beg);
    uint64_t bytes_to_read = std::min(chunk_size, file_size_bytes);
    

    uint64_t curr_bytes_read = 0;
    while(curr_bytes_read < file_size_bytes){
        // Create buffer to store chunks
        char *block_buffer = new char[chunk_size];
   
        
        file_ptr.read(block_buffer, bytes_to_read);
        
        // Create new chunk and push it into vector
        File_Chunk new_chunk(block_buffer, bytes_to_read);
        file_chunks.push_back(new_chunk);

        curr_bytes_read += bytes_to_read;

        // Handles the last chunk being smaller than @param chunk_size
        bytes_to_read = std::min(chunk_size, file_size_bytes - curr_bytes_read);
    }

    return file_chunks;
}
