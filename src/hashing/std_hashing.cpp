#include "std_hashing.hpp"

#include <unordered_map>
#include <string>

std::string Std_Hashing::hash_chunk(File_Chunk file_chunk)
{
    /**
     * @brief Hash a chunk using std::hash. In G++, MurmurHashUnaligned2 by Austin Applesby is used by default.
     * @param file_chunk: File_Chunk struct
     */
    std::string hash_data = std::string(file_chunk.chunk_data);

    // Generate hash value using std::hash
    ssize_t hash_value = std::hash<std::string>{}(hash_data);
    
    return std::to_string(hash_value);
}
