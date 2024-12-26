/**
 * @file RAM_Chunking.cpp
 * @author WASL
 * @brief Implementations for AE chunking technique
 * @version 0.1
 * @date 2023-3-10
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "ram_chunking.hpp"
#include <fstream>

RAM_Chunking::RAM_Chunking() {
    avg_block_size = DEFAULT_RAM_AVG_BLOCK_SIZE;
    technique_name = "RAM Chunking";

    window_size = avg_block_size - 256;
    
    sse_array = nullptr;
    avx256_array = nullptr;
    avx512_array = nullptr;
   
    simd_mode = SIMD_Mode::NONE;

}

RAM_Chunking::RAM_Chunking(const Config& config) {
    avg_block_size = config.get_ram_avg_block_size();
    max_block_size = config.get_ram_max_block_size();
    window_size = avg_block_size - 256;
    // window_size = avg_block_size / (exp(1) - 1);  // avg_block size / e-1

    sse_array = nullptr;
    avx256_array = nullptr;
    avx512_array = nullptr;

    technique_name = "RAM Chunking";

    simd_mode = config.get_simd_mode();

    if(simd_mode == SIMD_Mode::SSE128 || simd_mode == SIMD_Mode::SSE128_NOSLIDE){
         uint64_t num_vectors = window_size / SSE_REGISTER_SIZE_BYTES;
         sse_array = new __m128i[num_vectors]();
    }
    else if(simd_mode == SIMD_Mode::AVX256){
        uint64_t num_vectors = window_size / AVX256_REGISTER_SIZE_BYTES;
        avx256_array = new __m256i[num_vectors]();
    }
    
    #if defined(__AVX512F__)
    else if(simd_mode == SIMD_Mode::AVX512){
        uint64_t num_vectors = window_size / AVX512_REGISTER_SIZE_BYTES;
        avx512_array = new __m512i[num_vectors]();
    }
    #endif    
}

RAM_Chunking::~RAM_Chunking() {
    if(simd_mode == SIMD_Mode::SSE128 || simd_mode == SIMD_Mode::SSE128_NOSLIDE)
        delete sse_array;
    else if(simd_mode == SIMD_Mode::AVX256)
        delete avx256_array;
   #if defined(__AVX512F__)
    else if(simd_mode == SIMD_Mode::AVX512)
        delete avx512_array;
    #endif
}

uint64_t RAM_Chunking::find_cutpoint(char* buff, uint64_t size) {
    uint32_t i = 0;
    uint8_t max_value = (uint8_t)buff[i];
    i++;
    if (size > max_block_size)
        size = max_block_size;
    else if(size < window_size)
        return size;

    // If SSE128 with normal sliding
    if (simd_mode == SIMD_Mode::SSE128_NOSLIDE){
        max_value = find_maximum_sse128(buff, 0, window_size, sse_array);
    }
    // If SIMD enabled, accelerate find_maximum() and slide depending on chosen SIMD mode 
    else if(simd_mode == SIMD_Mode::SSE128){
        max_value = find_maximum_sse128(buff, 0, window_size, sse_array);
        return get_return_position_sse128(buff, window_size, size, max_value);
    }
    else if(simd_mode == SIMD_Mode::AVX256){
        max_value = find_maximum_avx256(buff, 0, window_size, avx256_array);
        return get_return_position_avx256(buff, window_size, size, max_value);
    }
    #if defined(__AVX512F__)
    else if(simd_mode == SIMD_Mode::AVX512){
        max_value = find_maximum_avx512(buff, 0, window_size, avx512_array);
        return get_return_position_avx512(buff, window_size, size, max_value);
    }
    #endif
    
    // Else execute basic find_maximum, same as native RAM
    else {
        for(i = 0; i < window_size; i++){
            if ((uint8_t)buff[i] >= max_value)
	    	    max_value = (uint8_t)buff[i];
        }
    }

    for (i = window_size; i < size; i++) {
        if ((uint8_t)buff[i] >= max_value)
            return i;
    }

    return size;
}

uint64_t RAM_Chunking::get_return_position_sse128(char *buff, uint64_t start_position, uint64_t end_position, uint8_t max_value){
    
    uint64_t num_vectors = (end_position - start_position) / SSE_REGISTER_SIZE_BYTES;
    uint64_t curr_scan_start;

    // Structures to store bytes from data stream and comparison results in 128-bit SSE format
    __m128i xmm_array, cmp_array;
    int cmp_mask;

    // Load max_value into xmm-format
    __m128i max_val_xmm = _mm_set1_epi8((char)max_value);
    
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
        if(cmp_mask)
            return curr_scan_start + (__builtin_ffs(cmp_mask) - 1);
    }
 
    return end_position;
}

uint64_t RAM_Chunking::get_return_position_avx256(char *buff, uint64_t start_position, uint64_t end_position, uint8_t max_value){
    
    uint64_t num_vectors = (end_position - start_position) / AVX256_REGISTER_SIZE_BYTES;
    uint64_t curr_scan_start;

    // Structures to store bytes from data stream and comparison results in 128-bit SSE format
    __m256i xmm_array, cmp_array;
    uint32_t cmp_mask;

    // Load max_value into xmm-format
    __m256i max_val_xmm = _mm256_set1_epi8((char)max_value);
    
    for(uint64_t i = 0; i < num_vectors; i++){
        curr_scan_start = start_position + (i * AVX256_REGISTER_SIZE_BYTES);
        // Load data into xmm register
        xmm_array = _mm256_loadu_si256((__m256i const *)(buff + curr_scan_start));
        
        /* 

         Compare values with max_value. If a byte in xmm_array is geq max_val_xmm,  
         the corresponding bit of cmp_mask is set to 1.
        */
        #if defined(__AVX512F__)
             __mmask32 all_ones = UINT32_MAX;
            cmp_mask = _mm256_mask_cmpge_epu8_mask(all_ones, xmm_array, max_val_xmm);
        #else
            cmp_array = GreaterOrEqual8uAVX256(xmm_array, max_val_xmm);
        
            // Create a mask using the most-significant bit of each byte value in cmp_array
            cmp_mask = _mm256_movemask_epi8(cmp_array);
        #endif
        
        // Return index of first non-zero bit in mask
        // This corresponds to the first non-zero byte in cmp_array 
        if(cmp_mask)
            return curr_scan_start + (__builtin_ffs(cmp_mask) - 1);
    }

    return end_position;

}

#if defined(__AVX512F__)
    uint64_t RAM_Chunking::get_return_position_avx512(char *buff, uint64_t start_position, uint64_t end_position, uint8_t max_value){
        
        uint64_t num_vectors = (end_position - start_position) / AVX512_REGISTER_SIZE_BYTES;
        uint64_t curr_scan_start;

        // Structures to store bytes from data stream and comparison results in 128-bit SSE format
        __m512i xmm_array;
        uint64_t cmp_mask;

        // Load max_value into xmm-format
        __m512i max_val_xmm = _mm512_set1_epi8((char)max_value);
        
        for(uint64_t i = 0; i < num_vectors; i++){
            curr_scan_start = start_position + (i * AVX512_REGISTER_SIZE_BYTES);
            // Load data into xmm register
            xmm_array = _mm512_loadu_si512((__m512i const *)(buff + curr_scan_start));
            
            /* 
            Compare values with max_value. If a byte in xmm_array is geq max_val_xmm,  
            the corresponding bit of cmp_mask is set to 1.
            */

            // Return a mask with the most significant bit of GEQ comparison byte-wise
            cmp_mask = _mm512_cmpge_epu8_mask(xmm_array, max_val_xmm);

            // Return index of first non-zero bit in mask
            // This corresponds to the first non-zero byte in cmp_array 
            if(cmp_mask)
                return curr_scan_start + (__builtin_ffsll(cmp_mask) - 1);
        }

        return end_position;
    }
#endif
