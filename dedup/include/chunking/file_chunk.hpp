#ifndef _FILE_CHUNK_
#define _FILE_CHUNK_

#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include <istream>
#include "hash.hpp"

class File_Chunk {
    /**
     * @brief Structure representing file chunks. Common structure across all chunking techniques
     * 
     */

    // Chunk Data
    std::unique_ptr<char[]> chunk_data;
    // Chunk Size
    uint64_t chunk_size;
    // Chunk Hash
    std::unique_ptr<Hash> chunk_hash;

    // Private constructor used by move constructor
    File_Chunk() {};

    public:
        // Constructors
        File_Chunk(uint64_t _chunk_size);

        File_Chunk(const File_Chunk& other);

        File_Chunk(File_Chunk&& other) noexcept;

        File_Chunk& operator=(const File_Chunk& other) = delete;

        File_Chunk& operator=(File_Chunk&& other) noexcept = delete;

        /**
         * @brief Allocate space for the hash of this chunk
         * 
         * @param hashing_tech 
         * @param size 
         */
        void init_hash(HashingTech hashing_tech, uint64_t size);

        /**
         * @brief Get the size of the chunk
         * 
         * @return uint64_t 
         */
        uint64_t get_size() const;

        /**
         * @brief Get the pointer to the allocated space for the chunk
         * 
         * @return char* 
         */
        char* get_data() const;

        /**
         * @brief Get the pointer to the allocated space for the hash of the chunk
         * 
         * @return BYTE* 
         */
        BYTE* get_hash() const;

        /**
         * @brief Returns the hash and size of the chunk as a string
         * 
         * @return std::string 
         */
        std::string to_string() const;

        /**
         * @brief Prints the details of the chunk
         * 
         * @return: void
         */
        void print() const;
};

#endif