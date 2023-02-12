#include "parser.hpp"
#include "config_error.hpp"
#include <iostream>
#include <fstream>

// trim from left
inline std::string ltrim(std::string s, const char* t = " \t\n\r\f\v")
{
    s.erase(0, s.find_first_not_of(t));
    return s;
}

// trim from right
inline std::string rtrim(std::string s, const char* t = " \t\n\r\f\v")
{
    s.erase(s.find_last_not_of(t) + 1);
    return s;
}

// trim from left & right
inline std::string trim(std::string s, const char* t = " \t\n\r\f\v")
{
    return ltrim(rtrim(s, t), t);
}

Parser::Parser(const std::string& config_file_path) {
	// read the config file
	std::ifstream infile(config_file_path);
	if (infile.fail()) {
		throw ConfigError("Failed to open config file");
	}

	std::string line;
	while(std::getline(infile, line)) {
		// skip empty lines or comment lines
		if (line.length() == 0 || line[0] == comment) {
			continue;
		}
		size_t idx = line.find(delimiter);
		dict[trim(line.substr(0, idx))] = trim(line.substr(idx+1, line.length()-idx-1));
  	}
}

std::string Parser::get_property(const std::string& key) {
	return dict.at(key);
}

ChunkingTech Parser::get_chunking_tech() {
	auto it = dict.find(CHUNKING_TECH);
	if (it != dict.end()) {
		std::string value = it->second;
		if (value == "fixed") {
			return ChunkingTech::FIXED;
		}
	}
	return ChunkingTech::UNKNOWN;
}

HashingTech Parser::get_hashing_tech() {
	auto it = dict.find(HASHING_TECH);
	if (it != dict.end()) {
		std::string value = it->second;
		if (value == "std") {
			return HashingTech::STD;
		} else if (value == "fnv") {
			return HashingTech::FNV;
		}
	}
	return HashingTech::UNKNOWN;
}

void Parser::print() {
	for (auto it = dict.begin(); it != dict.end(); ++it) {
		std::cout << it->first << " = " << it->second << std::endl;
	}
}