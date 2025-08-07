#ifndef _AE_CHUNKING_
#define _AE_CHUNKING_

#include <math.h>
#include <iostream>

#include "avx_chunking_common.hpp"
#include "config.hpp"

#include <cstring>

#define DEFAULT_AE_AVG_BLOCK_SIZE 4096

class AE_Chunking : public virtual AVX_Chunking_Technique {
   private:
    uint64_t avg_block_size;
    uint64_t window_size;
    uint64_t curr_pos;
    AE_Mode extreme_mode;

    /**
     * @brief SSE128, AVX256 and AVX512 arrays for SIMD operations
     * 
     */
    #if defined(__SSE3__)
    __m128i* sse_array;
    #endif

    #if defined(__AVX2__)
    __m256i* avx256_array;
    #endif

    #if defined(__AVX512F__)
    __m512i* avx512_array;
    #endif

    #if defined(__ARM_NEON)
    uint8x16_t* neon_array;
    #endif

    #if defined(__ALTIVEC__)
    __vector unsigned char* altivec_array;
    #endif

    /**
     * @brief finds the next cut point in an array of bytes
     * @param buff: the buff to find the cutpoint in.
     * @param size: the size of the buffer
     * @return: cutpoint position in the buffer 
     */
    uint64_t find_cutpoint(char* buff, uint64_t size) override;
    uint64_t find_cutpoint_native(char* buff, uint64_t size);
   
    #if defined(__SSE3__)
    uint64_t find_cutpoint_sse128(char* buff, uint64_t size);
    #endif
    
    #if defined(__AVX2__)
    uint64_t find_cutpoint_avx256(char* buff, uint64_t size);
    #endif
    
    #if defined(__AVX512F__)
    uint64_t find_cutpoint_avx512(char* buff, uint64_t size);
    #endif

    #if defined(__ARM_NEON)
    uint64_t find_cutpoint_neon(char* buff, uint64_t size);
    #endif

    #if defined(__ALTIVEC__)
    uint64_t find_cutpoint_altivec(char* buff, uint64_t size);
    #endif


    public:
    /**
     * @brief Default constructor.
     * @return: void
     */
    AE_Chunking();

    /**
     * @brief Constructor with custom config from a config object
     * @param config: the config object
     * @return: void
     */
    AE_Chunking(const Config& config);

    ~AE_Chunking();

};

#endif