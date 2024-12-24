#include "config.hpp"

#include <sstream>
#include <string>

#include "config_error.hpp"
#include "parser.hpp"

Config::Config(std::string config_file_path) : parser{config_file_path} {}

ChunkingTech Config::get_chunking_tech() const {
    try {
        std::string value = parser.get_property(CHUNKING_TECH);
        if (value == "fixed") {
            return ChunkingTech::FIXED;
        } else if (value == "rabins") {
            return ChunkingTech::RABINS;
        } else if (value == "ae") {
            return ChunkingTech::AE;
        } else if (value == "gear") {
            return ChunkingTech::GEAR;
        } else if (value == "fastcdc") {
            return ChunkingTech::FASTCDC;
        } else if (value == "ram") {
            return ChunkingTech::RAM;
        } else if (value == "experiment") {
            return ChunkingTech::EXPERIMENT;
        } else if (value == "crc") {
            return ChunkingTech::CRC;
        } else if (value == "seq") {
            return ChunkingTech::SEQ;
        } else if (value == "tttd"){
            return ChunkingTech::TTTD;
        }
    } catch (...) {
    }
    throw ConfigError(
        "The configuration file does not specify a valid chunking technique");
}

HashingTech Config::get_hashing_tech() const {
    try {
        std::string value = parser.get_property(HASHING_TECH);
        if (value == "sha1") {
            return HashingTech::SHA1;
        } else if (value == "sha256") {
            return HashingTech::SHA256;
        } else if (value == "sha512") {
            return HashingTech::SHA512;
        } else if (value == "md5") {
            return HashingTech::MD5;
        }
    } catch (...) {
    }
    throw ConfigError(
        "The configuration file does not specify a valid hashing technique");
}

SIMD_Mode Config::get_simd_mode() const {
    try {
        std::string value = parser.get_property(SIMD_MODE_STRING);
        if (value == "none") {
            return SIMD_Mode::NONE;
        } else if (value == "sse128") {
            return SIMD_Mode::SSE128;
        } else if (value == "avx256") {
            return SIMD_Mode::AVX256;
        } else if (value == "avx512") {
            if(__builtin_cpu_supports("avx512f"))
                return SIMD_Mode::AVX512;
            else
                throw ConfigError(
                    "Invalid SIMD Mode in configuration file: AVX-512 not supported by CPU");
        } else if (value == "sse128_noslide"){
            return SIMD_Mode::SSE128_NOSLIDE;
        }
    } catch (...) {
    }
    throw ConfigError(
        "The configuration file does not specify a valid SIMD mode");
}

uint64_t Config::get_fc_size() const {
    try {
        std::string value = parser.get_property(FC_SIZE);
        return std::stoull(value);
    } catch (...) {
    }
    throw ConfigError(
        "The configuration file does not specify a valid size for fixed size "
        "chunking");
}

uint64_t Config::get_rabinc_window_size() const {
    try {
        std::string value = parser.get_property(RABINC_WINDOW_SIZE);
        return std::stoull(value);
    } catch (...) {
    }
    throw ConfigError(
        "The configuration file does not specify a valid size for the sliding "
        "window");
}

uint64_t Config::get_rabinc_min_block_size() const {
    try {
        std::string value = parser.get_property(RABINC_MIN_BLOCK_SIZE);
        return std::stoull(value);
    } catch (...) {
    }
    throw ConfigError(
        "The configuration file does not specify a valid minimum block size");
}

uint64_t Config::get_rabinc_avg_block_size() const {
    try {
        std::string value = parser.get_property(RABINC_AVG_BLOCK_SIZE);
        return std::stoull(value);
    } catch (...) {
    }
    throw ConfigError(
        "The configuration file does not specify a valid avarage block size");
}

uint64_t Config::get_rabinc_max_block_size() const {
    try {
        std::string value = parser.get_property(RABINC_MAX_BLOCK_SIZE);
        return std::stoull(value);
    } catch (...) {
    }
    throw ConfigError(
        "The configuration file does not specify a valid maximum block size");
}

std::string Config::get_output_file() const {
    try {
        std::string value = parser.get_property(OUTPUT_FILE);
        return value;
    } catch (...) {
    }
    return "hashes.out";
}

uint64_t Config::get_ae_avg_block_size() const {
    try {
        std::string value = parser.get_property(AE_AVG_BLOCK_SIZE);
        return std::stoull(value);
    } catch (...) {
    }
    throw ConfigError(
        "The configuration file does not specify a valid AE avarage block "
        "size");
}

AE_Mode Config::get_ae_extreme_mode() const {
    try {
        std::string value = parser.get_property(AE_EXTREME_MODE);
        if (value == "min") {
            return AE_Mode::MIN;
        } else if (value == "max") {
            return AE_Mode::MAX;
        }
    } catch (...) {
    }
    throw ConfigError(
        "The configuration file does not specify a valid AE extreme mode");
}

uint64_t Config::get_gear_min_block_size() const {
    try {
        std::string value = parser.get_property(GEAR_MIN_BLOCK_SIZE);
        return std::stoull(value);
    } catch (...) {
    }
    throw ConfigError(
        "The configuration file does not specify a valid gear minimum block "
        "size for gear hash");
}

uint64_t Config::get_gear_max_block_size() const {
    try {
        std::string value = parser.get_property(GEAR_MAX_BLOCK_SIZE);
        return std::stoull(value);
    } catch (...) {
    }
    throw ConfigError(
        "The configuration file does not specify a valid gear maximum block "
        "size for gear hash");
}

uint64_t Config::get_gear_avg_block_size() const {
    try {
        std::string value = parser.get_property(GEAR_AVG_BLOCK_SIZE);
        return std::stoull(value);
    } catch (...) {
    }
    throw ConfigError(
        "The configuration file does not specify a valid gear maximum block "
        "size for gear hash");
}

uint64_t Config::get_fastcdc_min_block_size() const {
    try {
        std::string value = parser.get_property(FASTCDC_MIN_BLOCK_SIZE);
        return std::stoull(value);
    } catch (...) {
    }
    throw ConfigError(
        "The configuration file does not specify a valid fastcdc minimum block "
        "size");
}

uint64_t Config::get_fastcdc_max_block_size() const {
    try {
        std::string value = parser.get_property(FASTCDC_MAX_BLOCK_SIZE);
        return std::stoull(value);
    } catch (...) {
    }
    throw ConfigError(
        "The configuration file does not specify a valid fastcdc maximum block "
        "size");
}

uint64_t Config::get_fastcdc_avg_block_size() const {
    try {
        std::string value = parser.get_property(FASTCDC_AVG_BLOCK_SIZE);
        return std::stoull(value);
    } catch (...) {
    }
    throw ConfigError(
        "The configuration file does not specify a valid fastcdc avarge block "
        "size");
}

uint64_t Config::get_fastcdc_normalization_level() const {
    try {
        std::string value = parser.get_property(FASTCDC_NORMALIZATION_LEVEL);
        return std::stoull(value);
    } catch (...) {
    }
    throw ConfigError(
        "The configuration file does not specify a valid fastcdc normaliztion "
        "level");
}

bool Config::get_fastcdc_disable_normalization() const {
    try {
        std::string value = parser.get_property(FASTCDC_DISABLE_NORMALIZATION);
        return value == "true" ? true : false;
    } catch (...) {
    }
    throw ConfigError(
        "The configuration file does not specify a valid fastcdc disable "
        "normalization option");
}

uint64_t Config::get_ram_avg_block_size() const {
    try {
        std::string value = parser.get_property(RAM_AVG_BLOCK_SIZE);
        return std::stoull(value);
    } catch (...) {
    }
    throw ConfigError(
        "The configuration file does not specify a valid RAM avarage block "
        "size");
}

uint64_t Config::get_ram_max_block_size() const {
    try {
        std::string value = parser.get_property(RAM_MAX_BLOCK_SIZE);
        return std::stoull(value);
    } catch (...) {
    }
    throw ConfigError(
        "The configuration file does not specify a valid RAM max block size");
}

uint64_t Config::get_crc_avg_block_size() const {
    try {
        std::string value = parser.get_property(CRC_AVG_BLOCK_SIZE);
        return std::stoull(value);
    } catch (...) {
    }
    throw ConfigError(
        "The configuration file does not specify a valid crc average block "
        "size");
}

uint64_t Config::get_crc_min_block_size() const {
    try {
        std::string value = parser.get_property(CRC_MIN_BLOCK_SIZE);
        return std::stoull(value);
    } catch (...) {
    }
    throw ConfigError(
        "The configuration file does not specify a valid crc minimum block "
        "size");
}

uint64_t Config::get_crc_max_block_size() const {
    try {
        std::string value = parser.get_property(CRC_MAX_BLOCK_SIZE);
        return std::stoull(value);
    } catch (...) {
    }
    throw ConfigError(
        "The configuration file does not specify a valid crc maximum block "
        "size");
}

uint64_t Config::get_crc_window_size() const {
    try {
        std::string value = parser.get_property(CRC_WINDOW_SIZE);
        return std::stoull(value);
    } catch (...) {
    }
    throw ConfigError(
        "The configuration file does not specify a valid crc window size");
}

uint64_t Config::get_crc_window_step() const {
    try {
        std::string value = parser.get_property(CRC_WINDOW_STEP_SIZE);
        return std::stoull(value);
    } catch (...) {
    }
    throw ConfigError(
        "The configuration file does not specify a valid crc window step size");
}

uint32_t Config::get_crc_hash_bits() const {
    try {
        std::string value = parser.get_property(CRC_HASH_BITS);
        return std::stoull(value);
    } catch (...) {
    }
    throw ConfigError(
        "The configuration file does not specify a valid crc hash bit number");
}

uint64_t Config::get_buffer_size() const {
    try {
        std::string value = parser.get_property(BUFFER_SIZE);
        return std::stoull(value);
    } catch (...) {
    }
    throw ConfigError(
        "The configuration file does not specify a valid buffer size");
}

uint64_t Config::get_seq_jump_trigger() const {
    try {
        std::string value = parser.get_property(SEQ_JUMP_TRIGGER);
        return std::stoull(value);
    } catch (...) {
    }
    throw ConfigError(
        "The configuration file does not specify a valid seq jump trigger "
        "value");
}

uint64_t Config::get_seq_threshold() const {
    try {
        std::string value = parser.get_property(SEQ_CHUNK_BOUNDARY_THRESHOLD);
        return std::stoull(value);
    } catch (...) {
    }
    throw ConfigError(
        "The configuration file does not specify a valid seq sequence "
        "threshold");
}

uint64_t Config::get_seq_min_block_size() const {
    try {
        std::string value = parser.get_property(SEQ_MIN_BLOCK_SIZE);
        return std::stoull(value);
    } catch (...) {
    }
    throw ConfigError(
        "The configuration file does not specify a valid seq minimum block "
        "size");
}

uint64_t Config::get_seq_max_block_size() const {
    try {
        std::string value = parser.get_property(SEQ_MAX_BLOCK_SIZE);
        return std::stoull(value);
    } catch (...) {
    }
    throw ConfigError(
        "The configuration file does not specify a valid seq maximum block "
        "size");
}

uint64_t Config::get_seq_avg_block_size() const {
    try {
        std::string value = parser.get_property(SEQ_AVG_BLOCK_SIZE);
        return std::stoull(value);
    } catch (...) {
    }
    throw ConfigError(
        "The configuration file does not specify a valid seq average block "
        "size");
}

uint64_t Config::get_seq_jump_size() const {
    try {
        std::string value = parser.get_property(SEQ_JUMP_SIZE);
        return std::stoull(value);
    } catch (...) {
    }
    throw ConfigError(
        "The configuration file does not specify a valid max seq jump size");
}

Seq_Op_Mode Config::get_seq_op_mode() const {
    try {
        std::string value = parser.get_property(SEQ_OPERATION_MODE);
        if (value == "inc")
            return Seq_Op_Mode::INCREASING;
        else if (value == "dec")
            return Seq_Op_Mode::DECREASING;
    } catch (...) {
    }
    throw ConfigError(
        "The configuration file does not specify a valid seq operation mode");
}

uint64_t Config::get_exp_window_size() const {
    try {
        std::string value = parser.get_property(EXP_WINDOW_SIZE);
        return std::stoull(value);
    } catch (...) {
    }
    throw ConfigError(
        "The configuration file does not specify a valid window size");
}

uint64_t Config::get_exp_little_window_size() const {
    try {
        std::string value = parser.get_property(EXP_LITTLE_WINDOW_SIZE);
        return std::stoull(value);
    } catch (...) {
    }
    throw ConfigError(
        "The configuration file does not specify a valid little window size");
}

uint64_t Config::get_exp_jump_threshold() const {
    try {
        std::string value = parser.get_property(EXP_JUMP_THRESHOLD);
        return std::stoull(value);
    } catch (...) {
    }
    throw ConfigError(
        "The configuration file does not specify a valid jump threshold");
}

uint64_t Config::get_exp_jump_amount() const {
    try {
        std::string value = parser.get_property(EXP_JUMP_AMOUNT);
        return std::stoull(value);
    } catch (...) {
    }
    throw ConfigError(
        "The configuration file does not specify a valid jump amount");
}

uint64_t Config::get_exp_min_block_size() const {
    try {
        std::string value = parser.get_property(EXP_MIN_BLOCK_SIZE);
        return std::stoull(value);
    } catch (...) {
    }
    throw ConfigError(
        "The configuration file does not specify a valid min block size");
}

uint64_t Config::get_tttd_avg_block_size() const {
    try {
        std::string value = parser.get_property(TTTD_AVG_BLOCK_SIZE);
        return std::stoull(value);
    } catch (...) {
    }
    throw ConfigError(
        "The configuration file does not specify a valid TTTD average block "
        "size");
}

uint64_t Config::get_tttd_min_block_size() const {
    try {
        std::string value = parser.get_property(TTTD_MIN_BLOCK_SIZE);
        return std::stoull(value);
    } catch (...) {
    }
    throw ConfigError(
        "The configuration file does not specify a valid TTTD min block "
        "size");
}

uint64_t Config::get_tttd_max_block_size() const {
    try {
        std::string value = parser.get_property(TTTD_MAX_BLOCK_SIZE);
        return std::stoull(value);
    } catch (...) {
    }
    throw ConfigError(
        "The configuration file does not specify a valid TTTD max block "
        "size");
}
