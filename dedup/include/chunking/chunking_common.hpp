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
#include <sstream>
#include <istream>
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

class Chunking_Technique{
    /**
     * @brief Interface for all chunking techniques
     * 
     */

    private:
        /**
         * @brief a helper function to create a chunk from a data buffer
         * @param file_chunks: chunks vector
         * @param data: the data buffer
         * @param buffer_end: the logical size of the buffer in bytes
         * @return: size of the chunk
         */
        int64_t create_chunk(std::vector<File_Chunk>& file_chunks, char* data, uint64_t buffer_end);

    public:
        std::string technique_name;

        /**
         * @brief Chunk a file using a chunking technique and return the struct File_Chunks from this operation
         * 
         * @param file_path: String containing path to file
         * @return: Vector of struct File_Chunk
         */
        virtual uint64_t find_cutpoint(char*, uint64_t) {};

        /**
         * @brief calculates the size of the given file
         * 
         * @param file_ptr: ifstream pointer to the file   
         * @return: the size of the file
         */
        uint64_t get_file_size(std::istream* file_ptr);

        /**
         * @brief Chunk a file using a chunking technique and return the struct File_Chunks from this operation
         * 
         * @param file_path: String containing path to file
         * @return: Vector of struct File_Chunk
         */
        std::vector<File_Chunk> chunk_file(std::string file_path);
        /**
         * @brief Chunk a stream using a chunking technique and append the struct File_Chunks from this operation
         * to the vector passed in
         * 
         * @param stream: input stream containing the data to be chunked
         * @return: void
         */
        virtual void chunk_stream(std::vector<File_Chunk>& result, std::istream& stream);

        virtual ~Chunking_Technique() {};

        /**
         * @brief Read every file in the directory and subdirectories into a buffer for each file and return it
         * 
         * @param dir_path : String containing path to the directory. Must be a valid directory path
         * @return std::vector<std::istringstream> 
         */
        static std::vector<std::unique_ptr<std::istream>> read_files_to_buffers(std::string dir_path);
};

#endif