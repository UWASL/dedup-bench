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
        if (value == "sha1") {
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

uint64_t Config::get_rabinc_window_size() const {
    try {
        std::string value = parser.get_property(RABINC_WINDOW_SIZE);
        return std::stoull(value);
    } catch (const std::out_of_range) {}
    catch (const std::invalid_argument) {}
    throw ConfigError("The configuration file does not specify a valid size for the sliding window");
}

uint64_t Config::get_rabinc_min_block_size() const {
    try {
        std::string value = parser.get_property(RABINC_MIN_BLOCK_SIZE);
        return std::stoull(value);
    } catch (const std::out_of_range) {}
    catch (const std::invalid_argument) {}
    throw ConfigError("The configuration file does not specify a valid minimum block size");
}

uint64_t Config::get_rabinc_avg_block_size() const {
    try {
        std::string value = parser.get_property(RABINC_AVG_BLOCK_SIZE);
        return std::stoull(value);
    } catch (const std::out_of_range) {}
    catch (const std::invalid_argument) {}
    throw ConfigError("The configuration file does not specify a valid avarage block size");
}

uint64_t Config::get_rabinc_max_block_size() const {
    try {
        std::string value = parser.get_property(RABINC_MAX_BLOCK_SIZE);
        return std::stoull(value);
    } catch (const std::out_of_range) {}
    catch (const std::invalid_argument) {}
    throw ConfigError("The configuration file does not specify a valid maximum block size");
}