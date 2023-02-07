#include "fnv_hashing.hpp"
#include <string>

std::string Fnv_Hashing::hash_chunk(File_Chunk file_chunk){
    /**
     * @brief Function implementing FNV Hashing (Fowler-Noll-Vo)
     *        Note that this hash function isn't cryptographically secure. Its just a fast placeholder technique for testing.
     * 
     *        This function can handle binary data.
     *    
     *        Algorithm Details:
     *              All integers are 64-bit unsigned
     *              Overflows do not matter as we only care about the lower 64 bits according to the algorithm itself
     *        
     *              Reference: https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
     *        
     *        Note: This function implements FNV-1 hashing
     * 
     * @param file_chunk: File chunk to be hashed
     * 
     */

    char *data_to_hash = file_chunk.chunk_data;
    uint64_t chunk_size = file_chunk.chunk_size;

    // Initialize hash value to FNV_Offset_Basis
    uint64_t chunk_hash = fnv_offset_basis;

    // For each byte, multiply hash by FNV_Offset_Prime and XOR with the corresponding byte.
    // Note: If multiplication overflows, we're still good since we only need the lower 64 bits of the result anyway
    for(uint64_t i=0; i < chunk_size; i++){
        chunk_hash = chunk_hash * fnv_prime;
        chunk_hash = chunk_hash ^ data_to_hash[i];
    }


    // Return hash value as string for storage
    return std::to_string(chunk_hash);
}