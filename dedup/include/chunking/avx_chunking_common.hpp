#ifndef _AVX_COMMON_CHUNKING_
#define _AVX_COMMON_CHUNKING_

#include <chunking_common.hpp>

#if defined(__SSE3__) || defined(__AVX2__) || defined(__AVX512F__) || defined(__BMI2__)
    #include <x86intrin.h>
#elif defined(__ARM_NEON)
    #include <arm_neon.h>
#elif defined(__ALTIVEC__)
    #include <altivec.h>
    #undef vector // Avoid conflict with std::vector
    #undef bool // Avoid conflict with std::bool
#endif

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

#define NEON_REGISTER_SIZE_BITS 128
#define NEON_REGISTER_SIZE_BYTES 16

#define ALTIVEC_REGISTER_SIZE_BITS 128
#define ALTIVEC_REGISTER_SIZE_BYTES 16

class AVX_Chunking_Technique: public virtual Chunking_Technique{
    protected:

    SIMD_Mode simd_mode;

    #ifdef __SSE3__
        const __m128i K_INV_ZERO = _mm_set1_epi8(0xFF);
    #endif
    
    #ifdef __AVX2__
        const __m256i K_INV_ZERO_256 = _mm256_set1_epi8(0xFF);
    #endif

    // For debugging only
    uint64_t chunk_counter;

    public:
    /**
     * @brief Finds maximum value in the region between <start_pos, end_pos> within the data stream *buff using SIMD instructions
     * 
     * @param buff Data Stream
     * @param start_pos Starting position of scanned region
     * @param end_pos Ending position of scanned region
     * @return uint8_t Maximum Value
     */
    #ifdef __SSE3__
        uint8_t find_maximum_sse128(char *buff, uint64_t start_pos, uint64_t end_pos, __m128i *xmm_array);
    #endif
    
    #ifdef __AVX2__
        uint8_t find_maximum_avx256(char *buff, uint64_t start_pos, uint64_t end_pos, __m256i *ymm_array);
    #endif

    #if defined(__AVX512F__)
        uint8_t find_maximum_avx512(char *buff, uint64_t start_pos, uint64_t end_pos, __m512i *zmm_array);
    #endif

    #if defined(__ARM_NEON)
        uint8_t find_maximum_neon(char *buff, uint64_t start_pos, uint64_t end_pos, uint8x16_t *neon_array);
    #endif

    #ifdef __ALTIVEC__
        uint8_t find_maximum_altivec(char *buff, uint64_t start_pos, uint64_t end_pos, __vector unsigned char *vec_array);
    #endif

    /**
     * @brief Finds minimum value in the region between <start_pos, end_pos> within the data stream *buff using SIMD instructions
     * 
     * @param buff Data Stream
     * @param start_pos Starting position of scanned region
     * @param end_pos Ending position of scanned region
     * @return uint8_t Minimum Value
     */

    #ifdef __SSE3__
        uint8_t find_minimum_sse128(char *buff, uint64_t start_pos, uint64_t end_pos, __m128i *xmm_array);
    #endif
    
    #ifdef __AVX2__
        uint8_t find_minimum_avx256(char *buff, uint64_t start_pos, uint64_t end_pos, __m256i *ymm_array);
    #endif  

    #if defined(__AVX512F__)
        uint8_t find_minimum_avx512(char *buff, uint64_t start_pos, uint64_t end_pos, __m512i *zmm_array);
    #endif

    #if defined(__ARM_NEON)
        uint8_t find_minimum_neon(char *buff, uint64_t start_pos, uint64_t end_pos, uint8x16_t *neon_array);
    #endif

    #ifdef __ALTIVEC__
        uint8_t find_minimum_altivec(char *buff, uint64_t start_pos, uint64_t end_pos, __vector unsigned char *vec_array);
    #endif

    /**
     * @brief Executes a range scan comparing bytes serially against the target value between <start_pos, end_pos> within the data stream *buff using SIMD instructions.
     * Comparison operator: GEQ (>=)
     * 
     * @param buff Data Stream
     * @param start_pos Starting position of scanned region
     * @param end_pos Ending position of scanned region
     * @param target_value Target value
     * @return uint64_t Position of first matching candidate. 
     * @return end_position if no match found
     */
    #ifdef __SSE3__
        uint64_t range_scan_geq_sse128(char *buff, uint64_t start_position, uint64_t end_position, uint8_t target_value);
    #endif

    #ifdef __AVX2__
        uint64_t range_scan_geq_avx256(char *buff, uint64_t start_position, uint64_t end_position, uint8_t target_value);
    #endif

    #if defined(__AVX512F__)
        uint64_t range_scan_geq_avx512(char *buff, uint64_t start_position, uint64_t end_position, uint8_t target_value);
    #endif

    #if defined(__ARM_NEON)
        uint64_t range_scan_geq_neon(char *buff, uint64_t start_position, uint64_t end_position, uint8_t target_value);
    #endif

    #if defined(__ALTIVEC__)
        uint64_t range_scan_geq_altivec(char *buff, uint64_t start_position, uint64_t end_position, uint8_t target_value);
    #endif
    
    /**
     * @brief Executes a range scan comparing bytes serially against the target value between <start_pos, end_pos> within the data stream *buff using SIMD instructions.
     * Comparison operator: GT (>)
     * 
     * @param buff Data Stream
     * @param start_pos Starting position of scanned region
     * @param end_pos Ending position of scanned region
     * @param target_value Target value
     * @return uint64_t Position of first matching candidate. 
     * @return end_position if no match found
     */
    #ifdef __SSE3__
        uint64_t range_scan_gt_sse128(char *buff, uint64_t start_position, uint64_t end_position, uint8_t target_value);
    #endif

    #ifdef __AVX2__
        uint64_t range_scan_gt_avx256(char *buff, uint64_t start_position, uint64_t end_position, uint8_t target_value);
    #endif

    #if defined(__AVX512F__)
        uint64_t range_scan_gt_avx512(char *buff, uint64_t start_position, uint64_t end_position, uint8_t target_value);
    #endif

    #if defined(__ARM_NEON)
        uint64_t range_scan_gt_neon(char *buff, uint64_t start_position, uint64_t end_position, uint8_t target_value);
    #endif

    #if defined(__ALTIVEC__)
        uint64_t range_scan_gt_altivec(char *buff, uint64_t start_position, uint64_t end_position, uint8_t target_value);
    #endif

     /**
     * @brief Executes a range scan comparing bytes serially against the target value between <start_pos, end_pos> within the data stream *buff using SIMD instructions.
     * Comparison operator: LEQ (<=)
     * 
     * @param buff Data Stream
     * @param start_pos Starting position of scanned region
     * @param end_pos Ending position of scanned region
     * @param target_value Target value
     * @return uint64_t Position of first matching candidate. 
     * @return end_position if no match found
     */
    #ifdef __SSE3__
        uint64_t range_scan_leq_sse128(char *buff, uint64_t start_position, uint64_t end_position, uint8_t target_value);
    #endif

    #ifdef __AVX2__
        uint64_t range_scan_leq_avx256(char *buff, uint64_t start_position, uint64_t end_position, uint8_t target_value);
    #endif

    #if defined(__AVX512F__)
        uint64_t range_scan_leq_avx512(char *buff, uint64_t start_position, uint64_t end_position, uint8_t target_value);
    #endif

    #if defined(__ARM_NEON)
        uint64_t range_scan_leq_neon(char *buff, uint64_t start_position, uint64_t end_position, uint8_t target_value);
    #endif

    #ifdef __ALTIVEC__
        uint64_t range_scan_leq_altivec(char *buff, uint64_t start_position, uint64_t end_position, uint8_t target_value);
    #endif

    /**
     * @brief Executes a range scan comparing bytes serially against the target value between <start_pos, end_pos> within the data stream *buff using SIMD instructions.
     * Comparison operator: LT (<)
     * 
     * @param buff Data Stream
     * @param start_pos Starting position of scanned region
     * @param end_pos Ending position of scanned region
     * @param target_value Target value
     * @return uint64_t Position of first matching candidate. 
     * @return end_position if no match found
     */
    #ifdef __SSE3__
        uint64_t range_scan_lt_sse128(char *buff, uint64_t start_position, uint64_t end_position, uint8_t target_value);
    #endif

    #ifdef __AVX2__
        uint64_t range_scan_lt_avx256(char *buff, uint64_t start_position, uint64_t end_position, uint8_t target_value);
    #endif

    #if defined(__AVX512F__)
        uint64_t range_scan_lt_avx512(char *buff, uint64_t start_position, uint64_t end_position, uint8_t target_value);
    #endif

    #if defined(__ARM_NEON)
        uint64_t range_scan_lt_neon(char *buff, uint64_t start_position, uint64_t end_position, uint8_t target_value);
    #endif

    #ifdef __ALTIVEC__
        uint64_t range_scan_lt_altivec(char *buff, uint64_t start_position, uint64_t end_position, uint8_t target_value);
    #endif


    /**
     * @brief Helper functions to compare 2 128-bit XMM registers
     * 
     * @param a First XMM register containing 8-bit packed values
     * @param b Second XMM register containing 8-bit packed values
     * @return __m128i Result of bytewise comparison
     */
    #ifdef __SSE3__
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
    #endif

    #ifdef __AVX2__
        inline __m256i Greater8uAVX256(__m256i a, __m256i b){
        return _mm256_andnot_si256(_mm256_cmpeq_epi8(_mm256_min_epu8(a, b), a), K_INV_ZERO_256);
        }
        inline __m256i Lesser8uAVX256(__m256i a, __m256i b){
                return _mm256_andnot_si256(_mm256_cmpeq_epi8(_mm256_max_epu8(a, b), a), K_INV_ZERO_256);
        }

        inline __m256i GreaterOrEqual8uAVX256(__m256i a, __m256i b){
            return _mm256_cmpeq_epi8(_mm256_max_epu8(a, b), a);
        }
        inline __m256i LesserOrEqual8uAVX256(__m256i a, __m256i b){
            return _mm256_cmpeq_epi8(_mm256_min_epu8(a, b), a);
        }
    #endif


    
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

};

#endif