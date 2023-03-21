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
#include <tuple>
#include "hash.hpp"


class Hashing_Technique{
    /**
     * @brief Interface for all hashing techniques to follow.
     *        Currently, all hash values are stored as strings to support a wide variety of algorithms
     * 
     */
    public:

    std::string technique_name;
    // Return a hash value for a given File_Chunk
    virtual Hash hash_chunk(File_Chunk) = 0;
    
    // Hash all chunks in a given vector of chunks, and return a vector of hash and chunk size
    std::vector<std::tuple<Hash, uint64_t>> hash_chunks(std::vector<File_Chunk>);

    // Virtual destructor to support delete on base class ptr
    virtual ~Hashing_Technique() {}
};

// Print all given hash values in list
void print_hashes(std::vector<Hash> hash_list);

#endif