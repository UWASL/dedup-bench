#ifndef _RAM_Chunking_
#define _RAM_Chunking_

#include <math.h>
#include <iostream>

#include "avx_chunking_common.hpp"
#include "config.hpp"

#include <cstring>

#define DEFAULT_RAM_AVG_BLOCK_SIZE 4096

class RAM_Chunking : public virtual AVX_Chunking_Technique {
   private:
    uint64_t avg_block_size;
    uint64_t max_block_size;
    uint64_t window_size;
    uint64_t curr_pos;

    __m128i *sse_array;
    __m256i *avx256_array;
    __m512i *avx512_array;

    /**
     * @brief finds the next cut point in an array of bytes
     * @param buff: the buff to find the cutpoint in.
     * @param size: the size of the buffer
     * @return: cutpoint position in the buffer 
     */
    uint64_t find_cutpoint(char* buff, uint64_t size);

    /**
     * @brief Get the return position (chunk boundary) for RAM using SSE128 instructions. This is the first position after start_position with a value > max_val.
     * 
     * @param buff Buffer
     * @param start_position Starting position of scan within buffer
     * @param max_value Max value within fixed size window
     * @return uint64_t Return position in stream
     */
    uint64_t get_return_position_sse128(char *buff, uint64_t start_position, uint64_t end_position, uint8_t max_value);
    uint64_t get_return_position_avx256(char *buff, uint64_t start_position, uint64_t end_position, uint8_t max_value);
    #if defined(__AVX512F__)
        uint64_t get_return_position_avx512(char *buff, uint64_t start_position, uint64_t end_position, uint8_t max_value);
    #endif

    
   public:
    /**
     * @brief Default constructor.
     * @return: void
     */
    RAM_Chunking();

    /**
     * @brief Constructor with custom config from a config object
     * @param config: the config object
     * @return: void
     */
    RAM_Chunking(const Config& config);

    ~RAM_Chunking();

};

#endif