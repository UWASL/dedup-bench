/**
 * @file maxp_chunking.cpp
 * @author WASL
 * @brief Implementations for MAXP chunking technique
 * @version 0.1
 * @date 2025-3-26
 *
 * @copyright Copyright (c) 2023
 *
 */

 #include <fstream>
 #include "maxp_chunking.hpp"
 
 MAXP_Chunking::MAXP_Chunking(){
    window_size = DEFAULT_MAXP_WINDOW_SIZE;
    max_block_size = DEFAULT_MAXP_MAX_BLOCK_SIZE;
    simd_mode = SIMD_Mode::NONE;

    
    #ifdef __SSE3__
    xmm_array = nullptr;
    #endif

    #ifdef __AVX2__
    ymm_array = nullptr;
    #endif

    #if defined(__AVX512F__)
    zmm_array = nullptr;
    #endif

    #if defined(__ARM_NEON)
    neon_array = nullptr;
    #endif

    #ifdef __ALTIVEC__
    altivec_array = nullptr;
    #endif

    chunk_counter = 0;
 }
 
 MAXP_Chunking::MAXP_Chunking(const Config &config){
    window_size = config.get_maxp_window_size();
    max_block_size = config.get_maxp_max_block_size();
    simd_mode = config.get_simd_mode();

    #ifdef __SSE3__
    xmm_array = nullptr;
    #endif

    #ifdef __AVX2__
    ymm_array = nullptr;
    #endif
    
    #if defined(__AVX512F__)
    zmm_array = nullptr;
    #endif

    #if defined(__ARM_NEON)
    neon_array = nullptr;
    #endif

    #ifdef __ALTIVEC__
    altivec_array = nullptr;
    #endif

    chunk_counter = 0;

    if(simd_mode == SIMD_Mode::NONE) {
        // No SIMD mode selected
        // Pass
    }

    #ifdef __SSE3__
    else if(simd_mode == SIMD_Mode::SSE128) {
        if(window_size % SSE_REGISTER_SIZE_BYTES != 0 || (window_size / 16) % 2 != 0) {
            // Check if window size is a multiple of 16
            // Check if window size makes an even number of vectors for find_maximum_sse128
            std::cout << "MAXP window size currently unsupported by SSE128. Please use an even multiple of SSE128_REGISTER_SIZE_BYTES (default 16)." << std::endl;
            exit(1);
        }
        uint64_t num_vectors = window_size / SSE_REGISTER_SIZE_BYTES;
        xmm_array = new __m128i[num_vectors]();
        if(xmm_array == nullptr) {
            std::cout << "Error allocating memory for 128-bit vectors (__m128i)" << std::endl;
            exit(1);
        }
    }
    #endif

    #ifdef __AVX2__
    else if(simd_mode == SIMD_Mode::AVX256) {
        if(window_size % AVX256_REGISTER_SIZE_BYTES != 0 || (window_size / 32) % 2 != 0) {
            // Check if window size is a multiple of 32
            // Check if window size makes an even number of vectors for find_maximum_AVX256
            std::cout << "MAXP window size currently unsupported by AVX256. Please use an even multiple of AVX_REGISTER_SIZE_BYTES (default 32)." << std::endl;
            exit(1);
        }
        uint64_t num_vectors = window_size / AVX256_REGISTER_SIZE_BYTES;
        ymm_array = new __m256i[num_vectors]();
        if(ymm_array == nullptr) {
            std::cout << "Error allocating memory for 256-bit vectors(__m256i)" << std::endl;
            exit(1);
        }
    }
    #endif

    #if defined(__AVX512F__)
    else if(simd_mode == SIMD_Mode::AVX512) {
        if(window_size % AVX512_REGISTER_SIZE_BYTES != 0 || (window_size / 64) % 2 != 0) {
            // Check if window size is a multiple of 64
            // Check if window size makes an even number of vectors for find_maximum_AVX512
            std::cout << "MAXP window size currently unsupported by AVX512. Please use an even multiple of AVX_REGISTER_SIZE_BYTES (default 64)." << std::endl;
            exit(1);
        }
        uint64_t num_vectors = window_size / AVX512_REGISTER_SIZE_BYTES;
        zmm_array = new __m512i[num_vectors]();
        if(zmm_array == nullptr) {
            std::cout << "Error allocating memory for 512-bit vectors(__m512i)" << std::endl;
            exit(1);
        }
    }
    #endif

    #if defined(__ARM_NEON)
    else if(simd_mode == SIMD_Mode::NEON) {
        if(window_size % NEON_REGISTER_SIZE_BYTES != 0 || (window_size / 16) % 2 != 0) {
            // Check if window size is a multiple of 16
            // Check if window size makes an even number of vectors for find_maximum_neon
            std::cout << "MAXP window size currently unsupported by NEON. Please use an even multiple of NEON_REGISTER_SIZE_BYTES (default 16)." << std::endl;
            exit(1);
        }
        uint64_t num_vectors = window_size / NEON_REGISTER_SIZE_BYTES;
        neon_array = new uint8x16_t[num_vectors]();
        if(neon_array == nullptr) {
            std::cout << "Error allocating memory for NEON vectors(uint8x16_t)" << std::endl;
            exit(1);
        }
    }
    #endif

    #ifdef __ALTIVEC__
    else if(simd_mode == SIMD_Mode::ALTIVEC) {
        if(window_size % ALTIVEC_REGISTER_SIZE_BYTES != 0 || (window_size / 16) % 2 != 0) {
            // Check if window size is a multiple of 16
            // Check if window size makes an even number of vectors for find_maximum_altivec
            std::cout << "MAXP window size currently unsupported by ALTIVEC. Please use an even multiple of ALTIVEC_REGISTER_SIZE_BYTES (default 16)." << std::endl;
            exit(1);
        }
        uint64_t num_vectors = window_size / ALTIVEC_REGISTER_SIZE_BYTES;
        altivec_array = new __vector unsigned char[num_vectors]();
        if(altivec_array == nullptr) {
            std::cout << "Error allocating memory for ALTIVEC vectors(__vector unsigned char)" << std::endl;
            exit(1);
        }
    }
    #endif
    
    else {
        std::cout << "Unsupported SIMD Mode for MAXP" << std::endl;
        exit(1);
    }
 }

 #ifdef __SSE3__
 uint64_t MAXP_Chunking::find_cutpoint_sse128(char *buff, uint64_t size){
    if(size < (2 * window_size) + 1)
        return size;
    
    // Cap out max size
    size = std::min(size, max_block_size);

    uint8_t max_value;
    uint8_t backward_max = 0;
    uint64_t max_pos = window_size;

    uint64_t return_pos_range_scan;

    while(max_pos < (size - window_size)){

        max_value = (uint8_t)buff[max_pos];

        return_pos_range_scan = range_scan_geq_sse128(buff, max_pos + 1, max_pos + 1 + window_size, max_value);
        if(return_pos_range_scan == max_pos + window_size + 1){
            // No match found i.e. all bytes in range are less than max_value
            // Start backward scan to verify if target is a local max

            backward_max = find_maximum_sse128(buff, max_pos - window_size, max_pos, xmm_array);
            
            // No bytes > max value in the backward region i.e. max_value is a local max
            // Chunk boundary found
            if(backward_max <= max_value){
                return max_pos;
            }
            else {
                // Nothing until max_pos + window_size + 1 can be a chunk boundary now
                max_pos += window_size + 1;
            }

        }
        else {
            // Update maximum value
            max_pos = return_pos_range_scan;
        }
    }

    // No chunk boundary found
    return size;

}
#endif

#ifdef __AVX2__
uint64_t MAXP_Chunking::find_cutpoint_avx256(char *buff, uint64_t size){
    if(size < (2 * window_size) + 1)
        return size;

    // Cap out max size
    size = std::min(size, max_block_size);

    uint8_t max_value;
    uint8_t backward_max = 0;
    uint64_t max_pos = window_size;

    uint64_t return_pos_range_scan;

    while(max_pos < (size - window_size)){

        max_value = (uint8_t)buff[max_pos];

        return_pos_range_scan = range_scan_geq_avx256(buff, max_pos + 1, max_pos + 1 + window_size, max_value);
        if(return_pos_range_scan == max_pos + window_size + 1){
            // No match found i.e. all bytes in range are less than max_value
            // Start backward scan to verify if target is a local max

            backward_max = find_maximum_avx256(buff, max_pos - window_size, max_pos, ymm_array);
            
            // No bytes > max value in the backward region i.e. max_value is a local max
            // Chunk boundary found
            if(backward_max <= max_value){
                return max_pos;
            }
            else {
                // Nothing until max_pos + window_size + 1 can be a chunk boundary now
                max_pos += window_size + 1;
            }

        }
        else {
            // Update maximum value
            max_pos = return_pos_range_scan;
        }
    }

    // No chunk boundary found
    return size;
}
#endif
    
#if defined(__AVX512F__)
uint64_t MAXP_Chunking::find_cutpoint_avx512(char *buff, uint64_t size){
    if(size < (2 * window_size) + 1)
        return size;

    // Cap out max size
    size = std::min(size, max_block_size);

    uint8_t max_value;
    uint8_t backward_max = 0;
    uint64_t max_pos = window_size;

    uint64_t return_pos_range_scan;

    while(max_pos < (size - window_size)){

        max_value = (uint8_t)buff[max_pos];

        return_pos_range_scan = range_scan_geq_avx512(buff, max_pos + 1, max_pos + 1 + window_size, max_value);
        if(return_pos_range_scan == max_pos + window_size + 1){
            // No match found i.e. all bytes in range are less than max_value
            // Start backward scan to verify if target is a local max

            backward_max = find_maximum_avx512(buff, max_pos - window_size, max_pos, zmm_array);
            
            // No bytes > max value in the backward region i.e. max_value is a local max
            // Chunk boundary found
            if(backward_max <= max_value){
                return max_pos;
            }
            else {
                // Nothing until max_pos + window_size + 1 can be a chunk boundary now
                max_pos += window_size + 1;
            }

        }
        else {
            // Update maximum value
            max_pos = return_pos_range_scan;
        }
    }

    // No chunk boundary found
    return size;

}
#endif

#if defined(__ARM_NEON)
uint64_t MAXP_Chunking::find_cutpoint_neon(char *buff, uint64_t size){
    if(size < (2 * window_size) + 1)
        return size;

    // Cap out max size
    size = std::min(size, max_block_size);

    uint8_t max_value;
    uint8_t backward_max = 0;
    uint64_t max_pos = window_size;

    uint64_t return_pos_range_scan;

    while(max_pos < (size - window_size)){

        max_value = (uint8_t)buff[max_pos];

        return_pos_range_scan = range_scan_geq_neon(buff, max_pos + 1, max_pos + 1 + window_size, max_value);
        if(return_pos_range_scan == max_pos + window_size + 1){
            // No match found i.e. all bytes in range are less than max_value
            // Start backward scan to verify if target is a local max

            backward_max = find_maximum_neon(buff, max_pos - window_size, max_pos, neon_array);
            
            // No bytes > max value in the backward region i.e. max_value is a local max
            // Chunk boundary found
            if(backward_max <= max_value){
                return max_pos;
            }
            else {
                // Nothing until max_pos + window_size + 1 can be a chunk boundary now
                max_pos += window_size + 1;
            }

        }
        else {
            // Update maximum value
            max_pos = return_pos_range_scan;
        }
    }

    // No chunk boundary found
    return size;

}
#endif

#ifdef __ALTIVEC__
uint64_t MAXP_Chunking::find_cutpoint_altivec(char *buff, uint64_t size){
    if(size < (2 * window_size) + 1){
        return size;
    }
    
    // Cap out max size
    size = std::min(size, max_block_size);
    uint8_t max_value;
    uint8_t backward_max = 0;
    uint64_t max_pos = window_size;
    uint64_t return_pos_range_scan;

    while(max_pos < (size - window_size)){

        max_value = (uint8_t)buff[max_pos];
        
        return_pos_range_scan = range_scan_geq_altivec(buff, max_pos + 1, max_pos + 1 + window_size, max_value);
        
        if(return_pos_range_scan == max_pos + window_size + 1){
            // No match found i.e. all bytes in range are less than max_value
            // Start backward scan to verify if target is a local max
        
            backward_max = find_maximum_altivec(buff, max_pos - window_size, max_pos, altivec_array);
        
            // No bytes > max value in the backward region i.e. max_value is a local max
            // Chunk boundary found
            if(backward_max <= max_value){
                return max_pos;
            }
            else {
                // Nothing until max_pos + window_size + 1 can be a chunk boundary now
                max_pos += window_size + 1;
            }

        }
        else {
            // Update maximum value
            max_pos = return_pos_range_scan;
        }
    }

    // N:wqo chunk boundary found
    return size;
}
#endif

uint64_t MAXP_Chunking::find_cutpoint_native(char *buff, uint64_t size){
 
     if(size < (2 * window_size) + 1){
        return size;
     }


     size = std::min(size, max_block_size);

     uint64_t max_position = window_size;
     uint8_t max_value = (uint8_t)buff[max_position];
     uint64_t j;
     bool local_max_found = false;
 
     // Scan window from left to right looking for a byte with a value > all bytes in a "window_size" after it
     for(uint64_t i = window_size; i < size - 1; i++){
 
         if((uint8_t)buff[i] >= max_value){
            max_position = i;
            max_value = (uint8_t)buff[i];
         }
         else if(i == max_position + window_size){ 
            // Target byte found
            // Initiate backward scanning to see if i is local maximum
            local_max_found = true;
                
            for(j = max_position - window_size; j < max_position; j++){
                if((uint8_t)buff[j] > max_value){
                    max_position = i+1;
                    max_value = (uint8_t)buff[i+1];
                    local_max_found = false;
                    break;
                }
            }
 
            // Insert chunk boundary at max_position (not i) i.e. chunk boundary does not include right window
            if(local_max_found == true){
                return max_position;
            }
        }
    }
    
    // Returning maximum chunk size
    return size;
 }

 uint64_t MAXP_Chunking::find_cutpoint(char *buff, uint64_t size){
    chunk_counter++;
    if(simd_mode == SIMD_Mode::NONE) {
        return find_cutpoint_native(buff, size);
    }
    
    #ifdef __SSE3__
    else if(simd_mode == SIMD_Mode::SSE128) {
        return find_cutpoint_sse128(buff, size);
    }
    #endif
    
    #ifdef __AVX2__
    else if(simd_mode == SIMD_Mode::AVX256) {
        return find_cutpoint_avx256(buff, size);
    }
    #endif

    #if defined(__AVX512F__)
    else if(simd_mode == SIMD_Mode::AVX512) {
        return find_cutpoint_avx512(buff, size);
    }
    #endif

    #if defined(__ARM_NEON)
    else if(simd_mode == SIMD_Mode::NEON) {
        return find_cutpoint_neon(buff, size);
    }
    #endif

    #ifdef __ALTIVEC__
    else if(simd_mode == SIMD_Mode::ALTIVEC) {
        return find_cutpoint_altivec(buff, size);
    }
    #endif

    else {
        std::cout << "Unsupported SIMD Mode for MAXP" << std::endl;
        exit(1);
    }
 }
 
 MAXP_Chunking::~MAXP_Chunking(){
    
    #ifdef __SSE3__
    if(xmm_array != nullptr)
        delete xmm_array;
    #endif

    #ifdef __AVX2__
    if(ymm_array != nullptr)
        delete ymm_array;
    #endif 

    #if defined(__AVX512F__)
    if(zmm_array != nullptr)
        delete zmm_array;
    #endif

    #if defined(__ARM_NEON__)
    if(neon_array != nullptr)
        delete neon_array;
    #endif  

    #ifdef __ALTIVEC__
    if(altivec_array != nullptr)
        delete altivec_array; 
    #endif
 }