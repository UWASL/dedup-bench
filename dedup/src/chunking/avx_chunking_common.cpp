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

uint8_t AVX_Chunking_Technique::find_maximum_sse128(char *buff, uint64_t start_pos, uint64_t end_pos, __m128i *xmm_array){

    // Assume window_size is a multiple of SSE_REGISTER_SIZE_BYTES for now
    // Assume num_vectors is even for now - True for most common window sizes. Can fix later via specific check

    uint64_t num_vectors = (end_pos - start_pos) / SSE_REGISTER_SIZE_BYTES;

    uint64_t step = 2;
    uint64_t half_step = 1;

    // Load contents into __m128i structures
    // Could be optimized later as only 16 xmm registers are avaiable per CPU in 64-bit
    for(uint64_t i = start_pos; i < num_vectors; i++)
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

uint8_t AVX_Chunking_Technique::find_maximum_avx256(char *buff, uint64_t start_pos, uint64_t end_pos, __m256i *xmm_array){

    // Assume window_size is a multiple of AVX256_REGISTER_SIZE_BYTES for now
    // Assume num_vectors is even for now - True for most common window sizes. Can fix later via specific check

    uint64_t num_vectors = (end_pos - start_pos) / AVX256_REGISTER_SIZE_BYTES;

    uint64_t step = 2;
    uint64_t half_step = 1;

    // Load contents into __m256i structures
    // Could be optimized later as only 16 xmm registers are avaiable per CPU in 64-bit
    for(uint64_t i = start_pos; i < num_vectors; i++)
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
    for(uint64_t i = start_pos; i < num_vectors; i++)
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
