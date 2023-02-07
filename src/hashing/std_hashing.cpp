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

std::vector<std::string> Std_Hashing::hash_chunks(std::vector<File_Chunk> file_chunks){
    /**
     * @brief Hash all chunks in a given vector using the relevant hash_chunk() implementation
     * @param file_chunks: Vector containing struct File_Chunk
     * @return: Vector of hashes (strings)
     */
    std::vector<std::string> hash_vector;

    // Iterate over all chunks and generate hash values
    for(File_Chunk fc: file_chunks){
        std::string chunk_hash = hash_chunk(fc);
        hash_vector.push_back(chunk_hash);
    }

    return hash_vector; 
}