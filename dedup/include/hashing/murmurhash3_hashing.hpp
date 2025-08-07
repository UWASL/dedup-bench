//-----------------------------------------------------------------------------
// MurmurHash3 was written by Austin Appleby, and is placed in the public
// domain. The author hereby disclaims copyright to this source code.

#ifndef _MURMURHASH3_H_
#define _MURMURHASH3_H_

//-----------------------------------------------------------------------------
// Platform-specific functions and macros

// Microsoft Visual Studio

#if defined(_MSC_VER) && (_MSC_VER < 1600)

typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef unsigned __int64 uint64_t;

// Other compilers

#else	// defined(_MSC_VER)

#include <stdint.h>

#endif // !defined(_MSC_VER)

//-----------------------------------------------------------------------------

void MurmurHash3_x86_32  ( const void * key, int len, uint32_t seed, void * out );

void MurmurHash3_x86_128 ( const void * key, int len, uint32_t seed, void * out );

void MurmurHash3_x64_128 ( const void * key, int len, uint32_t seed, void * out );

//-----------------------------------------------------------------------------

#endif // _MURMURHASH3_H_

// Additions for dedup-bench
//-----------------------------------------------------------------------------

#include "hashing_common.hpp"

#define MURMURHASH3_DIGEST_LENGTH 16

class MurmurHash3_Hashing : public Hashing_Technique {
    /**
     * @brief Class to implement MurmurHash3 Hashing
     * This class implements the MurmurHash3 hashing technique.
     */
    public:
        // Function to hash a given chunk
        void hash_chunk(File_Chunk& file_chunk) override;
        MurmurHash3_Hashing() {
            technique_name = "MurmurHash3-Hashing";
        }
};
