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
        }else if(value == "rabins"){
            return ChunkingTech::RABINS;
        }
        else if(value == "ae"){
            return ChunkingTech::AE;
        }
        else if(value == "gear"){
            return ChunkingTech::GEAR;
        }
        else if(value == "fastcdc"){
            return ChunkingTech::FASTCDC;
        }
    } catch (const std::out_of_range&) {}
    throw ConfigError("The configuration file does not specify a valid chunking technique");
}

HashingTech Config::get_hashing_tech() const {
    try {
        std::string value = parser.get_property(HASHING_TECH);
        if (value == "sha1") {
            return HashingTech::SHA1;
        } else if (value == "sha256") {
            return HashingTech::SHA256;
        } else if (value == "md5") {
            return HashingTech::MD5;
        }
    } catch (const std::out_of_range&) {}
    throw ConfigError("The configuration file does not specify a valid hashing technique");
}

uint64_t Config::get_fc_size() const {
    try {
        std::string value = parser.get_property(FC_SIZE);
        return std::stoull(value);
    } catch (const std::out_of_range&) {}
    catch (const std::invalid_argument&) {}
    throw ConfigError("The configuration file does not specify a valid size for fixed size chunking");
}

uint64_t Config::get_rabinc_window_size() const {
    try {
        std::string value = parser.get_property(RABINC_WINDOW_SIZE);
        return std::stoull(value);
    } catch (const std::out_of_range&) {}
    catch (const std::invalid_argument&) {}
    throw ConfigError("The configuration file does not specify a valid size for the sliding window");
}

uint64_t Config::get_rabinc_min_block_size() const {
    try {
        std::string value = parser.get_property(RABINC_MIN_BLOCK_SIZE);
        return std::stoull(value);
    } catch (const std::out_of_range&) {}
    catch (const std::invalid_argument&) {}
    throw ConfigError("The configuration file does not specify a valid minimum block size");
}

uint64_t Config::get_rabinc_avg_block_size() const {
    try {
        std::string value = parser.get_property(RABINC_AVG_BLOCK_SIZE);
        return std::stoull(value);
    } catch (const std::out_of_range&) {}
    catch (const std::invalid_argument&) {}
    throw ConfigError("The configuration file does not specify a valid avarage block size");
}

uint64_t Config::get_rabinc_max_block_size() const {
    try {
        std::string value = parser.get_property(RABINC_MAX_BLOCK_SIZE);
        return std::stoull(value);
    } catch (const std::out_of_range&) {}
    catch (const std::invalid_argument&) {}
    throw ConfigError("The configuration file does not specify a valid maximum block size");
}

std::string Config::get_output_file() const {
    try {
        std::string value = parser.get_property(OUTPUT_FILE);
        return value;
    } catch (const std::out_of_range&) {}
    return "hashes.out";
}

uint64_t Config::get_ae_avg_block_size() const {
    try {
        std::string value = parser.get_property(AE_AVG_BLOCK_SIZE);
        return std::stoull(value);
    } catch (const std::out_of_range&) {}
    catch (const std::invalid_argument&) {}
    throw ConfigError("The configuration file does not specify a valid AE avarage block size");
}

AE_Mode Config::get_ae_extreme_mode() const {
    try {
        std::string value = parser.get_property(AE_EXTREME_MODE);
        if (value == "min"){
           return AE_Mode::MIN;
        }else if(value == "max"){
           return AE_Mode::MAX;
        }
    } catch (const std::out_of_range&) {}
    catch (const std::invalid_argument&) {}
    throw ConfigError("The configuration file does not specify a valid AE extreme mode");
}

uint64_t Config::get_gear_min_block_size() const {
    try {
        std::string value = parser.get_property(GEAR_MIN_BLOCK_SIZE);
        return std::stoull(value);
    } catch (const std::out_of_range&) {}
    catch (const std::invalid_argument&) {}
    throw ConfigError("The configuration file does not specify a valid gear minimum block size for gear hash");
}

uint64_t Config::get_gear_max_block_size() const {
    try {
        std::string value = parser.get_property(GEAR_MAX_BLOCK_SIZE);
        return std::stoull(value);
    } catch (const std::out_of_range&) {}
    catch (const std::invalid_argument&) {}
    throw ConfigError("The configuration file does not specify a valid gear maximum block size for gear hash");
}

uint64_t Config::get_gear_avg_block_size() const {
    try {
        std::string value = parser.get_property(GEAR_AVG_BLOCK_SIZE);
        return std::stoull(value);
    } catch (const std::out_of_range&) {}
    catch (const std::invalid_argument&) {}
    throw ConfigError("The configuration file does not specify a valid gear maximum block size for gear hash");
}

uint64_t Config::get_fastcdc_min_block_size() const {
    try {
        std::string value = parser.get_property(FASTCDC_MIN_BLOCK_SIZE);
        return std::stoull(value);
    } catch (const std::out_of_range&) {}
    catch (const std::invalid_argument&) {}
    throw ConfigError("The configuration file does not specify a valid fastcdc minimum block size");
}

uint64_t Config::get_fastcdc_max_block_size() const {
    try {
        std::string value = parser.get_property(FASTCDC_MAX_BLOCK_SIZE);
        return std::stoull(value);
    } catch (const std::out_of_range&) {}
    catch (const std::invalid_argument&) {}
    throw ConfigError("The configuration file does not specify a valid fastcdc maximum block size");
}

uint64_t Config::get_fastcdc_avg_block_size() const {
    try {
        std::string value = parser.get_property(FASTCDC_AVG_BLOCK_SIZE);
        return std::stoull(value);
    } catch (const std::out_of_range&) {}
    catch (const std::invalid_argument&) {}
    throw ConfigError("The configuration file does not specify a valid fastcdc avarge block size");
}

uint64_t Config::get_fastcdc_normalization_level() const {
    try {
        std::string value = parser.get_property(FASTCDC_NORMALIZATION_LEVEL);
        return std::stoull(value);
    } catch (const std::out_of_range&) {}
    catch (const std::invalid_argument&) {}
    throw ConfigError("The configuration file does not specify a valid fastcdc normaliztion level");
}

bool Config::get_fastcdc_disable_normalization() const {
    try {
        std::string value = parser.get_property(FASTCDC_DISABLE_NORMALIZATION);
        return value== "true" ? true : false;
    } catch (const std::out_of_range&) {}
    catch (const std::invalid_argument&) {}
    throw ConfigError("The configuration file does not specify a valid fastcdc disable normalization option");
}