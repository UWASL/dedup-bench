#ifndef _CONFIG_
#define _CONFIG_
#include "parser.hpp"

#define CHUNKING_TECH "chunking_algo"
#define HASHING_TECH "hashing_algo"
#define FC_SIZE "fc_size"
#define RABINC_WINDOW_SIZE "rabinc_window_size"
#define RABINC_MIN_BLOCK_SIZE "rabinc_min_block_size"
#define RABINC_AVG_BLOCK_SIZE "rabinc_avg_block_size"
#define RABINC_MAX_BLOCK_SIZE "rabinc_max_block_size"
#define OUTPUT_FILE "output_file"
#define AE_AVG_BLOCK_SIZE "ae_avg_block_size"
#define AE_EXTREME_MODE "ae_extreme_mode"


// define the possible chunking algorithms
enum class ChunkingTech { FILE, FIXED, RABINS, AE };
// define the possible hashing algorithms
enum class HashingTech { MD5, SHA1, SHA256 };
// define the the extreme value type of AE algorithm
enum AE_Mode { MAX, MIN };

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
        uint64_t get_rabinc_window_size() const;
        
        /**
         * @brief Get the minimum size of a block for CDC  techniques
         * throws ConfigError if the key does not exist or if the value is invalid
         * 
         * @return HashingTech 
         */
        uint64_t get_rabinc_min_block_size() const;

        /**
         * @brief Get the desired avg size of the block when using CDC techniques
         * throws ConfigError if the key does not exist or if the value is invalid
         * 
         * @return HashingTech 
         */
        uint64_t get_rabinc_avg_block_size() const;

        /**
         * @brief Get the maximum size of a block that can be reached before inserting a cut point
         * throws ConfigError if the key does not exist or if the value is invalid
         * 
         * @return HashingTech 
         */
        uint64_t get_rabinc_max_block_size() const;

        /**
         * @brief Get the file name to write the hashes to. Defaults to "hashes.out"
         * 
         * @return std::string
         */
        std::string get_output_file() const;
        
         /**
         * @brief Get the desired avg size of the block when using AE chunking
         * throws ConfigError if the key does not exist or if the value is invalid
         * 
         * @return AE desired avg block size   
         */
        uint64_t get_ae_avg_block_size() const;

        /**
         * @brief get the operating mode of ae (can be maximum or minimum)
         * throws ConfigError if the key does not exist or if the value is invalid
         * 
         * @return AE extreme mode (max or min)
         */
        AE_Mode get_ae_extreme_mode() const;
};

#endif