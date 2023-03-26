#include "sha1_hashing.hpp"
#include "hash.hpp"
#include <string>
#include <openssl/sha.h>

void SHA1_Hashing::hash_chunk(File_Chunk& file_chunk) {
    file_chunk.init_hash(HashingTech::SHA1, SHA_DIGEST_LENGTH);

    SHA1((const unsigned char*)file_chunk.get_data(), file_chunk.get_size(), file_chunk.get_hash());
    return;
}