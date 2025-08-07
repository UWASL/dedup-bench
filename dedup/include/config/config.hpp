
#ifndef _CONFIG_
#define _CONFIG_
#include "parser.hpp"

#include <cstdint>

#define CHUNKING_TECH "chunking_algo"
#define HASHING_TECH "hashing_algo"
#define CHUNKING_MODE "chunking_mode"
#define SIMD_MODE_STRING "simd_mode"

#define FC_SIZE "fc_size"
#define RABINC_WINDOW_SIZE "rabinc_window_size"
#define RABINC_MIN_BLOCK_SIZE "rabinc_min_block_size"
#define RABINC_AVG_BLOCK_SIZE "rabinc_avg_block_size"
#define RABINC_MAX_BLOCK_SIZE "rabinc_max_block_size"
#define OUTPUT_FILE "output_file"
#define AE_AVG_BLOCK_SIZE "ae_avg_block_size"
#define AE_EXTREME_MODE "ae_extreme_mode"
#define GEAR_MIN_BLOCK_SIZE "gear_min_block_size"
#define GEAR_MAX_BLOCK_SIZE "gear_max_block_size"
#define GEAR_AVG_BLOCK_SIZE "gear_avg_block_size"
#define FASTCDC_MIN_BLOCK_SIZE "fastcdc_min_block_size"
#define FASTCDC_MAX_BLOCK_SIZE "fastcdc_max_block_size"
#define FASTCDC_AVG_BLOCK_SIZE "fastcdc_avg_block_size"
#define FASTCDC_NORMALIZATION_LEVEL "fastcdc_normalization_level"
#define FASTCDC_DISABLE_NORMALIZATION "fastcdc_disable_normalization"
#define RAM_AVG_BLOCK_SIZE "ram_avg_block_size"
#define RAM_MAX_BLOCK_SIZE "ram_max_block_size"
#define CRC_MIN_BLOCK_SIZE "crc_min_block_size"
#define CRC_MAX_BLOCK_SIZE "crc_max_block_size"
#define CRC_AVG_BLOCK_SIZE "crc_avg_block_size"
#define CRC_WINDOW_SIZE "crc_window_size"
#define CRC_WINDOW_STEP_SIZE "crc_window_step_size"
#define CRC_HASH_BITS "crc_hash_bits"
#define BUFFER_SIZE "buffer_size"
#define MAXP_WINDOW_SIZE "maxp_window_size"
#define MAXP_MAX_BLOCK_SIZE "maxp_max_block_size"
#define SEQ_JUMP_TRIGGER "seq_jump_trigger"
#define SEQ_CHUNK_BOUNDARY_THRESHOLD "seq_sequence_threshold"
#define SEQ_JUMP_SIZE "seq_jump_size"
#define SEQ_OPERATION_MODE "seq_op_mode"
#define SEQ_MIN_BLOCK_SIZE "seq_min_block_size"
#define SEQ_MAX_BLOCK_SIZE "seq_max_block_size"
#define SEQ_AVG_BLOCK_SIZE "seq_avg_block_size"
#define TTTD_MIN_BLOCK_SIZE "tttd_min_block_size"
#define TTTD_MAX_BLOCK_SIZE "tttd_max_block_size"
#define TTTD_AVG_BLOCK_SIZE "tttd_avg_block_size"

#define EXP_MIN_BLOCK_SIZE "exp_min_block_size"
#define EXP_JUMP_THRESHOLD "exp_jump_threshold"
#define EXP_JUMP_AMOUNT "exp_jump_amount"
#define EXP_WINDOW_SIZE "exp_window_size"
#define EXP_LITTLE_WINDOW_SIZE "exp_little_window_size"

// define the possible chunking algorithms
enum class ChunkingTech {
    FILE,
    FIXED,
    RABINS,
    AE,
    GEAR,
    FASTCDC,
    MAXP,
    RAM,
    EXPERIMENT,
    CRC,
    SEQ,
    TTTD,
};

enum class SIMD_Mode{
    NONE,
    SSE128_NOSLIDE,
    SSE128,
    AVX256,
    AVX512,
    NEON,
    ALTIVEC
};

// define the possible hashing algorithms
enum class HashingTech { MD5, SHA1, SHA256, SHA512, XXHASH128, MURMURHASH3 };

// define the the extreme value type of AE algorithm
enum AE_Mode { MAX, MIN };

// define SeqCDC operating modes
enum Seq_Op_Mode { INCREASING, DECREASING };


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
     * @brief Get the chunking mode specified in the config file.
     * throws ConfigError if the key does not exist or if the value is invalid
     *
     * @return ChunkingMode
     */
    HashingTech get_hashing_tech() const;

    
    /**
     * @brief Get the SIMD mode for chunking technique
     * 
     * @return SIMD_Mode
     */
    SIMD_Mode get_simd_mode() const;
    
    /**
     * @brief Get the size (in number of bytes) of a chunk when using fixed-size
     * chunking throws ConfigError if the key does not exist or if the value is
     * invalid
     *
     * @return uint64_t
     */

    uint64_t get_fc_size() const;

    /**
     * @brief Get the size of the sliding window when using any sliding window
     * chunking technique throws ConfigError if the key does not exist or if the
     * value is invalid
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
     * @brief Get the maximum size of a block that can be reached before
     * inserting a cut point throws ConfigError if the key does not exist or if
     * the value is invalid
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

    /**
     * @brief Get the desired minimum size of the block when using Gear chunking
     * throws ConfigError if the key does not exist or if the value is invalid
     *
     * @return gear desired avg block size
     */
    uint64_t get_gear_min_block_size() const;

    /**
     * @brief Get the desired avarqage size of the block when using Gear
     * chunking throws ConfigError if the key does not exist or if the value is
     * invalid
     *
     * @return gear desired avg block size
     */
    uint64_t get_gear_avg_block_size() const;

    /**
     * @brief Get the desired maximum size of the block when using Gear chunking
     * throws ConfigError if the key does not exist or if the value is invalid
     *
     * @return gear desired avg block size
     */
    uint64_t get_gear_max_block_size() const;

    /**
     * @brief Get the desired minimum size of the block when using FASTCDC
     * chunking throws ConfigError if the key does not exist or if the value is
     * invalid
     *
     * @return fastcdc desired avg block size
     */
    uint64_t get_fastcdc_min_block_size() const;

    /**
     * @brief Get the desired avarqage size of the block when using FASTCDC
     * chunking throws ConfigError if the key does not exist or if the value is
     * invalid
     *
     * @return fastcdc desired avg block size
     */
    uint64_t get_fastcdc_avg_block_size() const;

    /**
     * @brief Get the desired maximum size of the block when using FASTCDC
     * chunking throws ConfigError if the key does not exist or if the value is
     * invalid
     *
     * @return fastcdc desired avg block size
     */
    uint64_t get_fastcdc_max_block_size() const;

    /**
     * @brief Get chunk normalization level. It may be set to 1, 2 or 3,
     * unless DisableNormalization is true, in which case it's ignored
     *
     * @return fastcdc desired normalization level
     */
    uint64_t get_fastcdc_normalization_level() const;

    /**
     * @brief turns normalization off if true
     *
     * @return disable normalization state
     */
    bool get_fastcdc_disable_normalization() const;

    /**
     * @brief Get the desired avg size of the block when using RAM chunking
     * throws ConfigError if the key does not exist or if the value is invalid
     *
     * @return RAM desired avg block size
     */
    uint64_t get_ram_avg_block_size() const;

    /**
     * @brief Get the desired max size of the block when using RAM chunking
     * throws ConfigError if the key does not exist or if the value is invalid
     *
     * @return RAM desired avg block size
     */
    uint64_t get_ram_max_block_size() const;

    /**
     * @brief Get the desired minimum size of the block when using crc chunking
     * throws ConfigError if the key does not exist or if the value is invalid
     *
     * @return crc desired avg block size
     */

    uint64_t get_crc_min_block_size() const;

    /**
     * @brief Get the desired average size of the block when using crc chunking
     * throws ConfigError if the key does not exist or if the value is invalid
     *
     * @return fastcdc desired avg block size
     */
    uint64_t get_crc_avg_block_size() const;

    /**
     * @brief Get the desired maximum size of the block when using crc chunking
     * throws ConfigError if the key does not exist or if the value is invalid
     *
     * @return crc desired avg block size
     */
    uint64_t get_crc_max_block_size() const;

    /**
     * @brief Get the window size when using crc chunking
     * throws ConfigError if the key does not exist or if the value is invalid
     *
     * @return crc window size
     */

    uint64_t get_crc_window_size() const;

    /**
     * @brief Get the window step size when using crc chunking
     * throws ConfigError if the key does not exist or if the value is invalid
     *
     * @return crc window step size
     */

    uint64_t get_crc_window_step() const;

    /**
     * @brief Get the number of hash bits for comparison when using crc chunking
     * throws ConfigError if the key does not exist or if the value is invalid
     *
     * @return crc hash bits
     */

    uint32_t get_crc_hash_bits() const;

    /**
     * @brief Get the desired internal buffer size
     * throws ConfigError if the key does not exist or
     * if the value is invalid
     * @return the desired internal buffer size
     */

    uint64_t get_seq_jump_trigger() const;

    /**
     * @brief Get number of opposing slopes needed to trigger jumps for seq
     * chunking
     *
     * @return Trigger for jumping in seq chunking
     */

    uint64_t get_seq_threshold() const;
    /**
     * @brief Get number of values required in sequence to trigger chunk
     * boundary insertion
     *
     * @return Threshold for chunk boundary in seq chunking
     */

    uint64_t get_seq_jump_size() const;
    /**
     * @brief Get seq jump size
     *
     * @return Pointer to seq jump size
     */

    Seq_Op_Mode get_seq_op_mode() const;
    /**
     * @brief Get seq operation mode
     *
     * @return enum Seq_Op_Mode
     */

    uint64_t get_seq_min_block_size() const;
    uint64_t get_seq_avg_block_size() const;
    uint64_t get_seq_max_block_size() const;

    /**
     * @brief Get the min, avg and max block sizes for seq chunking
     *
     * @return uint64_t
     */
    uint64_t get_buffer_size() const;

    /**
    * @brief Get the desired window size
    * throws ConfigError if the key does not exist or
    * if the value is invalid
    * @return the desired window size
    */
    uint64_t get_exp_window_size() const;

    /**
     * @brief Get the desired little window size
     * throws ConfigError if the key does not exist or
     * if the value is invalid
     * @return the desired little window size
     */
    uint64_t get_exp_little_window_size() const;

    /**
     * @brief Get the desired jump_threshold for experiment chunking
     * throws ConfigError if the key does not exist or
     * if the value is invalid
     * @return the desired jump_threshold for experiment chunking
     */
    uint64_t get_exp_jump_threshold() const;

    /**
     * @brief Get the desired jump amount
     * throws ConfigError if the key does not exist or
     * if the value is invalid
     * @return the desired jump amount
     */
    uint64_t get_exp_jump_amount() const;

    /**
     * @brief Get the desired min block size
     * throws ConfigError if the key does not exist or
     * if the value is invalid
     * @return the desired min block size
     */
    uint64_t get_exp_min_block_size() const;

    uint64_t get_tttd_min_block_size() const;
    uint64_t get_tttd_avg_block_size() const;
    uint64_t get_tttd_max_block_size() const;

     /**
     * @brief Get the min, avg and max block sizes for TTTD chunking
     *
     * @return uint64_t
     */

    uint64_t get_maxp_window_size() const;
    uint64_t get_maxp_max_block_size() const;
     
     /**
      * @brief: Get MAXP window size
      * 
      * @return uint64_t
      */
};

#endif
