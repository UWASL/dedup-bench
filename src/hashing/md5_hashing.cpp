#include "md5_hashing.hpp"
#include "hash.hpp"
#include <string>
#include <openssl/md5.h>

Hash MD5_Hashing::hash_chunk(File_Chunk file_chunk) {
    Hash hash{HashingTech::MD5, MD5_DIGEST_LENGTH};

    MD5((const unsigned char*)file_chunk.chunk_data, file_chunk.chunk_size, hash.getHash());
    return hash;
}
