#include "sha1_hashing.hpp"
#include "base64.hpp"
#include <string>
#include <openssl/sha.h>

std::string SHA1_Hashing::hash_chunk(File_Chunk file_chunk) {
	BYTE hash[20];

	SHA1((const unsigned char*)file_chunk.chunk_data, file_chunk.chunk_size, hash);
	return base64_encode(hash, 20);
}