/**
 * @file avx_chunking_common.cpp
 * @author WASL
 * @brief Implementations of functions common across all AVX chunking techniques
 * @version 0.1
 * @date 2023-02-02
 *
 * @copyright Copyright (c) 2023
 *
 */


#include "avx_chunking_common.hpp"
#include "chunking_common.hpp"
#include "hashing_common.hpp"

#include <cmath>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>

/**
 * * @brief: Helper functions to find maximum value in a region of the data stream
 * * @param buff Data Stream
 * * @param start_pos Starting position of scanned region
 * * @param end_pos Ending position of scanned region
 * * @param xmm_array Array of XMM registers to store intermediate results
 * * @return uint8_t Maximum Value
 */

#if defined(__SSE3__)
uint8_t AVX_Chunking_Technique::find_maximum_sse128(char *buff, uint64_t start_pos, uint64_t end_pos, __m128i *xmm_array){

    // Assume window_size is a multiple of SSE_REGISTER_SIZE_BYTES for now
    // Assume num_vectors is even for now - True for most common window sizes. Can fix later via specific check

    uint64_t num_vectors = (end_pos - start_pos) / SSE_REGISTER_SIZE_BYTES;

    uint64_t step = 2;
    uint64_t half_step = 1;

    // Load contents into __m128i structures
    // Could be optimized later as only 16 xmm registers are avaiable per CPU in 64-bit
    for(uint64_t i = 0; i < num_vectors; i++)
        xmm_array[i] = _mm_loadu_si128((__m128i const *)(buff + start_pos + (SSE_REGISTER_SIZE_BYTES * i)));
    
    // Repeat vmaxu until a single register is remaining with maximum values
    // Each iteration calculates maximums between a pair of registers and moves it into the first register in the pair
    // Finally, only one will be left with the maximum values from all pairs
    while(step <= num_vectors){
        
        for(uint64_t i = 0; i < num_vectors; i+=step)
            xmm_array[i] = _mm_max_epu8(xmm_array[i], xmm_array[i+half_step]);
        
        // Multiply step by 2
        half_step = step;
        step = step << 1;
    
    }

    // Move the final set of values from the xmm into local memory    
    uint8_t result_store[SSE_REGISTER_SIZE_BYTES] = {0};

    _mm_storeu_si128((__m128i *)&result_store, xmm_array[0]);

    // Sequentially scan the last remaining bytes (128 in this case) to find the max value
    uint8_t max_val = 0;
    for(uint64_t i = 0; i < SSE_REGISTER_SIZE_BYTES; i++){
        if(result_store[i] > max_val)
            max_val = result_store[i];
    }

    // Return maximum value
    return max_val;
}
#endif

#if defined(__AVX2__)
uint8_t AVX_Chunking_Technique::find_maximum_avx256(char *buff, uint64_t start_pos, uint64_t end_pos, __m256i *xmm_array){

    // Assume window_size is a multiple of AVX256_REGISTER_SIZE_BYTES for now
    // Assume num_vectors is even for now - True for most common window sizes. Can fix later via specific check

    uint64_t num_vectors = (end_pos - start_pos) / AVX256_REGISTER_SIZE_BYTES;

    uint64_t step = 2;
    uint64_t half_step = 1;

    // Load contents into __m256i structures
    // Could be optimized later as only 16 xmm registers are avaiable per CPU in 64-bit
    for(uint64_t i = 0; i < num_vectors; i++)
        xmm_array[i] = _mm256_loadu_si256((__m256i const *)(buff + start_pos + (AVX256_REGISTER_SIZE_BYTES * i)));
    
    // Repeat vmaxu until a single register is remaining with maximum values
    // Each iteration calculates maximums between a pair of registers and moves it into the first register in the pair
    // Finally, only one will be left with the maximum values from all pairs
    while(step <= num_vectors){
        
        for(uint64_t i = 0; i < num_vectors; i+=step)
            xmm_array[i] = _mm256_max_epu8(xmm_array[i], xmm_array[i+half_step]);
        
        // Multiply step by 2
        half_step = step;
        step = step << 1;
    
    }

    // Move the final set of values from the xmm into local memory    
    uint8_t result_store[AVX256_REGISTER_SIZE_BYTES] = {0};

    _mm256_storeu_si256((__m256i *)&result_store, xmm_array[0]);

    // Sequentially scan the last remaining bytes (256 in this case) to find the max value
    uint8_t max_val = 0;
    for(uint64_t i = 0; i < AVX256_REGISTER_SIZE_BYTES; i++){
        if(result_store[i] > max_val)
            max_val = result_store[i];
    }

    // Return maximum value
    return max_val;
}
#endif

#if defined(__AVX512F__)
uint8_t AVX_Chunking_Technique::find_maximum_avx512(char *buff, uint64_t start_pos, uint64_t end_pos, __m512i *xmm_array){

    // Assume window_size is a multiple of AVX512_REGISTER_SIZE_BYTES for now
    // Assume num_vectors is even for now - True for most common window sizes. Can fix later via specific check

    __mmask64 cmp_mask = UINT64_MAX;

    uint64_t num_vectors = (end_pos - start_pos) / AVX512_REGISTER_SIZE_BYTES;

    uint64_t step = 2;
    uint64_t half_step = 1;

    // Load contents into __m512i structures
    // Could be optimized later as only 16 xmm registers are avaiable per CPU in 64-bit
    for(uint64_t i = 0; i < num_vectors; i++)
        xmm_array[i] = _mm512_loadu_si512((__m512i const *)(buff + start_pos + (AVX512_REGISTER_SIZE_BYTES * i)));
    
    // Repeat vmaxu until a single register is remaining with maximum values
    // Each iteration calculates maximums between a pair of registers and moves it into the first register in the pair
    // Finally, only one will be left with the maximum values from all pairs
    while(step <= num_vectors){
        
        for(uint64_t i = 0; i < num_vectors; i+=step)
            xmm_array[i] = _mm512_maskz_max_epu8(cmp_mask, xmm_array[i], xmm_array[i+half_step]);
        
        // Multiply step by 2
        half_step = step;
        step = step << 1;
    
    }

    // Move the final set of values from the xmm into local memory    
    uint8_t result_store[AVX512_REGISTER_SIZE_BYTES] = {0};

    _mm512_storeu_si512((__m512i *)&result_store, xmm_array[0]);

    // Sequentially scan the last remaining bytes (512 in this case) to find the max value
    uint8_t max_val = 0;
    for(uint64_t i = 0; i < AVX512_REGISTER_SIZE_BYTES; i++){
        if(result_store[i] > max_val)
            max_val = result_store[i];
    }

    // Return maximum value
    return max_val;
}
#endif

#if defined(__ARM_NEON)
uint8_t AVX_Chunking_Technique::find_maximum_neon(char *buff, uint64_t start_pos, uint64_t end_pos, uint8x16_t *neon_array){

    // Assume window_size is a multiple of NEON_REGISTER_SIZE_BYTES for now
    // Assume num_vectors is even for now - True for most common window sizes. Can fix later via specific check

    uint64_t num_vectors = (end_pos - start_pos) / NEON_REGISTER_SIZE_BYTES;

    uint64_t step = 2;
    uint64_t half_step = 1;

    // Load contents into uint8x16_t structures
    // Could be optimized later as only 16 mm registers are avaiable per CPU in 64-bit
    for(uint64_t i = 0; i < num_vectors; i++)
        neon_array[i] = vld1q_u8((uint8_t const *)(buff + start_pos + (NEON_REGISTER_SIZE_BYTES * i)));

    // Repeat vmaxu until a single register is remaining with maximum values
    // Each iteration calculates maximums between a pair of registers and moves it into the first register in the pair
    // Finally, only one will be left with the maximum values from all pairs
    while(step <= num_vectors){
        
        for(uint64_t i = 0; i < num_vectors; i+=step)
            neon_array[i] = vmaxq_u8(neon_array[i], neon_array[i+half_step]);
        
        // Multiply step by 2
        half_step = step;
        step = step << 1;
    
    }

    // Move the final set of values from the NEON into local memory
    uint8_t result_store[NEON_REGISTER_SIZE_BYTES] = {0};
    vst1q_u8((uint8_t *)&result_store, neon_array[0]);
    
    // Sequentially scan the last remaining bytes (16 in this case) to find the max value
    uint8_t max_val = 0;
    for(uint64_t i = 0; i < NEON_REGISTER_SIZE_BYTES; i++){
        if(result_store[i] > max_val)
            max_val = result_store[i];
    }
    // Return maximum value
    return max_val;

}
#endif

#if defined(__ALTIVEC__)
uint8_t AVX_Chunking_Technique::find_maximum_altivec(char *buff, uint64_t start_pos, uint64_t end_pos, __vector unsigned char *vec_array){

    // Assume window_size is a multiple of ALTIVEC_REGISTER_SIZE_BYTES for now
    // Assume num_vectors is even for now - True for most common window sizes. Can fix later via specific check

    uint64_t num_vectors = (end_pos - start_pos) / ALTIVEC_REGISTER_SIZE_BYTES;

    uint64_t step = 2;
    uint64_t half_step = 1;

    // Load contents into vector unsigned char structures
    // Could be optimized later as only 32 vec registers are available per CPU
    for(uint64_t i = 0; i < num_vectors; i++)
        vec_array[i] = vec_xl(0, (unsigned char const *)(buff + start_pos + (ALTIVEC_REGISTER_SIZE_BYTES * i)));

    // Repeat vmaxu until a single register is remaining with maximum values
    // Each iteration calculates maximums between a pair of registers and moves it into the first register in the pair
    // Finally, only one will be left with the maximum values from all pairs
    while(step <= num_vectors){
        
        for(uint64_t i = 0; i < num_vectors; i+=step)
            vec_array[i] = vec_max(vec_array[i], vec_array[i+half_step]);
        
        // Multiply step by 2
        half_step = step;
        step = step << 1;
    
    }

    // Move the final set of values from the vec into local memory    
    unsigned char result_store[ALTIVEC_REGISTER_SIZE_BYTES] = {0};

    vec_st(vec_array[0], 0, (unsigned char *)&result_store);

    // Sequentially scan the last remaining bytes (16 in this case) to find the max value
    uint8_t max_val = 0;
    for(uint64_t i = 0; i < ALTIVEC_REGISTER_SIZE_BYTES; i++){
        if(result_store[i] > max_val)
            max_val = result_store[i];
    }

    // Return maximum value
    return max_val;
}
#endif

/**
 * @brief: Helper functions to find minimum value in data stream
 * * @param buff Data Stream
 * * @param start_pos Starting position of scanned region
 * * @param end_pos Ending position of scanned region
 * * @param xmm_array Array of SIMD registers to store intermediate results
 * * @return uint8_t Minimum Value
 */

 #if defined(__SSE3__)
 uint8_t AVX_Chunking_Technique::find_minimum_sse128(char *buff, uint64_t start_pos, uint64_t end_pos, __m128i *xmm_array){

    // Assume window_size is a multiple of SSE_REGISTER_SIZE_BYTES for now
    // Assume num_vectors is even for now - True for most common window sizes. Can fix later via specific check

    uint64_t num_vectors = (end_pos - start_pos) / SSE_REGISTER_SIZE_BYTES;

    uint64_t step = 2;
    uint64_t half_step = 1;

    // Load contents into __m128i structures
    // Could be optimized later as only 16 xmm registers are avaiable per CPU in 64-bit
    for(uint64_t i = 0; i < num_vectors; i++)
        xmm_array[i] = _mm_loadu_si128((__m128i const *)(buff + start_pos + (SSE_REGISTER_SIZE_BYTES * i)));
    
    // Repeat vminu until a single register is remaining with minimum values
    // Each iteration calculates minimums between a pair of registers and moves it into the first register in the pair
    // Finally, only one will be left with the minimum values from all pairs
    while(step <= num_vectors){
        
        for(uint64_t i = 0; i < num_vectors; i+=step)
            xmm_array[i] = _mm_min_epu8(xmm_array[i], xmm_array[i+half_step]);
        
        // Multiply step by 2
        half_step = step;
        step = step << 1;
    
    }

    // Move the final set of values from the xmm into local memory    
    uint8_t result_store[SSE_REGISTER_SIZE_BYTES] = {0};

    _mm_storeu_si128((__m128i *)&result_store, xmm_array[0]);

    // Sequentially scan the last remaining bytes (128 in this case) to find the min value
    uint8_t min_val = UINT8_MAX;
    for(uint64_t i = 0; i < SSE_REGISTER_SIZE_BYTES; i++){
        if(result_store[i] < min_val)
            min_val = result_store[i];
    }

    // Return minimum value
    return min_val;
}
#endif

#if defined(__AVX2__)
uint8_t AVX_Chunking_Technique::find_minimum_avx256(char *buff, uint64_t start_pos, uint64_t end_pos, __m256i *xmm_array){

    // Assume window_size is a multiple of AVX256_REGISTER_SIZE_BYTES for now
    // Assume num_vectors is even for now - True for most common window sizes. Can fix later via specific check

    uint64_t num_vectors = (end_pos - start_pos) / AVX256_REGISTER_SIZE_BYTES;

    uint64_t step = 2;
    uint64_t half_step = 1;

    // Load contents into __m256i structures
    // Could be optimized later as only 16 xmm registers are avaiable per CPU in 64-bit
    for(uint64_t i = 0; i < num_vectors; i++)
        xmm_array[i] = _mm256_loadu_si256((__m256i const *)(buff + start_pos + (AVX256_REGISTER_SIZE_BYTES * i)));
    
    // Repeat vminu until a single register is remaining with minimum values
    // Each iteration calculates minimums between a pair of registers and moves it into the first register in the pair
    // Finally, only one will be left with the minimum values from all pairs
    while(step <= num_vectors){
        
        for(uint64_t i = 0; i < num_vectors; i+=step)
            xmm_array[i] = _mm256_min_epu8(xmm_array[i], xmm_array[i+half_step]);
        
        // Multiply step by 2
        half_step = step;
        step = step << 1;
    
    }

    // Move the final set of values from the xmm into local memory    
    uint8_t result_store[AVX256_REGISTER_SIZE_BYTES] = {0};

    _mm256_storeu_si256((__m256i *)&result_store, xmm_array[0]);

    // Sequentially scan the last remaining bytes (256 in this case) to find the min value
    uint8_t min_val = UINT8_MAX;
    for(uint64_t i = 0; i < AVX256_REGISTER_SIZE_BYTES; i++){
        if(result_store[i] < min_val)
            min_val = result_store[i];
    }

    // Return minimum value
    return min_val;
}
#endif

#if defined(__AVX512F__)
uint8_t AVX_Chunking_Technique::find_minimum_avx512(char *buff, uint64_t start_pos, uint64_t end_pos, __m512i *xmm_array){

    // Assume window_size is a multiple of AVX512_REGISTER_SIZE_BYTES for now
    // Assume num_vectors is even for now - True for most common window sizes. Can fix later via specific check

    uint64_t num_vectors = (end_pos - start_pos) / AVX512_REGISTER_SIZE_BYTES;

    uint64_t step = 2;
    uint64_t half_step = 1;

    // Load contents into __m512i structures
    // Could be optimized later as only 16 xmm registers are avaiable per CPU in 64-bit
    for(uint64_t i = 0; i < num_vectors; i++)
        xmm_array[i] = _mm512_loadu_si512((__m512i const *)(buff + start_pos + (AVX512_REGISTER_SIZE_BYTES * i)));
    
    // Repeat vminu until a single register is remaining with minimum values
    // Each iteration calculates minimums between a pair of registers and moves it into the first register in the pair
    // Finally, only one will be left with the minimum values from all pairs
    while(step <= num_vectors){
        
        for(uint64_t i = 0; i < num_vectors; i+=step)
            xmm_array[i] = _mm512_maskz_min_epu8(UINT64_MAX, xmm_array[i], xmm_array[i+half_step]);
        
        // Multiply step by 2
        half_step = step;
        step = step << 1;
    
    }

    // Move the final set of values from the xmm into local memory    
    uint8_t result_store[AVX512_REGISTER_SIZE_BYTES] = {0};

    _mm512_storeu_si512((__m512i *)&result_store, xmm_array[0]);

    // Sequentially scan the last remaining bytes (512 in this case) to find the min value
    uint8_t min_val = UINT8_MAX;
    for(uint64_t i = 0; i < AVX512_REGISTER_SIZE_BYTES; i++){
        if(result_store[i] < min_val)
            min_val = result_store[i];
    }

    // Return minimum value
    return min_val;
}
#endif

#if defined(__ARM_NEON)
uint8_t AVX_Chunking_Technique::find_minimum_neon(char *buff, uint64_t start_pos, uint64_t end_pos, uint8x16_t *neon_array){

    // Assume window_size is a multiple of NEON_REGISTER_SIZE_BYTES for now
    // Assume num_vectors is even for now - True for most common window sizes. Can fix later via specific check

    uint64_t num_vectors = (end_pos - start_pos) / NEON_REGISTER_SIZE_BYTES;

    uint64_t step = 2;
    uint64_t half_step = 1;

    // Load contents into uint8x16_t structures
    // Could be optimized later as only 16 mm registers are avaiable per CPU in 64-bit
    for(uint64_t i = 0; i < num_vectors; i++)
        neon_array[i] = vld1q_u8((uint8_t const *)(buff + start_pos + (NEON_REGISTER_SIZE_BYTES * i)));

    // Repeat vminu until a single register is remaining with minimum values
    // Each iteration calculates minimums between a pair of registers and moves it into the first register in the pair
    // Finally, only one will be left with the minimum values from all pairs
    while(step <= num_vectors){
        
        for(uint64_t i = 0; i < num_vectors; i+=step)
            neon_array[i] = vminq_u8(neon_array[i], neon_array[i+half_step]);
        
        // Multiply step by 2
        half_step = step;
        step = step << 1;
    
    }

    // Move the final set of values from the NEON into local memory
    uint8_t result_store[NEON_REGISTER_SIZE_BYTES] = {0};
    vst1q_u8((uint8_t *)&result_store, neon_array[0]);
    
    // Sequentially scan the last remaining bytes (16 in this case) to find the min value
    uint8_t min_val = UINT8_MAX;
    for(uint64_t i = 0; i < NEON_REGISTER_SIZE_BYTES; i++){
        if(result_store[i] < min_val)
            min_val = result_store[i];
    }
    
    // Return minimum value
    return min_val;

}
#endif

#if defined(__ALTIVEC__)
uint8_t AVX_Chunking_Technique::find_minimum_altivec(char *buff, uint64_t start_pos, uint64_t end_pos, __vector unsigned char *vec_array){
    // Assume window_size is a multiple of ALTIVEC_REGISTER_SIZE_BYTES for now
    // Assume num_vectors is even for now - True for most common window sizes. Can fix later via specific check

    uint64_t num_vectors = (end_pos - start_pos) / ALTIVEC_REGISTER_SIZE_BYTES;

    uint64_t step = 2;
    uint64_t half_step = 1;

    // Load contents into vector unsigned char structures
    // Could be optimized later as only 32 vec registers are available per CPU
    for(uint64_t i = 0; i < num_vectors; i++)
        vec_array[i] = vec_xl(0, (unsigned char const *)(buff + start_pos + (ALTIVEC_REGISTER_SIZE_BYTES * i)));

    // Repeat vminu until a single register is remaining with minimum values
    // Each iteration calculates minimums between a pair of registers and moves it into the first register in the pair
    // Finally, only one will be left with the minimum values from all pairs
    while(step <= num_vectors){
        
        for(uint64_t i = 0; i < num_vectors; i+=step)
            vec_array[i] = vec_min(vec_array[i], vec_array[i+half_step]);
        
        // Multiply step by 2
        half_step = step;
        step = step << 1;
    
    }

    // Move the final set of values from the vec into local memory    
    unsigned char result_store[ALTIVEC_REGISTER_SIZE_BYTES] = {0};

    vec_st(vec_array[0], 0, (unsigned char *)&result_store);

    // Sequentially scan the last remaining bytes (16 in this case) to find the min value
    uint8_t min_val = UINT8_MAX;
    for(uint64_t i = 0; i < ALTIVEC_REGISTER_SIZE_BYTES; i++){
        if(result_store[i] < min_val)
            min_val = result_store[i];
    }

    // Return minimum value
    return min_val;
}
#endif

/**
 * Range scan functions for greater-than-or-equal-to (geq) comparisons
 * These functions are used to find the first position in the data stream matching a target comparison
 * The functions have been implemented for SSE, AVX, AVX512, NEON and AltiVec instruction sets
 */

#if defined(__SSE3__)
uint64_t AVX_Chunking_Technique::range_scan_geq_sse128(char *buff, uint64_t start_position, uint64_t end_position, uint8_t target_value){
    
    uint64_t num_vectors = (end_position - start_position) / SSE_REGISTER_SIZE_BYTES;
    uint64_t curr_scan_start;
    uint64_t return_pos;

    // Structures to store bytes from data stream and comparison results in 128-bit SSE format
    __m128i xmm_array, cmp_array;
    int cmp_mask;

    // Load max_value into xmm-format
    __m128i max_val_xmm = _mm_set1_epi8((char)target_value);
    
    for(uint64_t i = 0; i < num_vectors; i++){
        curr_scan_start = start_position + (i * SSE_REGISTER_SIZE_BYTES);
        // Load data into xmm register
        xmm_array = _mm_loadu_si128((__m128i const *)(buff + curr_scan_start));
        
        /* 
         Compare values with max_value. If a byte in xmm_array is geq max_val_xmm,  
         ALL the corresponding bits of the corresponding byte in cmp_array are set to 1.
        */
        
        #if defined(__AVX512F__)
            cmp_mask = _mm_cmpge_epu8_mask(xmm_array, max_val_xmm);
        #else
            cmp_array = GreaterOrEqual8uSSE(xmm_array, max_val_xmm);
        
            // Create a mask using the most-significant bit of each byte value in cmp_array
            cmp_mask = _mm_movemask_epi8(cmp_array);
        #endif
        


        // Return index of first non-zero bit in mask
        // This corresponds to the first non-zero byte in cmp_array 
        if(cmp_mask){
            return_pos = curr_scan_start + (__builtin_ffs(cmp_mask) - 1);
            return return_pos;            
        }
    }
    
    return end_position;
}
#endif

#if defined(__AVX2__)
uint64_t AVX_Chunking_Technique::range_scan_geq_avx256(char *buff, uint64_t start_position, uint64_t end_position, uint8_t target_value){
    
    uint64_t num_vectors = (end_position - start_position) / AVX256_REGISTER_SIZE_BYTES;
    uint64_t curr_scan_start;

    // Structures to store bytes from data stream and comparison results in 256-bit AVX format
    __m256i xmm_array, cmp_array;
    uint32_t cmp_mask;

    // Load max_value into xmm-format
    __m256i max_val_xmm = _mm256_set1_epi8((char)target_value);
    
    for(uint64_t i = 0; i < num_vectors; i++){
        curr_scan_start = start_position + (i * AVX256_REGISTER_SIZE_BYTES);
        // Load data into xmm register
        xmm_array = _mm256_loadu_si256((__m256i const *)(buff + curr_scan_start));
        
        /* 
         Compare values with max_value. If a byte in xmm_array is geq max_val_xmm,  
         ALL the corresponding bits of the corresponding byte in cmp_array are set to 1.
        */
      
        #if defined(__AVX512F__)
            cmp_mask = _mm256_cmpge_epu8_mask(xmm_array, max_val_xmm);
        #else
            cmp_array = GreaterOrEqual8uAVX256(xmm_array, max_val_xmm);
        
            // Create a mask using the most-significant bit of each byte value in cmp_array
            cmp_mask = _mm256_movemask_epi8(cmp_array);
        #endif

        // Return index of first non-zero bit in mask
        // This corresponds to the first non-zero byte in cmp_array 
        if(cmp_mask){
            return curr_scan_start + (__builtin_ffs(cmp_mask) - 1);            
        }
    }
    
    return end_position;
}
#endif

#if defined(__AVX512F__)
uint64_t AVX_Chunking_Technique::range_scan_geq_avx512(char *buff, uint64_t start_position, uint64_t end_position, uint8_t target_value){
    
    uint64_t num_vectors = (end_position - start_position) / AVX512_REGISTER_SIZE_BYTES;
    uint64_t curr_scan_start;
    uint64_t return_pos;

    // Structures to store bytes from data stream and comparison results in 512-bit AVX format
    __m512i xmm_array;
    __mmask64 cmp_mask;

    // Load max_value into xmm-format
    __m512i max_val_xmm = _mm512_set1_epi8((char)target_value);
    
    for(uint64_t i = 0; i < num_vectors; i++){
        curr_scan_start = start_position + (i * AVX512_REGISTER_SIZE_BYTES);
        // Load data into xmm register
        xmm_array = _mm512_loadu_si512((__m512i const *)(buff + curr_scan_start));
        
        /* 
         Compare values with max_value. If a byte in xmm_array is geq max_val_xmm,  
         ALL the corresponding bits of the corresponding byte in cmp_array are set to 1.
        */
        
        cmp_mask = _mm512_cmpge_epu8_mask(xmm_array, max_val_xmm);

        // Return index of first non-zero bit in mask
        // This corresponds to the first non-zero byte in cmp_array 
        if(cmp_mask){
            return_pos = curr_scan_start + (__builtin_ffsll(cmp_mask) - 1);
            return return_pos;            
        }
    }
    
    return end_position;
}
#endif

#if defined(__ARM_NEON)
uint64_t AVX_Chunking_Technique::range_scan_geq_neon(char *buff, uint64_t start_position, uint64_t end_position, uint8_t target_value){
    
    uint64_t num_vectors = (end_position - start_position) / NEON_REGISTER_SIZE_BYTES;
    uint64_t curr_scan_start;

    // Structures to store bytes from data stream and comparison results in 128-bit NEON format
    uint8x16_t xmm_array, cmp_array;
    uint8x16_t max_val_xmm = vdupq_n_u8((uint8_t)target_value);
    
    for(uint64_t i = 0; i < num_vectors; i++){
        curr_scan_start = start_position + (i * NEON_REGISTER_SIZE_BYTES);
        // Load data into xmm register
        xmm_array = vld1q_u8((uint8_t const *)(buff + curr_scan_start));
        
        /* 
         Compare values with max_value. If a byte in xmm_array is geq max_val_xmm,  
         ALL the corresponding bits of the corresponding byte in cmp_array are set to 1.
        */
        
        cmp_array = vcgeq_u8(xmm_array, max_val_xmm);

        // ARM doesn't have native MOVEMASK instructions like x86 does.
        // Following advice from an ARM community blog post to implement mask creation instead
        // https://community.arm.com/arm-community-blogs/b/servers-and-cloud-computing-blog/posts/porting-x86-vector-bitmask-optimizations-to-arm-neon
        
        // Create a mask using the most-significant bit of each byte value in cmp_array
        
        uint64_t cmp_mask = vget_lane_u64(vreinterpret_u64_u8(vshrn_n_u16(vreinterpretq_u16_u8(cmp_array), 4)), 0);

        // Return index of first non-zero bit in mask
        // This corresponds to the first non-zero byte in cmp_array 
        if(cmp_mask){
            return curr_scan_start + ((__builtin_ffsll(cmp_mask) - 1) >> 2); // Shift right by 2 to account for 4 bits per byte            
        }
    }
    
    return end_position;
}
#endif

#ifdef __ALTIVEC__
uint64_t AVX_Chunking_Technique::range_scan_geq_altivec(char *buff, uint64_t start_position, uint64_t end_position, uint8_t target_value){ 
    
    uint64_t num_vectors = (end_position - start_position) / ALTIVEC_REGISTER_SIZE_BYTES;
    uint64_t curr_scan_start;

    // Structures to store bytes from data stream and comparison results in 128-bit AltiVec format
    __vector unsigned char vec_array;
    __vector unsigned char perm_array;
    __vector __bool char cmp_array;
    __vector unsigned char max_val_vec = vec_splats((unsigned char)target_value);
    __vector unsigned char bit_selector = {0, 8, 16, 24, 32, 40, 48, 56, 64, 72, 80, 88, 96, 104, 112, 120}; // select most significant bit from each byte                    

    uint8_t cond_mask_1, cond_mask_2;    

    for(uint64_t i = 0; i < num_vectors; i++){
        curr_scan_start = start_position + (i * ALTIVEC_REGISTER_SIZE_BYTES);
        
        // Load data into vec register
        // Use vec_xl instead of vec_ld because vec_ld truncates the load address to a 16 byte boundary
        // vec_xl allows loading from any address, which is necessary for unaligned data
        vec_array = vec_xl(0, (unsigned char const *)(buff + curr_scan_start));
                
        /* 
         Compare values with max_value. If a byte in vec_array is geq max_val_vec,  
         ALL the corresponding bits of the corresponding byte in cmp_array are set to 1.
        */
        
        cmp_array = vec_cmpge(vec_array, max_val_vec);
        
        // Create masks using the most-significant bit of each byte value in cmp_array
        perm_array = vec_bperm((__vector unsigned char)cmp_array, bit_selector);
        
        // bperm stores masks in elements 8 and 9 of the vector
        // Extract the masks from the perm_array
        // cond_mask_2 corresponds to the first 8 bytes, cond_mask_1 corresponds to the next 8 bytes
        // This is because vec_bperm rearranges the bits in little endian order
        cond_mask_1 = vec_extract(perm_array, 8);
        cond_mask_2 = vec_extract(perm_array, 9);
 
       // If cond_mask is not 0, it means at least one byte in cmp_array is non-zero
       // Subtract 24 because clz implicitly converts to int with a 4-byte size
       if (cond_mask_2){
            return curr_scan_start + __builtin_clz(cond_mask_2) - 24 ; 
        } 
       else if(cond_mask_1){
            return curr_scan_start + __builtin_clz(cond_mask_1) - 16; // Add 8 to the index to account for the first 8 bytes in the vector
        }
        
    }
    
    return end_position;
}
#endif

/**
 * @brief: Range scan functions for strictly greater than comparison
 * These functions are used to find the first position in the data stream matching a target comparison
 * The functions are imlemented for SSE, AVX2, AVX512, ARM NEON and AltiVec instruction sets.
 */


#if defined(__SSE3__)
uint64_t AVX_Chunking_Technique::range_scan_gt_sse128(char *buff, uint64_t start_position, uint64_t end_position, uint8_t target_value){
    
    uint64_t num_vectors = (end_position - start_position) / SSE_REGISTER_SIZE_BYTES;
    uint64_t curr_scan_start;
    uint64_t return_pos;

    // Structures to store bytes from data stream and comparison results in 128-bit SSE format
    __m128i xmm_array, cmp_array;
    int cmp_mask;

    // Load max_value into xmm-format
    __m128i max_val_xmm = _mm_set1_epi8((char)target_value);
    
    for(uint64_t i = 0; i < num_vectors; i++){
        curr_scan_start = start_position + (i * SSE_REGISTER_SIZE_BYTES);
        // Load data into xmm register
        xmm_array = _mm_loadu_si128((__m128i const *)(buff + curr_scan_start));
        
        /* 
         Compare values with max_value. If a byte in xmm_array is gt max_val_xmm,  
         ALL the corresponding bits of the corresponding byte in cmp_array are set to 1.
        */
        
        #if defined(__AVX512F__)
            cmp_mask = _mm_cmpgt_epu8_mask(xmm_array, max_val_xmm);
        #else
            cmp_array = Greater8uSSE(xmm_array, max_val_xmm);
        
            // Create a mask using the most-significant bit of each byte value in cmp_array
            cmp_mask = _mm_movemask_epi8(cmp_array);
        #endif
        
        // Return index of first non-zero bit in mask
        // This corresponds to the first non-zero byte in cmp_array 
        if(cmp_mask){
            return_pos = curr_scan_start + (__builtin_ffs(cmp_mask) - 1);
            return return_pos;            
        }
    }
    
    return end_position;
}
#endif

#if defined(__AVX2__)
uint64_t AVX_Chunking_Technique::range_scan_gt_avx256(char *buff, uint64_t start_position, uint64_t end_position, uint8_t target_value){
    
    uint64_t num_vectors = (end_position - start_position) / AVX256_REGISTER_SIZE_BYTES;
    uint64_t curr_scan_start;

    // Structures to store bytes from data stream and comparison results in 256-bit AVX format
    __m256i xmm_array, cmp_array;
    uint32_t cmp_mask;

    // Load max_value into xmm-format
    __m256i max_val_xmm = _mm256_set1_epi8((char)target_value);
    
    for(uint64_t i = 0; i < num_vectors; i++){
        curr_scan_start = start_position + (i * AVX256_REGISTER_SIZE_BYTES);
        // Load data into xmm register
        xmm_array = _mm256_loadu_si256((__m256i const *)(buff + curr_scan_start));
        
        /* 
         Compare values with max_value. If a byte in xmm_array is gt max_val_xmm,  
         ALL the corresponding bits of the corresponding byte in cmp_array are set to 1.
        */
        
        #if defined(__AVX512F__)
            cmp_mask = _mm256_cmpgt_epu8_mask(xmm_array, max_val_xmm);
        #else
            cmp_array = Greater8uAVX256(xmm_array, max_val_xmm);
        
            // Create a mask using the most-significant bit of each byte value in cmp_array
            cmp_mask = _mm256_movemask_epi8(cmp_array);
        #endif

        // Return index of first non-zero bit in mask
        // This corresponds to the first non-zero byte in cmp_array 
        if(cmp_mask){
            return curr_scan_start + (__builtin_ffs(cmp_mask) - 1);            
        }
    }
    
    return end_position;
}
#endif

#if defined(__AVX512F__)
uint64_t AVX_Chunking_Technique::range_scan_gt_avx512(char *buff, uint64_t start_position, uint64_t end_position, uint8_t target_value){
    
    uint64_t num_vectors = (end_position - start_position) / AVX512_REGISTER_SIZE_BYTES;
    uint64_t curr_scan_start;
    uint64_t return_pos;

    // Structures to store bytes from data stream and comparison results in 512-bit AVX format
    __m512i xmm_array;
    __mmask64 cmp_mask;

    // Load max_value into xmm-format
    __m512i max_val_xmm = _mm512_set1_epi8((char)target_value);
    
    for(uint64_t i = 0; i < num_vectors; i++){
        curr_scan_start = start_position + (i * AVX512_REGISTER_SIZE_BYTES);
        // Load data into xmm register
        xmm_array = _mm512_loadu_si512((__m512i const *)(buff + curr_scan_start));
        
        /* 
         Compare values with max_value. If a byte in xmm_array is gt max_val_xmm,  
         ALL the corresponding bits of the corresponding byte in cmp_array are set to 1.
        */
        
        cmp_mask = _mm512_cmpgt_epu8_mask(xmm_array, max_val_xmm);

        // Return index of first non-zero bit in mask
        // This corresponds to the first non-zero byte in cmp_array 
        if(cmp_mask){
            return_pos = curr_scan_start + (__builtin_ffsll(cmp_mask) - 1);
            return return_pos;            
        }
    }
    
    return end_position;
}
#endif

#if defined(__ARM_NEON)
uint64_t AVX_Chunking_Technique::range_scan_gt_neon(char *buff, uint64_t start_position, uint64_t end_position, uint8_t target_value){
    
    uint64_t num_vectors = (end_position - start_position) / NEON_REGISTER_SIZE_BYTES;
    uint64_t curr_scan_start;

    // Structures to store bytes from data stream and comparison results in 128-bit NEON format
    uint8x16_t xmm_array, cmp_array;
    uint8x16_t max_val_xmm = vdupq_n_u8((uint8_t)target_value);
    
    for(uint64_t i = 0; i < num_vectors; i++){
        curr_scan_start = start_position + (i * NEON_REGISTER_SIZE_BYTES);
        // Load data into xmm register
        xmm_array = vld1q_u8((uint8_t const *)(buff + curr_scan_start));
        
        /* 
         Compare values with max_value. If a byte in xmm_array is gt max_val_xmm,  
         ALL the corresponding bits of the corresponding byte in cmp_array are set to 1.
        */
        
        cmp_array = vcgtq_u8(xmm_array, max_val_xmm);

        // ARM doesn't have native MOVEMASK instructions like x86 does.
        // Following advice from an ARM community blog post to implement mask creation instead
        // https://community.arm.com/arm-community-blogs/b/servers-and-cloud-computing-blog/posts/porting-x86-vector-bitmask-optimizations-to-arm-neon
        
        // Create a mask using the most-significant bit of each byte value in cmp_array
        
        uint64_t cmp_mask = vget_lane_u64(vreinterpret_u64_u8(vshrn_n_u16(vreinterpretq_u16_u8(cmp_array), 4)), 0);

        // Return index of first non-zero bit in mask
        // This corresponds to the first non-zero byte in cmp_array 
        if(cmp_mask){
            return curr_scan_start + ((__builtin_ffsll(cmp_mask) - 1) >> 2); // Shift right by 2 to account for 4 bits per byte            
        }
    }
    
    return end_position;
}
#endif

#ifdef __ALTIVEC__
uint64_t AVX_Chunking_Technique::range_scan_gt_altivec(char *buff, uint64_t start_position, uint64_t end_position, uint8_t target_value){ 
    
    uint64_t num_vectors = (end_position - start_position) / ALTIVEC_REGISTER_SIZE_BYTES;
    uint64_t curr_scan_start;

    // Structures to store bytes from data stream and comparison results in 128-bit AltiVec format
    __vector unsigned char vec_array;
    __vector unsigned char perm_array;
    __vector __bool char cmp_array;
    __vector unsigned char max_val_vec = vec_splats((unsigned char)target_value);
    __vector unsigned char bit_selector = {0, 8, 16, 24, 32, 40, 48, 56, 64, 72, 80, 88, 96, 104, 112, 120}; // select most significant bit from each byte                    

    uint8_t cond_mask_1, cond_mask_2;    

    for(uint64_t i = 0; i < num_vectors; i++){
        curr_scan_start = start_position + (i * ALTIVEC_REGISTER_SIZE_BYTES);
        
        // Load data into vec register
        // Use vec_xl instead of vec_ld because vec_ld truncates the load address to a 16 byte boundary
        // vec_xl allows loading from any address, which is necessary for unaligned data
        vec_array = vec_xl(0, (unsigned char const *)(buff + curr_scan_start));
                
        /* 
         Compare values with max_value. If a byte in vec_array is gt max_val_vec,  
         ALL the corresponding bits of the corresponding byte in cmp_array are set to 1.
        */
        
        cmp_array = vec_cmpgt(vec_array, max_val_vec);
        
        // Create masks using the most-significant bit of each byte value in cmp_array
        perm_array = vec_bperm((__vector unsigned char)cmp_array, bit_selector);
        cond_mask_1 = vec_extract(perm_array, 8);
        cond_mask_2 = vec_extract(perm_array, 9);
 
       // If cond_mask is not 0, it means at least one byte in cmp_array is non-zero
       if (cond_mask_2){
            return curr_scan_start + __builtin_clz(cond_mask_2) - 24 ; 
        } 
       else if(cond_mask_1){
            return curr_scan_start + __builtin_clz(cond_mask_1) - 16; // Add 8
       }
    }
    return end_position;
}
#endif


/**
 * @brief: Range scan functions for less than or equal to comparison
 * These functions are used to find the first position in the data stream matching a target comparison
 * The functions are implemented for SSE, AVX2, AVX512, NEON and AltiVec instruction sets.
 */

#if defined(__SSE3__)
uint64_t AVX_Chunking_Technique::range_scan_leq_sse128(char *buff, uint64_t start_position, uint64_t end_position, uint8_t target_value){
    
    uint64_t num_vectors = (end_position - start_position) / SSE_REGISTER_SIZE_BYTES;
    uint64_t curr_scan_start;
    uint64_t return_pos;

    // Structures to store bytes from data stream and comparison results in 128-bit SSE format
    __m128i xmm_array, cmp_array;
    int cmp_mask;

    // Load max_value into xmm-format
    __m128i max_val_xmm = _mm_set1_epi8((char)target_value);
    
    for(uint64_t i = 0; i < num_vectors; i++){
        curr_scan_start = start_position + (i * SSE_REGISTER_SIZE_BYTES);
        // Load data into xmm register
        xmm_array = _mm_loadu_si128((__m128i const *)(buff + curr_scan_start));
        
        /* 
         Compare values with max_value. If a byte in xmm_array is leq max_val_xmm,  
         ALL the corresponding bits of the corresponding byte in cmp_array are set to 1.
        */
        
        #if defined(__AVX512F__)
            cmp_mask = _mm_cmple_epu8_mask(xmm_array, max_val_xmm);
        #else
            cmp_array = LesserOrEqual8uSSE(xmm_array, max_val_xmm);
        
            // Create a mask using the most-significant bit of each byte value in cmp_array
            cmp_mask = _mm_movemask_epi8(cmp_array);
        #endif
        
        // Return index of first non-zero bit in mask
        // This corresponds to the first non-zero byte in cmp_array 
        if(cmp_mask){
            return_pos = curr_scan_start + (__builtin_ffs(cmp_mask) - 1);
            return return_pos;            
        }
    }
    
    return end_position;
}
#endif

#if defined(__AVX2__)
uint64_t AVX_Chunking_Technique::range_scan_leq_avx256(char *buff, uint64_t start_position, uint64_t end_position, uint8_t target_value){
    
    uint64_t num_vectors = (end_position - start_position) / AVX256_REGISTER_SIZE_BYTES;
    uint64_t curr_scan_start;

    // Structures to store bytes from data stream and comparison results in 256-bit AVX format
    __m256i xmm_array, cmp_array;
    uint32_t cmp_mask;

    // Load max_value into xmm-format
    __m256i max_val_xmm = _mm256_set1_epi8((char)target_value);
    
    for(uint64_t i = 0; i < num_vectors; i++){
        curr_scan_start = start_position + (i * AVX256_REGISTER_SIZE_BYTES);
        // Load data into xmm register
        xmm_array = _mm256_loadu_si256((__m256i const *)(buff + curr_scan_start));
        
        /* 
         Compare values with max_value. If a byte in xmm_array is leq max_val_xmm,  
         ALL the corresponding bits of the corresponding byte in cmp_array are set to 1.
        */
        
        #if defined(__AVX512F__)
            cmp_mask = _mm256_cmple_epu8_mask(xmm_array, max_val_xmm);
        #else
            cmp_array = LesserOrEqual8uAVX256(xmm_array, max_val_xmm);
        
            // Create a mask using the most-significant bit of each byte value in cmp_array
            cmp_mask = _mm256_movemask_epi8(cmp_array);
        #endif

        // Return index of first non-zero bit in mask
        // This corresponds to the first non-zero byte in cmp_array 
        if(cmp_mask){
            return curr_scan_start + (__builtin_ffs(cmp_mask) - 1);            
        }
    }
    
    return end_position;
}
#endif

#if defined(__AVX512F__)
uint64_t AVX_Chunking_Technique::range_scan_leq_avx512(char *buff, uint64_t start_position, uint64_t end_position, uint8_t target_value){
    
    uint64_t num_vectors = (end_position - start_position) / AVX512_REGISTER_SIZE_BYTES;
    uint64_t curr_scan_start;
    uint64_t return_pos;

    // Structures to store bytes from data stream and comparison results in 512-bit AVX format
    __m512i xmm_array;
    __mmask64 cmp_mask;

    // Load max_value into xmm-format
    __m512i max_val_xmm = _mm512_set1_epi8((char)target_value);
    
    for(uint64_t i = 0; i < num_vectors; i++){
        curr_scan_start = start_position + (i * AVX512_REGISTER_SIZE_BYTES);
        // Load data into xmm register
        xmm_array = _mm512_loadu_si512((__m512i const *)(buff + curr_scan_start));
        
        /* 
         Compare values with max_value. If a byte in xmm_array is leq max_val_xmm,  
         ALL the corresponding bits of the corresponding byte in cmp_array are set to 1.
        */
        
        cmp_mask = _mm512_cmple_epu8_mask(xmm_array, max_val_xmm);

        // Return index of first non-zero bit in mask
        // This corresponds to the first non-zero byte in cmp_array 
        if(cmp_mask){
            return_pos = curr_scan_start + (__builtin_ffsll(cmp_mask) - 1);
            return return_pos;            
        }
    }
    
    return end_position;
}
#endif

#if defined(__ARM_NEON)
uint64_t AVX_Chunking_Technique::range_scan_leq_neon(char *buff, uint64_t start_position, uint64_t end_position, uint8_t target_value){
    
    uint64_t num_vectors = (end_position - start_position) / NEON_REGISTER_SIZE_BYTES;
    uint64_t curr_scan_start;

    // Structures to store bytes from data stream and comparison results in 128-bit NEON format
    uint8x16_t xmm_array, cmp_array;
    uint8x16_t max_val_xmm = vdupq_n_u8((uint8_t)target_value);
    
    for(uint64_t i = 0; i < num_vectors; i++){
        curr_scan_start = start_position + (i * NEON_REGISTER_SIZE_BYTES);
        // Load data into xmm register
        xmm_array = vld1q_u8((uint8_t const *)(buff + curr_scan_start));
        
        /* 
         Compare values with max_value. If a byte in xmm_array is leq max_val_xmm,  
         ALL the corresponding bits of the corresponding byte in cmp_array are set to 1.
        */
        
        cmp_array = vcleq_u8(xmm_array, max_val_xmm);

        // ARM doesn't have native MOVEMASK instructions like x86 does.
        // Following advice from an ARM community blog post to implement mask creation instead
        // https://community.arm.com/arm-community-blogs/b/servers-and-cloud-computing-blog/posts/porting-x86-vector-bitmask-optimizations-to-arm-neon
        
        // Create a mask using the most-significant bit of each byte value in cmp_array
        
        uint64_t cmp_mask = vget_lane_u64(vreinterpret_u64_u8(vshrn_n_u16(vreinterpretq_u16_u8(cmp_array), 4)), 0);

        // Return index of first non-zero bit in mask
        // This corresponds to the first non-zero byte in cmp_array 
        if(cmp_mask){
            return curr_scan_start + ((__builtin_ffsll(cmp_mask) - 1) >> 2); // Shift right by 2 to account for 4 bits per byte            
        }
    }
    
    return end_position;
}
#endif

#ifdef __ALTIVEC__
uint64_t AVX_Chunking_Technique::range_scan_leq_altivec(char *buff, uint64_t start_position, uint64_t end_position, uint8_t target_value){
    
    uint64_t num_vectors = (end_position - start_position) / ALTIVEC_REGISTER_SIZE_BYTES;
    uint64_t curr_scan_start;

    // Structures to store bytes from data stream and comparison results in 128-bit AltiVec format
    __vector unsigned char vec_array;
    __vector unsigned char perm_array;
    __vector __bool char cmp_array;
    __vector unsigned char max_val_vec = vec_splats((unsigned char)target_value);
    __vector unsigned char bit_selector = {0, 8, 16, 24, 32, 40, 48, 56, 64, 72, 80, 88, 96, 104, 112, 120}; // select most significant bit from each byte                    

    uint8_t cond_mask_1, cond_mask_2;    

    for(uint64_t i = 0; i < num_vectors; i++){
        curr_scan_start = start_position + (i * ALTIVEC_REGISTER_SIZE_BYTES);
        
        // Load data into vec register
        // Use vec_xl instead of vec_ld because vec_ld truncates the load address to a 16 byte boundary
        // vec_xl allows loading from any address, which is necessary for unaligned data
        vec_array = vec_xl(0, (unsigned char const *)(buff + curr_scan_start));
                
        /* 
         Compare values with max_value. If a byte in vec_array is leq max_val_vec,  
         ALL the corresponding bits of the corresponding byte in cmp_array are set to 1.
        */
        
        cmp_array = vec_cmple(vec_array, max_val_vec);
        
        // Create masks using the most-significant bit of each byte value in cmp_array
        perm_array = vec_bperm((__vector unsigned char)cmp_array, bit_selector);
        cond_mask_1 = vec_extract(perm_array, 8);
        cond_mask_2 = vec_extract(perm_array, 9);
 
       // If cond_mask is not 0, it means at least one byte in cmp_array is non-zero
       if (cond_mask_2){
            return curr_scan_start + __builtin_clz(cond_mask_2) - 24 ; 
        } 
       else if(cond_mask_1){
            return curr_scan_start + __builtin_clz(cond_mask_1) - 16; // Add 8
        }
    }
    return end_position;
}
#endif

/**
 * @brief: Range scan functions for strictly less than comparison
 * These functions are used to find the first position in the data stream matching a target comparison
 * The functions are implemented for SSE, AVX2, AVX512, NEON and AltiVec instruction sets.
 */

#if defined(__SSE3__)
uint64_t AVX_Chunking_Technique::range_scan_lt_sse128(char *buff, uint64_t start_position, uint64_t end_position, uint8_t target_value){
    
    uint64_t num_vectors = (end_position - start_position) / SSE_REGISTER_SIZE_BYTES;
    uint64_t curr_scan_start;
    uint64_t return_pos;

    // Structures to store bytes from data stream and comparison results in 128-bit SSE format
    __m128i xmm_array, cmp_array;
    int cmp_mask;

    // Load max_value into xmm-format
    __m128i max_val_xmm = _mm_set1_epi8((char)target_value);
    
    for(uint64_t i = 0; i < num_vectors; i++){
        curr_scan_start = start_position + (i * SSE_REGISTER_SIZE_BYTES);
        // Load data into xmm register
        xmm_array = _mm_loadu_si128((__m128i const *)(buff + curr_scan_start));
        
        /* 
         Compare values with max_value. If a byte in xmm_array is lt max_val_xmm,  
         ALL the corresponding bits of the corresponding byte in cmp_array are set to 1.
        */
        
        #if defined(__AVX512F__)
            cmp_mask = _mm_cmplt_epu8_mask(xmm_array, max_val_xmm);
        #else
            cmp_array = Lesser8uSSE(xmm_array, max_val_xmm);
        
            // Create a mask using the most-significant bit of each byte value in cmp_array
            cmp_mask = _mm_movemask_epi8(cmp_array);
        #endif
        
        // Return index of first non-zero bit in mask
        // This corresponds to the first non-zero byte in cmp_array 
        if(cmp_mask){
            return_pos = curr_scan_start + (__builtin_ffs(cmp_mask) - 1);
            return return_pos;            
        }
    }
    
    return end_position;
}
#endif

#if defined(__AVX2__)
uint64_t AVX_Chunking_Technique::range_scan_lt_avx256(char *buff, uint64_t start_position, uint64_t end_position, uint8_t target_value){
    
    uint64_t num_vectors = (end_position - start_position) / AVX256_REGISTER_SIZE_BYTES;
    uint64_t curr_scan_start;

    // Structures to store bytes from data stream and comparison results in 256-bit AVX format
    __m256i xmm_array, cmp_array;
    uint32_t cmp_mask;

    // Load max_value into xmm-format
    __m256i max_val_xmm = _mm256_set1_epi8((char)target_value);
    
    for(uint64_t i = 0; i < num_vectors; i++){
        curr_scan_start = start_position + (i * AVX256_REGISTER_SIZE_BYTES);
        // Load data into xmm register
        xmm_array = _mm256_loadu_si256((__m256i const *)(buff + curr_scan_start));
        
        /* 
         Compare values with max_value. If a byte in xmm_array is lt max_val_xmm,  
         ALL the corresponding bits of the corresponding byte in cmp_array are set to 1.
        */
        
        #if defined(__AVX512F__)
            cmp_mask = _mm256_cmplt_epu8_mask(xmm_array, max_val_xmm);
        #else
            cmp_array = Lesser8uAVX256(xmm_array, max_val_xmm);
        
            // Create a mask using the most-significant bit of each byte value in cmp_array
            cmp_mask = _mm256_movemask_epi8(cmp_array);
        #endif

        // Return index of first non-zero bit in mask
        // This corresponds to the first non-zero byte in cmp_array 
        if(cmp_mask){
            return curr_scan_start + (__builtin_ffs(cmp_mask) - 1);            
        }
    }
    
    return end_position;
}
#endif

#if defined(__AVX512F__)
uint64_t AVX_Chunking_Technique::range_scan_lt_avx512(char *buff, uint64_t start_position, uint64_t end_position, uint8_t target_value){
    
    uint64_t num_vectors = (end_position - start_position) / AVX512_REGISTER_SIZE_BYTES;
    uint64_t curr_scan_start;
    uint64_t return_pos;

    // Structures to store bytes from data stream and comparison results in 512-bit AVX format
    __m512i xmm_array;
    __mmask64 cmp_mask;

    // Load max_value into xmm-format
    __m512i max_val_xmm = _mm512_set1_epi8((char)target_value);
    
    for(uint64_t i = 0; i < num_vectors; i++){
        curr_scan_start = start_position + (i * AVX512_REGISTER_SIZE_BYTES);
        // Load data into xmm register
        xmm_array = _mm512_loadu_si512((__m512i const *)(buff + curr_scan_start));
        
        /* 
         Compare values with max_value. If a byte in xmm_array is lt max_val_xmm,  
         ALL the corresponding bits of the corresponding byte in cmp_array are set to 1.
        */
        
        cmp_mask = _mm512_cmplt_epu8_mask(xmm_array, max_val_xmm);

        // Return index of first non-zero bit in mask
        // This corresponds to the first non-zero byte in cmp_array 
        if(cmp_mask){
            return_pos = curr_scan_start + (__builtin_ffsll(cmp_mask) - 1);
            return return_pos;            
        }
    }
    
    return end_position;
}
#endif

#if defined(__ARM_NEON)
uint64_t AVX_Chunking_Technique::range_scan_lt_neon(char *buff, uint64_t start_position, uint64_t end_position, uint8_t target_value){
    
    uint64_t num_vectors = (end_position - start_position) / NEON_REGISTER_SIZE_BYTES;
    uint64_t curr_scan_start;

    // Structures to store bytes from data stream and comparison results in 128-bit NEON format
    uint8x16_t xmm_array, cmp_array;
    uint8x16_t max_val_xmm = vdupq_n_u8((uint8_t)target_value);
    
    for(uint64_t i = 0; i < num_vectors; i++){
        curr_scan_start = start_position + (i * NEON_REGISTER_SIZE_BYTES);
        // Load data into xmm register
        xmm_array = vld1q_u8((uint8_t const *)(buff + curr_scan_start));
        
        /* 
         Compare values with max_value. If a byte in xmm_array is lt max_val_xmm,  
         ALL the corresponding bits of the corresponding byte in cmp_array are set to 1.
        */
        
        cmp_array = vcltq_u8(xmm_array, max_val_xmm);

        // ARM doesn't have native MOVEMASK instructions like x86 does.
        // Following advice from an ARM community blog post to implement mask creation instead
        // https://community.arm.com/arm-community-blogs/b/servers-and-cloud-computing-blog/posts/porting-x86-vector-bitmask-optimizations-to-arm-neon
        
        // Create a mask using the most-significant bit of each byte value in cmp_array
        
        uint64_t cmp_mask = vget_lane_u64(vreinterpret_u64_u8(vshrn_n_u16(vreinterpretq_u16_u8(cmp_array), 4)), 0);

        // Return index of first non-zero bit in mask
        // This corresponds to the first non-zero byte in cmp_array 
        if(cmp_mask){
            return curr_scan_start + ((__builtin_ffsll(cmp_mask) - 1) >> 2); // Shift right by 2 to account for 4 bits per byte            
        }
    }
    
    return end_position;
}
#endif

#ifdef __ALTIVEC__
uint64_t AVX_Chunking_Technique::range_scan_lt_altivec(char *buff, uint64_t start_position, uint64_t end_position, uint8_t target_value){
    uint64_t num_vectors = (end_position - start_position) / ALTIVEC_REGISTER_SIZE_BYTES;
    uint64_t curr_scan_start;

    // Structures to store bytes from data stream and comparison results in 128-bit AltiVec format
    __vector unsigned char vec_array;
    __vector unsigned char perm_array;
    __vector __bool char cmp_array;
    __vector unsigned char max_val_vec = vec_splats((unsigned char)target_value);
    __vector unsigned char bit_selector = {0, 8, 16, 24, 32, 40, 48, 56, 64, 72, 80, 88, 96, 104, 112, 120}; // select most significant bit from each byte                    

    uint8_t cond_mask_1, cond_mask_2;    

    for(uint64_t i = 0; i < num_vectors; i++){
        curr_scan_start = start_position + (i * ALTIVEC_REGISTER_SIZE_BYTES);
        
        // Load data into vec register
        // Use vec_xl instead of vec_ld because vec_ld truncates the load address to a 16 byte boundary
        // vec_xl allows loading from any address, which is necessary for unaligned data
        vec_array = vec_xl(0, (unsigned char const *)(buff + curr_scan_start));
                
        /* 
         Compare values with max_value. If a byte in vec_array is lt max_val_vec,  
         ALL the corresponding bits of the corresponding byte in cmp_array are set to 1.
        */
        
        cmp_array = vec_cmplt(vec_array, max_val_vec);
        
        // Create masks using the most-significant bit of each byte value in cmp_array
        perm_array = vec_bperm((__vector unsigned char)cmp_array, bit_selector);
        
        // bperm stores masks in elements 8 and 9 of the vector
        // Extract the masks from the perm_array
        // cond_mask_2 corresponds to the first 8 bytes, cond_mask_1 corresponds to the next 8 bytes
        // This is because vec_bperm rearranges the bits in little endian order
        cond_mask_1 = vec_extract(perm_array, 8);
        cond_mask_2 = vec_extract(perm_array, 9);
 
       // If cond_mask is not 0, it means at least one byte in cmp_array is non-zero
       // Subtract 24 because clz implicitly converts to int with a 4-byte size
       if (cond_mask_2){
            return curr_scan_start + __builtin_clz(cond_mask_2) - 24 ; 
        } 
       else if(cond_mask_1){
            return curr_scan_start + __builtin_clz(cond_mask_1) - 16; // Add 8 to the index to account for the first 8 bytes in the vector
        }
    }
    return end_position;
}
#endif
