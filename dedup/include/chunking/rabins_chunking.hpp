#ifndef _RABINS_CHUNKING_
#define _RABINS_CHUNKING_

#include <errno.h>
#include <stdint.h>

#include <cstring>
#include <fstream>

#include "chunking_common.hpp"
#include "config.hpp"
#include "rabins_hashing.hpp"

#define DEFAULT_RABINC_WINDOW_SIZE 32
#define DEFAULT_RABINC_MIN_BLOCK_SIZE 1024
#define DEFAULT_RABINC_AVG_BLOCK_SIZE 8192
#define DEFAULT_RABINC_MAX_BLOCK_SIZE 65536

#define POLYNOMIAL 0x3DA3358B4DC173LL
#define POLYNOMIAL_DEGREE 53
#define POL_SHIFT (POLYNOMIAL_DEGREE - 8)

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

    uint8_t *window;
    unsigned int wpos;
    unsigned int count;
    unsigned int pos;
    unsigned int start;
    uint64_t digest;
    uint64_t window_size;

    bool tables_initialized;
    uint64_t mod_table[256];
    uint64_t out_table[256];


    /**
     * @brief initialize rabin chunking algorithm
     */
    void rabin_init(void);

    /**
     * @brief reset all internal buffers and counters
     */
    void rabin_reset();

    /**
     * @brief insert new byte b to the window
     */
    void rabin_slide(uint8_t b);

    /**
     * @brief adds a new byte to hash
     */
    void rabin_append(uint8_t b);

    /**
     * @brief finds the next chunk boundary
     * @return: the index after the chunk boundry relative to the buffer
     */
    uint64_t find_cutpoint(char *buf,uint64_t len);


    /**
     * @brief calculates the mod of x to p
     * @return: mod of x to p
     */
    uint64_t mod(uint64_t x, uint64_t p);

    /**
     * @brief calculates the degree of the polynomyal p
     * @return: the degree of the polynomyal
     */
    int deg(uint64_t p);

    /**
     * @brief appends byte b to the hash using polynomyal pol
     * @return: the new hash
     */
    uint64_t append_byte(uint64_t hash, uint8_t b, uint64_t pol);

    /**
     * @brief initlize mod tables 
     * @return: void
     */
    void calc_tables(void);

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
     * @brief Destructor to free the internal buffer
     * @return: void
     */
    ~Rabins_Chunking();

};

#endif