#ifndef _RABINS_CHUNKING_
#define _RABINS_CHUNKING_

#include "chunking_common.hpp"
#include "config.hpp"
#include "rabins_hashing.hpp"

#define DEFAULT_WINDOW_SIZE 32
#define DEFAULT_MIN_BLOCK_SIZE 1024
#define DEFAULT_AVG_BLOCK_SIZE 8192
#define DEFAULT_MAX_BLOCK_SIZE 65536

class Rabins_Chunking : public virtual Chunking_Technique {
    /**
     * @brief Class implementing rabin's based chunking
     *
     */

   private:
    uint64_t avg_block_size;
    uint64_t max_block_size;
    uint64_t min_block_size;
    uint64_t window_size;

    uint64_t fingerprint_mask;

    void init();
    void reset_stream();
    size_t rp_stream_read(unsigned char *dst, size_t size);
    int rp_block_next();

    int error;

    unsigned char *inbuf;    // input buffer
    size_t inbuf_size;       // size of input buffer
    size_t inbuf_data_size;  // size of valid data in input buffer

    size_t block_streampos;     // block start position in input stream
    unsigned char *block_addr;  // starting address of current block
    size_t block_size;          // size of the current block

    FILE *stream;

    Rabins_Hashing *r_hash;



   public:
    Rabins_Chunking() {
        /**
         * @brief Default constructor. defines all parameters to defualt values
         * @return: void
         */
        init();
    }

    Rabins_Chunking(const Config &config) {
        init();
        min_block_size = config.get_min_block_size();
        avg_block_size = config.get_avg_block_size();
        max_block_size = config.get_max_block_size();

        window_size = config.get_window_size();
    }

    // Implementation of chunk_file from Chunking_Technique
    std::vector<File_Chunk> chunk_file(std::string file_path);
};

#endif