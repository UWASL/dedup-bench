#ifndef _CONFIG_
#define _CONFIG_
#include "parser.hpp"

#define CHUNKING_TECH "chunking_algo"
#define HASHING_TECH "hashing_algo"
#define FC_SIZE "fc_size"

// define the possible chunking algorithms
enum class ChunkingTech { FILE, FIXED };
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
};

#endif
