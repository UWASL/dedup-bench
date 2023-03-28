#ifndef _RABINS_CHUNKING_
#define _RABINS_CHUNKING_

#include "chunking_common.hpp"
#include "config.hpp"
#include "rabins_hashing.hpp"

#define DEFAULT_RABINC_WINDOW_SIZE 32
#define DEFAULT_RABINC_MIN_BLOCK_SIZE 1024
#define DEFAULT_RABINC_AVG_BLOCK_SIZE 8192
#define DEFAULT_RABINC_MAX_BLOCK_SIZE 65536

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

    /**
     * @brief initilize the needed varibles for chunking
     */
    void init();

    /**
     * @brief resets the current stream
     */
    void reset_stream();

    /**
     * @brief reads data from a stream and sotre it in dst param
     * @param dst pointer to array of char where the data will be stored
     * @param size the size of data to read

     * @return the number of bytes that has been read
     *
     */
    size_t rp_stream_read(unsigned char *dst, size_t size);

    /**
     * @brief searches for the next cut point
     * @return 0 if a cut point has been found. a positive value if error has
     * occured or eof has been reached (based on error flag)
     */
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

    /**
     * @brief Default constructor. defines all parameters to defualt values
     * @return: void
     */
    Rabins_Chunking();

    /**
     * @brief Defines all parameters based on values from the config file
     * @return: void
     */
    Rabins_Chunking(const Config &config);

    /**
     * @brief chunk a file using rabins algorithms
     * @param file_path the path of the file to be chunked
     * @return A vector of chunks
     */
    std::vector<File_Chunk> chunk_file(std::string file_path);
};

#endif