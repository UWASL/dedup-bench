#ifndef _CONFIG_
#define _CONFIG_
#include "parser.hpp"

#define CHUNKING_TECH "chunking_algo"
#define HASHING_TECH "hashing_algo"
#define FC_SIZE "fc_size"
#define WINDOW_SIZE "window_size"
#define MIN_BLOCK_SIZE "min_block_size"
#define AVG_BLOCK_SIZE "avg_block_size"
#define MAX_BLOCK_SIZE "max_block_size"



// define the possible chunking algorithms
enum class ChunkingTech { FILE, FIXED, RABINS };
// define the possible hashing algorithms
enum class HashingTech { SHA1 };


class Config {
    const Parser parser;

    public:
        Config(std::string config_file_path);

        /**
         * @brief Get the chunking algorithm specified in the config file.
         * throws ConfigError if the key does not exist or if the value is invalid
         * 
         * @return ChunkingTech 
         */
        ChunkingTech get_chunking_tech() const;

        /**
         * @brief Get the hashing algorithm specified in the config file.
         * throws ConfigError if the key does not exist or if the value is invalid
         * 
         * @return HashingTech 
         */
        HashingTech get_hashing_tech() const;

        /**
         * @brief Get the size (in number of bytes) of a chunk when using fixed-size chunking
         * throws ConfigError if the key does not exist or if the value is invalid
         * 
         * @return HashingTech 
         */
        uint64_t get_fc_size() const;


        /**
         * @brief Get the size of the sliding window when using any sliding window chunking technique
         * throws ConfigError if the key does not exist or if the value is invalid
         * 
         * @return HashingTech 
         */
        uint64_t get_window_size() const;
        
        /**
         * @brief Get the minimum size of a block for CDC  techniques
         * throws ConfigError if the key does not exist or if the value is invalid
         * 
         * @return HashingTech 
         */
        uint64_t get_min_block_size() const;

        /**
         * @brief Get the desired avg size of the block when using CDC techniques
         * throws ConfigError if the key does not exist or if the value is invalid
         * 
         * @return HashingTech 
         */
        uint64_t get_avg_block_size() const;


        /**
         * @brief Get the maximum size of a block that can be reached before inserting a cut point
         * throws ConfigError if the key does not exist or if the value is invalid
         * 
         * @return HashingTech 
         */
        uint64_t get_max_block_size() const;

};

#endif
