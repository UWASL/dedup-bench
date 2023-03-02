#include "config.hpp"

#include <string>

#include "config_error.hpp"
#include "parser.hpp"

Config::Config(std::string config_file_path) : parser{config_file_path} {}

ChunkingTech Config::get_chunking_tech() const {
    try {
        std::string value = parser.get_property(CHUNKING_TECH);
        if (value == "fixed") {
            return ChunkingTech::FIXED;
        }
        if (value == "rabins") {
            return ChunkingTech::RABINS;
        }
    } catch (const std::out_of_range) {
    }
    throw ConfigError(
        "The configuration file does not specify a valid chunking technique");
}

HashingTech Config::get_hashing_tech() const {
    try {
        std::string value = parser.get_property(HASHING_TECH);
        if (value == "std") {
            return HashingTech::STD;
        } else if (value == "fnv") {
            return HashingTech::FNV;
        }
    } catch (const std::out_of_range) {
    }
    throw ConfigError(
        "The configuration file does not specify a valid hashing technique");
}

uint64_t Config::get_fc_size() const {
    try {
        std::string value = parser.get_property(FC_SIZE);
        return std::stoull(value);
    } catch (const std::out_of_range) {
    } catch (const std::invalid_argument) {
    }
    throw ConfigError(
        "The configuration file does not specify a valid size for fixed size "
        "chunking");
}
