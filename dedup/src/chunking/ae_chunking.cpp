/**
 * @file ae_chunking.cpp
 * @author WASL
 * @brief Implementations for AE chunking technique
 * @version 0.1
 * @date 2023-3-10
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <fstream>
#include "ae_chunking.hpp"

AE_Chunking::AE_Chunking() {
    avg_block_size = DEFAULT_AE_AVG_BLOCK_SIZE;
    extreme_mode = MAX;
    technique_name = "AE Chunking";

	chunk_counter = 0;

	#if defined(__SSE3__)
	sse_array = nullptr;
	#endif
	
	#if defined(__AVX2__)
	avx256_array = nullptr;
	#endif
	
	#ifdef __AVX512F__
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

AE_Chunking::AE_Chunking(const Config& config) {
    extreme_mode = config.get_ae_extreme_mode();
    avg_block_size = config.get_ae_avg_block_size();
    window_size = avg_block_size - 256;
	// window_size = avg_block_size / (exp(1) - 1);  // avg_block size / e-1


	// Initialize SIMD arrays based on the chosen SIMD mode

	#if defined(__SSE3__)
	sse_array = nullptr;
	#endif
	
	#if defined(__AVX2__)
	avx256_array = nullptr;
	#endif
	
	#ifdef __AVX512F__
	avx512_array = nullptr;
	#endif

	#ifdef __ARM_NEON
	neon_array = nullptr;
	#endif

	#ifdef __ALTIVEC__
	altivec_array = nullptr;
	#endif

	simd_mode = config.get_simd_mode();
	if (simd_mode == SIMD_Mode::NONE) {

	} 
	#if defined(__SSE3__)
	else if (simd_mode == SIMD_Mode::SSE128) {
		if(window_size % SSE_REGISTER_SIZE_BYTES != 0 || (window_size / SSE_REGISTER_SIZE_BYTES) % 2 != 0) {
            // Check if window size is a multiple of 16
            // Check if window size makes an even number of vectors for find_maximum_sse128
            std::cout << "AE window size currently unsupported by SSE128. Please use an even multiple of SSE128_REGISTER_SIZE_BYTES (default 16)." << std::endl;
            exit(1);
        }
		sse_array = new __m128i[avg_block_size / SSE_REGISTER_SIZE_BYTES];
		if (sse_array == nullptr) {
			std::cout << "Error allocating memory for 128-bit vectors(__m128i)" << std::endl;
			exit(1);
		}
	} 
	#endif

	#if defined(__AVX2__)
	else if (simd_mode == SIMD_Mode::AVX256) {
		if(window_size % AVX256_REGISTER_SIZE_BYTES != 0 || (window_size / AVX256_REGISTER_SIZE_BYTES % 2 != 0)) {
			// Check if window size is a multiple of 32
			// Check if window size makes an even number of vectors for find_maximum_AVX256
			std::cout << "AE window size currently unsupported by AVX256. Please use an even multiple of AVX_REGISTER_SIZE_BYTES (default 32)." << std::endl;
			exit(1);
		}
		avx256_array = new __m256i[avg_block_size / AVX256_REGISTER_SIZE_BYTES];
		if (avx256_array == nullptr) {
			std::cout << "Error allocating memory for 256-bit vectors(__m256i)" << std::endl;
			exit(1);
		}
	}
	#endif

	#ifdef __AVX512F__
	else if (simd_mode == SIMD_Mode::AVX512) {
		if(window_size % AVX512_REGISTER_SIZE_BYTES != 0 || (window_size / AVX512_REGISTER_SIZE_BYTES) % 2 != 0) {
			// Check if window size is a multiple of 64
			// Check if window size makes an even number of vectors for find_maximum_AVX512
			std::cout << "AE window size currently unsupported by AVX512. Please use an even multiple of AVX_REGISTER_SIZE_BYTES (default 64)." << std::endl;
			exit(1);
		}
		avx512_array = new __m512i[avg_block_size / AVX512_REGISTER_SIZE_BYTES];
		if (avx512_array == nullptr) {
			std::cout << "Error allocating memory for 512-bit vectors(__m512i)" << std::endl;
			exit(1);
		}
	}
	#endif

	#ifdef __ARM_NEON
	else if (simd_mode == SIMD_Mode::NEON) {
		if(window_size % NEON_REGISTER_SIZE_BYTES != 0 || (window_size / NEON_REGISTER_SIZE_BYTES) % 2 != 0) {
			// Check if window size is a multiple of 16
			// Check if window size makes an even number of vectors for find_maximum_neon
			std::cout << "AE window size currently unsupported by NEON. Please use an even multiple of NEON_REGISTER_SIZE_BYTES (default 16)." << std::endl;
			exit(1);
		}
		neon_array = new uint8x16_t[avg_block_size / NEON_REGISTER_SIZE_BYTES];
		if (neon_array == nullptr) {
			std::cout << "Error allocating memory for NEON vectors(uint8x16_t)" << std::endl;
			exit(1);
		}
	}
	#endif

	#ifdef __ALTIVEC__
	else if (simd_mode == SIMD_Mode::ALTIVEC) {
		if(window_size % ALTIVEC_REGISTER_SIZE_BYTES != 0 || (window_size / ALTIVEC_REGISTER_SIZE_BYTES) % 2 != 0) {
			// Check if window size is a multiple of 16
			// Check if window size makes an even number of vectors for find_maximum_altivec
			std::cout << "AE window size currently unsupported by ALTIVEC. Please use an even multiple of ALTIVEC_REGISTER_SIZE_BYTES (default 16)." << std::endl;
			exit(1);
		}
		altivec_array = new __vector unsigned char[avg_block_size / ALTIVEC_REGISTER_SIZE_BYTES];
		if (altivec_array == nullptr) {
			std::cout << "Error allocating memory for ALTIVEC vectors(__vector unsigned char)" << std::endl;
			exit(1);
		}
	}
	#endif

	else {
		std::cerr << "Error: Unsupported SIMD mode" << std::endl;
		exit(1);
	}
    
    technique_name = "AE Chunking";
}

AE_Chunking::~AE_Chunking() {
	#ifdef __SSE3__
	if (sse_array != nullptr) {
		delete[] sse_array;
	}
	#endif

	#ifdef __AVX2__
	if (avx256_array != nullptr) {
		delete[] avx256_array;
	}
	#endif

	#ifdef __AVX512F__
	if (avx512_array != nullptr) {
		delete[] avx512_array;
	}
	#endif

	#ifdef __ARM_NEON
	if (neon_array != nullptr) {
		delete[] neon_array;
	}
	#endif

	#ifdef __ALTIVEC__
	if (altivec_array != nullptr) {
		delete[] altivec_array;	
	}
	#endif

}

uint64_t AE_Chunking::find_cutpoint(char *buff, uint64_t size){

	// chunk_counter++;

	if(simd_mode == SIMD_Mode::NONE){
		return find_cutpoint_native(buff, size);
	}
	
	#ifdef __SSE3__
	else if(simd_mode == SIMD_Mode::SSE128){
		return find_cutpoint_sse128(buff, size);
	}
	#endif
	
	#ifdef __AVX2__
	else if(simd_mode == SIMD_Mode::AVX256){
		return find_cutpoint_avx256(buff, size);
	}
	#endif

	#ifdef __AVX512F__
	else if(simd_mode == SIMD_Mode::AVX512){
		return find_cutpoint_avx512(buff, size);
	}
	#endif

	#ifdef __ARM_NEON
	else if(simd_mode == SIMD_Mode::NEON){
		return find_cutpoint_neon(buff, size);
	}
	#endif

	#ifdef __ALTIVEC__
	else if(simd_mode == SIMD_Mode::ALTIVEC){
		return find_cutpoint_altivec(buff, size);
	}
	#endif

	else{
		std::cerr << "Error: Unsupported SIMD mode" << std::endl;
		return 0;
	}
}

#ifdef __SSE3__
uint64_t AE_Chunking::find_cutpoint_sse128(char* buff, uint64_t size) {
	uint64_t target_pos = 0;
	uint64_t return_pos_range_scan;
	uint8_t find_max_result;

	uint8_t target_value = (uint8_t) buff[target_pos];
	if(extreme_mode == AE_Mode::MAX){
		while(target_pos < (size - window_size - 1)){

			return_pos_range_scan = range_scan_gt_sse128(buff, target_pos + 1, target_pos + 1 + window_size, target_value);
			if(return_pos_range_scan != target_pos + 1 + window_size){
				target_pos = return_pos_range_scan;
				target_value = (uint8_t) buff[target_pos];

				find_max_result = find_maximum_sse128(buff, target_pos + 1, target_pos + 1 + window_size, sse_array);

				if(find_max_result < target_value)
					return std::min(size, target_pos + window_size);
			}
			else {
				return std::min(size, target_pos + window_size);
			}
		}

		return size;
	}
	else if(extreme_mode == AE_Mode::MIN){
			uint64_t target_pos = 0;
			uint64_t return_pos_range_scan;
			uint8_t find_min_result;
			uint8_t target_value = (uint8_t) buff[target_pos];

			while(target_pos < (size - window_size - 1)){
				return_pos_range_scan = range_scan_lt_sse128(buff, target_pos + 1, target_pos + 1 + window_size, target_value);
				if(return_pos_range_scan != target_pos + 1 + window_size){
					target_pos = return_pos_range_scan;
					target_value = (uint8_t) buff[target_pos];

					find_min_result = find_minimum_sse128(buff, target_pos + 1, target_pos + 1 + window_size, sse_array);

					if(find_min_result > target_value)
						return std::min(size, target_pos + window_size);
				}
				else {
					return std::min(size, target_pos + window_size);
				}
			}
	}
	
	return size;
}
#endif

#ifdef __AVX2__
uint64_t AE_Chunking::find_cutpoint_avx256(char* buff, uint64_t size) {
	uint64_t target_pos = 0;
	uint64_t return_pos_range_scan;
	uint8_t find_max_result;

	uint8_t target_value = (uint8_t) buff[target_pos];
	if(extreme_mode == AE_Mode::MAX){
		while(target_pos < (size - window_size - 1)){

			return_pos_range_scan = range_scan_gt_avx256(buff, target_pos + 1, target_pos + 1 + window_size, target_value);
			if(return_pos_range_scan != target_pos + 1 + window_size){
				target_pos = return_pos_range_scan;
				target_value = (uint8_t) buff[target_pos];

				find_max_result = find_maximum_avx256(buff, target_pos + 1, target_pos + 1 + window_size, avx256_array);

				if(find_max_result < target_value)
					return std::min(size, target_pos + window_size);
			}
			else {
				return std::min(size, target_pos + window_size);
			}
		}

		return size;
	}
	else if(extreme_mode == AE_Mode::MIN){
			uint64_t target_pos = 0;
			uint64_t return_pos_range_scan;
			uint8_t find_min_result;
			uint8_t target_value = (uint8_t) buff[target_pos];

			while(target_pos < (size - window_size - 1)){
				return_pos_range_scan = range_scan_lt_avx256(buff, target_pos + 1, target_pos + 1 + window_size, target_value);
				if(return_pos_range_scan != target_pos + 1 + window_size){
					target_pos = return_pos_range_scan;
					target_value = (uint8_t) buff[target_pos];

					find_min_result = find_minimum_avx256(buff, target_pos + 1, target_pos + 1 + window_size, avx256_array);

					if(find_min_result > target_value)
						return std::min(size, target_pos + window_size);
				}
				else {
					return std::min(size, target_pos + window_size);
				}
			}
	}
	
	return size;
}
#endif

#if defined(__AVX512F__)
uint64_t AE_Chunking::find_cutpoint_avx512(char* buff, uint64_t size) {
	uint64_t target_pos = 0;
	uint64_t return_pos_range_scan;
	uint8_t find_max_result;

	uint8_t target_value = (uint8_t) buff[target_pos];
	if(extreme_mode == AE_Mode::MAX){
		while(target_pos < (size - window_size - 1)){

			return_pos_range_scan = range_scan_gt_avx512(buff, target_pos + 1, target_pos + 1 + window_size, target_value);
			if(return_pos_range_scan != target_pos + 1 + window_size){
				target_pos = return_pos_range_scan;
				target_value = (uint8_t) buff[target_pos];

				find_max_result = find_maximum_avx512(buff, target_pos + 1, target_pos + 1 + window_size, avx512_array);

				if(find_max_result < target_value)
					return std::min(size, target_pos + window_size);
			}
			else {
				return std::min(size, target_pos + window_size);
			}
		}

		return size;
	}
	else if(extreme_mode == AE_Mode::MIN){
			uint64_t target_pos = 0;
			uint64_t return_pos_range_scan;
			uint8_t find_min_result;
			uint8_t target_value = (uint8_t) buff[target_pos];

			while(target_pos < (size - window_size - 1)){
				return_pos_range_scan = range_scan_lt_avx512(buff, target_pos + 1, target_pos + 1 + window_size, target_value);
				if(return_pos_range_scan != target_pos + 1 + window_size){
					target_pos = return_pos_range_scan;
					target_value = (uint8_t) buff[target_pos];

					find_min_result = find_minimum_avx512(buff, target_pos + 1, target_pos + 1 + window_size, avx512_array);

					if(find_min_result > target_value)
						return std::min(size, target_pos + window_size);
				}
				else {
					return std::min(size, target_pos + window_size);
				}
			}
	}
	
	return size;
}
#endif

#ifdef __ARM_NEON
uint64_t AE_Chunking::find_cutpoint_neon(char* buff, uint64_t size) {
	uint64_t target_pos = 0;
	uint64_t return_pos_range_scan;
	uint8_t find_max_result;

	uint8_t target_value = (uint8_t) buff[target_pos];
	if(extreme_mode == AE_Mode::MAX){
		while(target_pos < (size - window_size - 1)){

			return_pos_range_scan = range_scan_gt_neon(buff, target_pos + 1, target_pos + 1 + window_size, target_value);
			if(return_pos_range_scan != target_pos + 1 + window_size){
				target_pos = return_pos_range_scan;
				target_value = (uint8_t) buff[target_pos];

				find_max_result = find_maximum_neon(buff, target_pos + 1, target_pos + 1 + window_size, neon_array);

				if(find_max_result < target_value)
					{
						return std::min(size, target_pos + window_size);
					}
			}
			else {
				return std::min(size, target_pos + window_size);
			}
		}

		return size;
	}
	else if(extreme_mode == AE_Mode::MIN){
			uint64_t target_pos = 0;
			uint64_t return_pos_range_scan;
			uint8_t find_min_result;
			uint8_t target_value = (uint8_t) buff[target_pos];

			while(target_pos < (size - window_size - 1)){
				return_pos_range_scan = range_scan_lt_neon(buff, target_pos + 1, target_pos + 1 + window_size, target_value);
				if(return_pos_range_scan != target_pos + 1 + window_size){
					target_pos = return_pos_range_scan;
					target_value = (uint8_t) buff[target_pos];

					find_min_result = find_minimum_neon(buff, target_pos + 1, target_pos + 1 + window_size, neon_array);

					if(find_min_result > target_value)
						return std::min(size, target_pos + window_size);
				}
				else {
					return std::min(size, target_pos + window_size);
				}
			}
	}
	
	return size;
}
#endif

#ifdef __ALTIVEC__
uint64_t AE_Chunking::find_cutpoint_altivec(char* buff, uint64_t size) {
	uint64_t target_pos = 0;
	uint64_t return_pos_range_scan;
	uint8_t find_max_result;

	uint8_t target_value = (uint8_t) buff[target_pos];
	if(extreme_mode == AE_Mode::MAX){
		while(target_pos < (size - window_size - 1)){

			return_pos_range_scan = range_scan_gt_altivec(buff, target_pos + 1, target_pos + 1 + window_size, target_value);
			if(return_pos_range_scan != target_pos + 1 + window_size){
				target_pos = return_pos_range_scan;
				target_value = (uint8_t) buff[target_pos];

				find_max_result = find_maximum_altivec(buff, target_pos + 1, target_pos + 1 + window_size, altivec_array);

				if(find_max_result < target_value)
				{
					return std::min(size, target_pos + window_size);
				}
			}
			else {
				return std::min(size, target_pos + window_size);
			}
		}

		return size;
	}
	else if(extreme_mode == AE_Mode::MIN){
			uint64_t target_pos = 0;
			uint64_t return_pos_range_scan;
			uint8_t find_min_result;
			uint8_t target_value = (uint8_t) buff[target_pos];

			while(target_pos < (size - window_size - 1)){
				return_pos_range_scan = range_scan_lt_altivec(buff, target_pos + 1, target_pos + 1 + window_size, target_value);
				if(return_pos_range_scan != target_pos + 1 + window_size){
					target_pos = return_pos_range_scan;
					target_value = (uint8_t) buff[target_pos];

					find_min_result = find_minimum_altivec(buff, target_pos + 1, target_pos + 1 + window_size, altivec_array);

					if(find_min_result > target_value)
						return std::min(size, target_pos + window_size);
				}
				else {
					return std::min(size, target_pos + window_size);
				}
			}
	}
	
	return size;
}
#endif

uint64_t AE_Chunking::find_cutpoint_native(char* buff, uint64_t size) {
    uint32_t i = 0;
    uint64_t max_value = buff[i];
    uint64_t max_pos = i;
    i++;
    if(extreme_mode == MAX){
	    while (i < size) {
        	    if (!((uint64_t)buff[i] > max_value)) {
                	if (i == max_pos + window_size)
	                    return i;
            		} 
	    
	    		else {
		                max_value = buff[i];
		                max_pos = i;
		            }
	            i++;
        	}
	}
    else if(extreme_mode == MIN){
    		while (i < size) {
        	    if (!((uint64_t)buff[i] < max_value)) {
                	if (i == max_pos + window_size)
	                    return i;
            		} 
	    
	    		else {
		                max_value = buff[i];
		                max_pos = i;
		            }
	            i++;
        	}

    }
    
    return size;
}

