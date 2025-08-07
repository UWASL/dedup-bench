#include "xxhash_hashing.hpp"
#include "hash.hpp"
#include <cstring>

#define XXH_STATIC_LINKING_ONLY /* access advanced declarations */
#define XXH_IMPLEMENTATION      /* access definitions */

void XXHash_Hashing::hash_chunk(File_Chunk& file_chunk) {
    file_chunk.init_hash(HashingTech::XXHASH128, XXH128_DIGEST_LENGTH);

    XXH128_hash_t hash_value =  XXH3_128bits((const unsigned char*)file_chunk.get_data(), file_chunk.get_size());
    BYTE* hash_ptr = file_chunk.get_hash();
    std::memcpy(hash_ptr, &hash_value, XXH128_DIGEST_LENGTH);
    return;
}
