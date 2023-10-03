#include "sha512_hashing.hpp"
#include "hash.hpp"
#include <string>
#include <openssl/sha.h>

void SHA512_Hashing::hash_chunk(File_Chunk& file_chunk) {
    file_chunk.init_hash(HashingTech::SHA512, SHA512_DIGEST_LENGTH);

    SHA512((const unsigned char*)file_chunk.get_data(), file_chunk.get_size(), file_chunk.get_hash());
    return;
}
