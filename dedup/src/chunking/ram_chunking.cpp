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
    chunk_counter = 0;

	byteroll_mode = HASHLESS_BYTEROLL_MODE::BYTE_ROLL;
	roll_size = 1; // Byte roll by default

    window_size = avg_block_size - 256;
    
    #ifdef __SSE3__
    sse_array = nullptr;
    #endif
    
    #ifdef __AVX2__
    avx256_array = nullptr;
    #endif
    
    #if defined(__AVX512F__)
        avx512_array = nullptr;
        avx512_arrays_nonbyteroll = nullptr;
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
    chunk_counter = 0;

    byteroll_mode = config.get_hashless_byteroll_mode();

    if(byteroll_mode == HASHLESS_BYTEROLL_MODE::BYTE_ROLL){
        roll_size = 1;
    }
    else if(byteroll_mode == HASHLESS_BYTEROLL_MODE::WORD_ROLL){
        roll_size = WORD_ROLL_SIZE_BYTES;
    }
    else if(byteroll_mode == HASHLESS_BYTEROLL_MODE::DWORD_ROLL){
        roll_size = DWORD_ROLL_SIZE_BYTES;
    }
    else if(byteroll_mode == HASHLESS_BYTEROLL_MODE::QWORD_ROLL){
        roll_size = QWORD_ROLL_SIZE_BYTES;
    }
    else{
        std::cerr << "Error: Unsupported byteroll mode for RAM chunking" << std::endl;
        exit(1);
    }

    window_size = avg_block_size;
    // window_size = avg_block_size / (exp(1) - 1);  // avg_block size / e-1

    #ifdef __SSE3__
        sse_array = nullptr;
    #endif
    
    #ifdef __AVX2__
        avx256_array = nullptr;
    #endif
    
    #if defined(__AVX512F__)
        avx512_array = nullptr;
        avx512_arrays_nonbyteroll = nullptr;
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
        if(byteroll_mode != HASHLESS_BYTEROLL_MODE::BYTE_ROLL){
            avx512_arrays_nonbyteroll = new __m512i*[roll_size];
            for(uint8_t i = 0; i < roll_size; i++){
                avx512_arrays_nonbyteroll[i] = new __m512i[num_vectors]();
            }
        }
        else
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
    else if(simd_mode == SIMD_Mode::AVX512){
        if(byteroll_mode == HASHLESS_BYTEROLL_MODE::BYTE_ROLL){
            delete[] avx512_array;
        }
        else if(avx512_arrays_nonbyteroll != nullptr){
            for(uint8_t i = 0; i < roll_size; i++)
                delete[] avx512_arrays_nonbyteroll[i];

            delete[] avx512_arrays_nonbyteroll;
        }
    }
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

template <typename t>
uint64_t RAM_Chunking::find_cutpoint_native_nonbyteroll(char* buff, uint64_t size) {
    uint64_t i = 0;
    t max_value = *(reinterpret_cast<t*>(&buff[i]));
    i++;

    if (size > max_block_size)
        size = max_block_size;
    else if(size < window_size)
        return size;

    for(i = 0; i < window_size; i++){
        t curr_value = *(reinterpret_cast<t*>(&buff[i]));
        if (curr_value >= max_value)
            max_value = curr_value;
    }

    for (i = window_size; i < size; i++) {
        if (*(reinterpret_cast<t*>(&buff[i])) >= max_value){
            return i;
        }
    }

    return size;
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
        if(byteroll_mode == HASHLESS_BYTEROLL_MODE::BYTE_ROLL){
            for(i = 0; i < window_size; i++){
                if ((uint8_t)buff[i] >= max_value)
	    	        max_value = (uint8_t)buff[i];
            }

            for (i = window_size; i < size; i++) {
                if ((uint8_t)buff[i] >= max_value)
                    return i;
            }
        }
        else if(byteroll_mode == HASHLESS_BYTEROLL_MODE::WORD_ROLL){
            return find_cutpoint_native_nonbyteroll<uint16_t>(buff, size);
        }
        else if(byteroll_mode == HASHLESS_BYTEROLL_MODE::DWORD_ROLL){
            return find_cutpoint_native_nonbyteroll<uint32_t>(buff, size);
        }
        else if(byteroll_mode == HASHLESS_BYTEROLL_MODE::QWORD_ROLL){
            return find_cutpoint_native_nonbyteroll<uint64_t>(buff, size);
        }
        else{
            std::cerr << "Error: Unsupported byteroll mode" << std::endl;
            exit(1);
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
        if(byteroll_mode == HASHLESS_BYTEROLL_MODE::BYTE_ROLL){
            max_value = find_maximum_avx512_accumulator(buff, 0, window_size);
            return range_scan_geq_avx512(buff, window_size, size, max_value);
        }
        else if(byteroll_mode == HASHLESS_BYTEROLL_MODE::WORD_ROLL){
            uint16_t max_value_2byte = find_maximum_avx512_2byteroll_accumulator(buff, 0, window_size);
            return range_scan_geq_avx512_naive_2byteroll(buff, window_size, size, max_value_2byte);
        }
        else if(byteroll_mode == HASHLESS_BYTEROLL_MODE::DWORD_ROLL){
            uint32_t max_value_4byte = find_maximum_avx512_4byteroll_accumulator(buff, 0, window_size);
            return range_scan_geq_avx512_4byteroll(buff, window_size, size, max_value_4byte);
        }
        else if(byteroll_mode == HASHLESS_BYTEROLL_MODE::QWORD_ROLL){
            uint64_t max_value_8byte = find_maximum_avx512_8byteroll_accumulator(buff, 0, window_size);
            return range_scan_geq_avx512_8byteroll(buff, window_size, size, max_value_8byte);
        }
        else{
            std::cerr << "Error: Unsupported byteroll mode for AVX-512 RAM chunking." << std::endl;
            exit(1);
        }
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

