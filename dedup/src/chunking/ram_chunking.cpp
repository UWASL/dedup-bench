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
    
    #ifdef __SSE3__
    sse_array = nullptr;
    #endif
    
    #ifdef __AVX2__
    avx256_array = nullptr;
    #endif
    
    #if defined(__AVX512F__)
        avx512_array = nullptr;
    #endif
    
    #ifdef __ARM_NEON
        neon_array = nullptr;
    #endif

    #ifdef __ALTIVEC__
        altivec_array = nullptr;
    #endif
   
    simd_mode = SIMD_Mode::NONE;

}

RAM_Chunking::RAM_Chunking(const Config& config) {
    avg_block_size = config.get_ram_avg_block_size();
    max_block_size = config.get_ram_max_block_size();
    window_size = avg_block_size - 256;
    // window_size = avg_block_size / (exp(1) - 1);  // avg_block size / e-1

    #ifdef __SSE3__
        sse_array = nullptr;
    #endif
    
    #ifdef __AVX2__
        avx256_array = nullptr;
    #endif
    
    #if defined(__AVX512F__)
        avx512_array = nullptr;
    #endif

    #ifdef __ARM_NEON
        neon_array = nullptr;
    #endif

    #ifdef __ALTIVEC__
        altivec_array = nullptr;
    #endif
    
    technique_name = "RAM Chunking";

    simd_mode = config.get_simd_mode();

    if(simd_mode == SIMD_Mode::NONE){

    }

    #ifdef __SSE3__
    else if(simd_mode == SIMD_Mode::SSE128){
         uint64_t num_vectors = window_size / SSE_REGISTER_SIZE_BYTES;
         sse_array = new __m128i[num_vectors]();
    }
    #endif
    
    #ifdef __AVX2__
    else if(simd_mode == SIMD_Mode::AVX256){
        uint64_t num_vectors = window_size / AVX256_REGISTER_SIZE_BYTES;
        avx256_array = new __m256i[num_vectors]();
    }
    #endif
    
    #if defined(__AVX512F__)
    else if(simd_mode == SIMD_Mode::AVX512){
        uint64_t num_vectors = window_size / AVX512_REGISTER_SIZE_BYTES;
        avx512_array = new __m512i[num_vectors]();
    }
    #endif
    
    #ifdef __ARM_NEON
    else if(simd_mode == SIMD_Mode::NEON){
        uint64_t num_vectors = window_size / NEON_REGISTER_SIZE_BYTES;
        neon_array = new uint8x16_t[num_vectors]();
    }
    #endif

    #ifdef __ALTIVEC__
    else if(simd_mode == SIMD_Mode::ALTIVEC){
        uint64_t num_vectors = window_size / ALTIVEC_REGISTER_SIZE_BYTES;
        altivec_array = new __vector unsigned char[num_vectors]();
    }
    #endif

    else {
        std::cout << "Error: Unsupported SIMD mode" << std::endl;
        exit(EXIT_FAILURE);
    }
}

RAM_Chunking::~RAM_Chunking() {

    if(simd_mode == SIMD_Mode::NONE){
        // No SIMD mode, nothing to delete
        return;
    }

    #ifdef __SSE3__
    else if(simd_mode == SIMD_Mode::SSE128)
        delete sse_array;
    #endif

    #ifdef __AVX2__
    else if(simd_mode == SIMD_Mode::AVX256)
        delete avx256_array;
    #endif

   #if defined(__AVX512F__)
    else if(simd_mode == SIMD_Mode::AVX512)
        delete avx512_array;
    #endif

    #ifdef __ARM_NEON
    else if(simd_mode == SIMD_Mode::NEON)
        delete neon_array;
    #endif

    #ifdef __ALTIVEC__
    else if(simd_mode == SIMD_Mode::ALTIVEC)
        delete altivec_array;
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

    if(simd_mode == SIMD_Mode::NONE){
        // If no SIMD enabled, execute basic find_maximum
        for(i = 0; i < window_size; i++){
            if ((uint8_t)buff[i] >= max_value)
	    	    max_value = (uint8_t)buff[i];
        }

        for (i = window_size; i < size; i++) {
            if ((uint8_t)buff[i] >= max_value)
                return i;
        }

    }

    #ifdef __SSE3__
    // If SIMD enabled, accelerate find_maximum() and slide depending on chosen SIMD mode 
    else if(simd_mode == SIMD_Mode::SSE128){
        max_value = find_maximum_sse128(buff, 0, window_size, sse_array);
        return range_scan_geq_sse128(buff, window_size, size, max_value);
    }
    #endif

    #ifdef __AVX2__
    else if(simd_mode == SIMD_Mode::AVX256){
        max_value = find_maximum_avx256(buff, 0, window_size, avx256_array);
        return range_scan_geq_avx256(buff, window_size, size, max_value);
    }
    #endif

    #if defined(__AVX512F__)
    else if(simd_mode == SIMD_Mode::AVX512){
        max_value = find_maximum_avx512(buff, 0, window_size, avx512_array);
        return range_scan_geq_avx512(buff, window_size, size, max_value);
    }
    #endif

    #if defined(__ARM_NEON)
    else if(simd_mode == SIMD_Mode::NEON){
        max_value = find_maximum_neon(buff, 0, window_size, neon_array);
        return range_scan_geq_neon(buff, window_size, size, max_value);
    }
    #endif

    #ifdef __ALTIVEC__
    else if(simd_mode == SIMD_Mode::ALTIVEC){
        max_value = find_maximum_altivec(buff, 0, window_size, altivec_array);
        return range_scan_geq_altivec(buff, window_size, size, max_value);
    }
    #endif
    
    else {
        std::cout << "Error: Unsupported SIMD mode" << std::endl;
        exit(EXIT_FAILURE);
    }

    return size;
}

