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
     * 
     */
    public:

    std::string technique_name;

    // Return a hash value for a given File_Chunk
    virtual void hash_chunk(File_Chunk& file_chunk) = 0;
    
    /**
     * @brief Hash all chunks in a given vector using the relevant hash_chunk() implementation
     * @param file_chunks: Vector containing struct File_Chunk
     * 
     * @return: void
     */
    void hash_chunks(std::vector<File_Chunk>& file_chunks);

    // Virtual destructor to support delete on base class ptr
    virtual ~Hashing_Technique() {}
};

#endif