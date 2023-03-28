#include "md5_hashing.hpp"
#include "hash.hpp"
#include <string>
#include <openssl/md5.h>
#include <utility>

void MD5_Hashing::hash_chunk(File_Chunk& file_chunk) {
    file_chunk.init_hash(HashingTech::MD5, MD5_DIGEST_LENGTH);

    MD5((const unsigned char*)file_chunk.get_data(), file_chunk.get_size(), file_chunk.get_hash());
    return;
}
