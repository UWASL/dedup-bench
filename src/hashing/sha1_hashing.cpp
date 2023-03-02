#include "sha1_hashing.hpp"
#include "hash.hpp"
#include <string>
#include <openssl/sha.h>

Hash SHA1_Hashing::hash_chunk(File_Chunk file_chunk) {
    Hash hash{HashingTech::SHA1, 20};

    SHA1((const unsigned char*)file_chunk.chunk_data, file_chunk.chunk_size, hash.getHash());
    return hash;
}