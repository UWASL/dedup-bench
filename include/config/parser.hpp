#ifndef _PARSER_
#define _PARSER_
#include <string>
#include <unordered_map>

#define CHUNKING_TECH "chunking_algo"
#define HASHING_TECH "hashing_algo"

// define the possible chunking algorithms
enum class ChunkingTech { UNKNOWN, FILE, FIXED };
// define the possible hashing algorithms
enum class HashingTech { UNKNOWN, STD, FNV, MD5, SHA1, SHA2 };


class Parser {
	const char delimiter = '=';
	const char comment = '#';
	std::unordered_map<std::string, std::string> dict;

	public:
		Parser(const std::string& config_file_path);

		/**
		 * @brief Get the value of the given key if it exist.
		 * throws std::out_of_range if key does not exist in the dictionary
		 * @param key 
		 * @return std::string 
		 */
		std::string get_property(const std::string& key);

		ChunkingTech get_chunking_tech();
		HashingTech get_hashing_tech();

		/**
		 * @brief Prints out all the stored key value mapping
		 */
		void print();
};

#endif