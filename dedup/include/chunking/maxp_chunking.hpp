#ifndef _MAXP_CHUNKING_
#define _MAXP_CHUNKING_

#include <math.h>
#include <iostream>

#include "avx_chunking_common.hpp"
#include "chunking_common.hpp"
#include "config.hpp"

#include <cstring>

#define DEFAULT_MAXP_WINDOW_SIZE 128
#define DEFAULT_MAXP_MAX_BLOCK_SIZE 65536

class MAXP_Chunking : public virtual AVX_Chunking_Technique {
   private:
    uint64_t max_block_size;
    uint64_t window_size;

    #ifdef __SSE3__
    __m128i *xmm_array;
    #endif

    #ifdef __AVX2__
    __m256i *ymm_array;
    #endif
    
    #if defined(__AVX512F__)
    __m512i *zmm_array;
    #endif

    #if defined(__ARM_NEON)
    uint8x16_t *neon_array;
    #endif

    #ifdef __ALTIVEC__
    __vector unsigned char *altivec_array;
    #endif

    /**
     * @brief finds the next cut point in an array of bytes
     * @param buff: the buff to find the cutpoint in.
     * @param size: the size of the buffer
     * @return: cutpoint position in the buffer 
     */
    uint64_t find_cutpoint(char* buff, uint64_t size) override;
    uint64_t find_cutpoint_native(char *buff, uint64_t size);
    
    #ifdef __SSE3__
    uint64_t find_cutpoint_sse128(char *buff, uint64_t size);
    #endif

    #ifdef __AVX2__
    uint64_t find_cutpoint_avx256(char *buff, uint64_t size);
    #endif
    
    #if defined(__AVX512F__)
    uint64_t find_cutpoint_avx512(char *buff, uint64_t size);
    #endif

    #if defined(__ARM_NEON)
    uint64_t find_cutpoint_neon(char *buff, uint64_t size);
    #endif

    #ifdef __ALTIVEC__
    uint64_t find_cutpoint_altivec(char *buff, uint64_t size);
    #endif

   public:
    /**
     * @brief Default constructor.
     * @return: void
     */
    MAXP_Chunking();

    /**
     * @brief Constructor with custom config from a config object
     * @param config: the config object
     * @return: void
     */
    MAXP_Chunking(const Config& config);

    ~MAXP_Chunking();

};

#endif