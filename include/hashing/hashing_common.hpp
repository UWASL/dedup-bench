/**
 * @file hashing_common.hpp
 * @author WASL
 * @brief Header defining Hash Technique interface and related common functions
 * @version 0.1
 * @date 2023-02-06
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef _COMMON_HASHING_
#define _COMMON_HASHING_

#include "chunking_common.hpp"

#include <string>
#include <vector>

class Hashing_Technique{
    /**
     * @brief Interface for all hashing techniques to follow.
     *        Currently, all hash values are stored as strings to support a wide variety of algorithms
     * 
     */
    public:

    std::string technique_name;
    // Return a hash value for a given File_Chunk
    virtual std::string hash_chunk(File_Chunk) = 0;
    
    // Hash all chunks in a given vector of chunks
    std::vector<std::string> hash_chunks(std::vector<File_Chunk>);

    // Virtual destructor to support delete on base class ptr
    virtual ~Hashing_Technique(){}
};

// Print all given hash values in list
void print_hashes(std::vector<std::string> hash_list);

// Write all given hash values in list to output file
void write_hashes_to_file(std::vector<std::string> hash_list, std::string out_file_path);

#endif