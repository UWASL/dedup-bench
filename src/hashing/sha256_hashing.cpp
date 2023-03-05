#include "sha256_hashing.hpp"
#include "hash.hpp"
#include <string>
#include <openssl/sha.h>

Hash SHA256_Hashing::hash_chunk(File_Chunk file_chunk) {
    Hash hash{HashingTech::SHA256, 32};

    SHA256((const unsigned char*)file_chunk.chunk_data, file_chunk.chunk_size, hash.getHash());
    return hash;
}