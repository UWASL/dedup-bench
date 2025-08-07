/**
 * @author: WASL
 * @date: 2025-07-05
 * @brief: Header file for xxHash Hashing Technique
 * This file defines the XXHash_Hashing class which implements the Hashing_Technique interface.
 * It provides the functionality to hash file chunks using the xxHash algorithm.
 * Note: I could only find xxHash128 on GitHub (Cyan4973/xxHash).)
 */

#ifndef _XXHASH_HASHING_
#define _XXHASH_HASHING_

#define XXHASH_INLINE_ALL /* enable inlining for all functions */

#include "hashing_common.hpp"
#include "xxhash.h"

// Define the digest length for xxHash128
// This is the size of the hash output in bytes
#define XXH128_DIGEST_LENGTH 16

class XXHash_Hashing: public virtual Hashing_Technique{
    /**
     * @brief Class to implement xxHash Hashing
     *
     */
    public:
        // Function to hash a given chunk
        void hash_chunk(File_Chunk& file_chunk) override;

        XXHash_Hashing() {
            technique_name = "xxHash-Hashing";
        }
};

#endif

