#ifndef _AE_CHUNKING_
#define _AE_CHUNKING_

#include <math.h>
#include <iostream>

#include "chunking_common.hpp"
#include "config.hpp"

#define DEFAULT_AE_AVG_BLOCK_SIZE 4096
#define BUFFER_SIZE 65535

class AE_Chunking : public virtual Chunking_Technique {
   private:
    uint64_t avg_block_size;
    uint64_t window_size;
    uint64_t curr_pos;
    AE_Mode extreme_mode;

    /**
     * @brief check if a file exits in the path
     * @param file_path:  path of the file
     * @return: whether the file exits.
     */
    bool check_file_exits(std::string file_path);

    /**
     * @brief tries to open a file for reading
     * @param file_path:  path of the file
     * @param file:  pointer to the file
     */
    bool read_file(std::string file_path, std::ifstream& file);

    /**
     * @brief gets the size of a file 
     * @param file:  pointer to the file
     * @return size of the file
     */
    uint64_t get_file_size(std::ifstream& file);

    /**
     * @brief compares a new value with the current extreme and returns whether
     * the new value is a new extreme value or not. (extreme value can be
     * maximum or minimum depnding on the MODE field)
     * @param new_val: the new value to test
     * @param current_extr: the current extreme value
     * @return: whether the new value is an extreme value or not
     */
    bool is_extreme(uint64_t new_val, uint64_t current_extr);

    /**
     * @brief finds the next cut point in an array of bytes
     * @param buff: the buff to find the cutpoint in.
     * @param size: the size of the buffer
     * @return: cutpoint position in the buffer
     */
    uint64_t find_cutpoint(char* buff, uint64_t size);

   public:
    /**
     * @brief Default constructor.
     * @return: void
     */
    AE_Chunking();

    /**
     * @brief Constructor with custom config from a config object
     * @param config: the config object
     * @return: void
     */
    AE_Chunking(const Config& config);

    /**
        @brief Divides a file into chunks using ae algorithm
       these chunks
        @param file_path: Path to input file
        @param chunk_size: Size of each chunk

        @return: Vector containing fixed size chunks from file
    */
    std::vector<File_Chunk> chunk_file(std::string file_path) override;
};

#endif