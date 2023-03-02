#ifndef _RABINS_CHUNKING_
#define _RABINS_CHUNKING_

#include "chunking_common.hpp"
#include "config.hpp"
#include "rabins_hashing.hpp"

#define DEFAULT_WINDOW_SIZE 32
#define DEFAULT_MIN_BLOCK_SIZE 1024
#define DEFAULT_AVG_BLOCK_SIZE 8192
#define DEFAULT_MAX_BLOCK_SIZE 65536
#define DEFAULT_BUFFER_SIZE 65536

class Rabins_Chunking : public virtual Chunking_Technique {
    /**
     * @brief Class implementing rabin's based chunking
     *
     */

   private:
    uint64_t avg_block_size;
    uint64_t max_block_size;
    uint64_t min_block_size;
    uint64_t fingerprint_mask;

    int error;

    unsigned char *inbuf;    // input buffer
    size_t inbuf_pos;        // current position in input buffer
    size_t inbuf_size;       // size of input buffer
    size_t inbuf_data_size;  // size of valid data in input buffer

    size_t block_streampos;     // block start position in input stream
    unsigned char *block_addr;  // starting address of current block
    size_t block_size;          // size of the current block
    int shift;

    FILE *stream;

    Rabins_Hashing *r_hash;

   public:
    Rabins_Chunking() {
        /**
         * @brief Default constructor. Initializes fixed_chunk_size to
         * DEFAULT_CHUNK_SIZE
         * @return: void
         */
        min_block_size = DEFAULT_MIN_BLOCK_SIZE;
        avg_block_size = DEFAULT_AVG_BLOCK_SIZE;
        max_block_size = DEFAULT_MAX_BLOCK_SIZE;

        inbuf_size = max_block_size * 10;

        fingerprint_mask = (1 << (fls32(avg_block_size) - 1)) - 1;

        inbuf = (unsigned char *)malloc(inbuf_size * sizeof(unsigned char));

        r_hash = new Rabins_Hashing();

        technique_name = "Rabins Chunking";
    }

    Rabins_Chunking(const Config &config) {
        technique_name = "Rabins Chunking";
        // TODO: add config to config parser
    }

    bool set_min_block_size(uint64_t _min_block_size);
    uint64_t get_min_block_size();

    bool set_avg_block_size(uint64_t _avg_block_size);
    uint64_t get_avg_block_size();

    bool set_max_block_size(uint64_t _max_block_size);
    uint64_t get_max_block_size();

    bool set_buffer_size(uint64_t _buffer_size);
    uint64_t get_buffer_size();

    bool init_stream();

    size_t rp_stream_read(unsigned char *dst, size_t size);

    int rp_block_next();
    // Implementation of chunk_file from Chunking_Technique
    std::vector<File_Chunk> chunk_file(std::string file_path);
};

#endif