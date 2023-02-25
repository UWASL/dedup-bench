#include "config.hpp"
#include "parser.hpp"
#include "config_error.hpp"
#include <string>


Config::Config(std::string config_file_path): parser{config_file_path} {}

ChunkingTech Config::get_chunking_tech() const {
	try {
		std::string value = parser.get_property(CHUNKING_TECH);
		if (value == "fixed") {
			return ChunkingTech::FIXED;
		}
	} catch (const std::out_of_range) {}
	throw ConfigError("The configuration file does not specify a valid chunking technique");
}

HashingTech Config::get_hashing_tech() const {
	try {
		std::string value = parser.get_property(HASHING_TECH);
		if (value == "std") {
			return HashingTech::STD;
		} else if (value == "fnv") {
			return HashingTech::FNV;
		} else if (value == "sha1") {
			return HashingTech::SHA1;
		}
	} catch (const std::out_of_range) {}
	throw ConfigError("The configuration file does not specify a valid hashing technique");
}

uint64_t Config::get_fc_size() const {
	try {
		std::string value = parser.get_property(FC_SIZE);
		return std::stoull(value);
	} catch (const std::out_of_range) {}
	catch (const std::invalid_argument) {}
	throw ConfigError("The configuration file does not specify a valid size for fixed size chunking");
}
