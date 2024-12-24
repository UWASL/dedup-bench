#ifndef _AVX_COMMON_CHUNKING_
#define _AVX_COMMON_CHUNKING_

#include <chunking_common.hpp>
#include <x86intrin.h>

#include <iostream>
#include <cstdint>
#include <cstring>


#define SSE_REGISTER_SIZE_BITS 128
#define SSE_REGISTER_SIZE_BYTES 16

#define AVX256_REGISTER_SIZE_BITS 256
#define AVX256_REGISTER_SIZE_BYTES 32

#define AVX512_REGISTER_SIZE_BITS 512
#define AVX512_REGISTER_SIZE_BYTES 64
#define AVX512_REGISTER_SIZE_INT32 16

class AVX_Chunking_Technique: public virtual Chunking_Technique{
    protected:

    SIMD_Mode simd_mode;
    const __m128i K_INV_ZERO = _mm_set1_epi8(0xFF);//_mm_set1_epi8(-1);

    public:
    /**
     * @brief Finds maximum value in the region between <start_pos, end_pos> within the data stream *buff using 128-bit SSE instructions
     * 
     * @param buff Data Stream
     * @param start_pos Starting position of scanned region
     * @param end_pos Ending position of scanned region
     * @return uint8_t Maximum Value
     */
    uint8_t find_maximum_sse128(char *buff, uint64_t start_pos, uint64_t end_pos, __m128i *xmm_array);
    uint8_t find_maximum_avx256(char *buff, uint64_t start_pos, uint64_t end_pos, __m256i *xmm_array);
    #if defined(__AVX512F__)
        uint8_t find_maximum_avx512(char *buff, uint64_t start_pos, uint64_t end_pos, __m512i *ymm_array);
    #endif

    /**
     * @brief Helper functions to compare 2 128-bit XMM registers
     * 
     * @param a First XMM register containing 8-bit packed values
     * @param b Second XMM register containing 8-bit packed values
     * @return __m128i Result of bytewise comparison
     */
    inline __m128i Greater8uSSE(__m128i a, __m128i b){
       return _mm_andnot_si128(_mm_cmpeq_epi8(_mm_min_epu8(a, b), a), K_INV_ZERO);
    }

    inline __m128i GreaterOrEqual8uSSE(__m128i a, __m128i b){
        return _mm_cmpeq_epi8(_mm_max_epu8(a, b), a);
    }

    inline __m128i Lesser8uSSE(__m128i a, __m128i b){
            return _mm_andnot_si128(_mm_cmpeq_epi8(_mm_max_epu8(a, b), a), K_INV_ZERO);
    }

    inline __m128i LesserOrEqual8uSSE(__m128i a, __m128i b){
            return _mm_cmpeq_epi8(_mm_min_epu8(a, b), a);
    }
    inline __m128i NotEqual8uSSE(__m128i a, __m128i b){
            return _mm_andnot_si128(_mm_cmpeq_epi8(a, b), K_INV_ZERO);
    }

    inline __m256i GreaterOrEqual8uAVX256(__m256i a, __m256i b){
        return _mm256_cmpeq_epi8(_mm256_max_epu8(a, b), a);
    }


    
    /**
     * @brief: Helper functions to print __m512i in human readable format for debugging
    */
    #if defined(__AVX512F__)
    inline void printm512i(__m512i entry){
        // Print __m512i as 16 32-bit values
        uint32_t val[16];
        std::memcpy(val, &entry, sizeof(val));

        for(int i = 0; i < 16; i++)
            std::cout << val[i] << " ";
        std::cout << std::endl;
    }

     inline void printm512i_left(__m512i entry){
        // Print leftmost 32-bit value from __m512i

        uint32_t val[16];
        std::memcpy(val, &entry, sizeof(val));

        std::cout << val[0] << " ";       
        std::cout << std::endl;
    }

    inline void printm512i_left_byte(__m512i entry){

        // Print leftmost 8-bit value from __m512i
        
        uint8_t val[AVX512_REGISTER_SIZE_BYTES];
        std::memcpy(val, &entry, sizeof(val));
        
        std::cout << val[0] << " ";
        std::cout << std::endl;
    }
    #endif


    /**
     * @brief Check if an integer has any byte with zero in it.
     * Found this off the Stanford bitchacks page: https://graphics.stanford.edu/~seander/bithacks.html#ZeroInWord
     * @param v Integer (4 bytes)
     * @return true If any byte is zero
     * @return false If all bytes are > 0
     */
    
    inline bool hasZeroByte(unsigned int v)
    {
        return ~((((v & 0x7F7F7F7F) + 0x7F7F7F7F) | v) | 0x7F7F7F7F);
    }

};

#endif