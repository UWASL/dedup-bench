/**
 * @file chunking.hpp
 * @author WASL
 * @brief Declarations for all chunking techniques
 * @version 0.1
 * @date 2023-01-31
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef _COMMON_CHUNKING_
#define _COMMON_CHUNKING_

#include <string>
#include <vector>
#include <memory>
#include "hash.hpp"
#include "config.hpp"


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
        File_Chunk(uint32_t _chunk_size);

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
         */
        void print() const;
};

class Chunking_Technique{
    /**
     * @brief Interface for all chunking techniques
     * 
     */

    public:
        std::string technique_name;
        virtual std::vector<File_Chunk> chunk_file(std::string file_path) = 0;
        /**
         * @brief Chunk a file using a chunking technique and return the struct File_Chunks from this operation
         * @param file_path: String containing path to file
         * @return: Vector of struct File_Chunk
         */

        virtual ~Chunking_Technique() {};
};

#endif
