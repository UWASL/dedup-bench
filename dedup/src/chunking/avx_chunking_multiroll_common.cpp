/**
 * @file avx_chunking_multiroll_common.cpp
 * @author WASL
 * @brief Implementations of multi-roll functions common across all AVX chunking techniques
 * @version 0.1
 * @date 2025-12-01
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "avx_chunking_multiroll_common.hpp"

/*
    ======================================================================
    AVX-512 find_maximum implementations for multiroll.
        This section has both naive and accumulator-based implementations for 2-byte and 4-byte rolls.
        Other Extreme Byte Searches (such as find_minimum) only have the final versions.
    ======================================================================
*/


#ifdef __AVX512F__
uint16_t MultiRoll_AVX_Chunking_Technique::find_maximum_avx512_naive_2byteroll(char *buff, uint64_t start_pos, uint64_t end_pos, __m512i **zmm_arrays){
    // Assume window_size is a multiple of AVX512_REGISTER_SIZE_BYTES for now
    // Assume num_vectors is even for now - True for most common window sizes. Can fix later via specific check

    __mmask32 cmp_mask = UINT32_MAX;

    uint64_t num_vectors = (end_pos - start_pos) / AVX512_REGISTER_SIZE_BYTES;

    uint64_t step = 2;
    uint64_t half_step = 1;

    // Load contents into __m512i structures
    // Could be optimized later as only 16 xmm registers are available per CPU in 64-bit
    for(uint64_t i = 0; i < num_vectors; i++){
        for(uint8_t j = 0; j < WORD_ROLL_SIZE_BYTES; j++){
            zmm_arrays[j][i] = _mm512_loadu_si512((__m512i const *)(buff + start_pos + (AVX512_REGISTER_SIZE_BYTES * i) + j)); // Offset of j to consider non-overlapping words
        }
    }
    
    // Repeat vmaxu until a single register is remaining with maximum values
    // Each iteration calculates maximums between a pair of registers and moves it into the first register in the pair
    // Finally, only one will be left with the maximum values from all pairs
    while(step <= num_vectors){
        
        for(uint64_t i = 0; i < num_vectors; i+=step){
            for(uint8_t j = 0; j < WORD_ROLL_SIZE_BYTES; j++){
                zmm_arrays[j][i] = _mm512_maskz_max_epu16(cmp_mask, zmm_arrays[j][i], zmm_arrays[j][i+half_step]);
            }
        }
        
        // Multiply step by 2
        half_step = step;
        step = step << 1;
    }

    //Run vmaxu on the remaining registers to get the final maximums
    for(uint8_t j = 1; j < WORD_ROLL_SIZE_BYTES; j++)
        zmm_arrays[0][0] = _mm512_maskz_max_epu16(cmp_mask, zmm_arrays[0][0], zmm_arrays[j][0]);


    // Move the final set of values from the xmm into local memory    
    uint16_t result_store[AVX512_REGISTER_SIZE_BYTES / 2] = {0};

    _mm512_storeu_epi16(&result_store, zmm_arrays[0][0]);

    // Sequentially scan the last remaining bytes (512 in this case) to find the max value
    uint16_t max_val = 0;
    for(uint64_t i = 0; i < (AVX512_REGISTER_SIZE_BYTES / 2); i++){
        if(result_store[i] > max_val)
            max_val = result_store[i];
    }

    // Return maximum value
    return max_val;
}

uint32_t MultiRoll_AVX_Chunking_Technique::find_maximum_avx512_naive_4byteroll(char *buff, uint64_t start_pos, uint64_t end_pos, __m512i **zmm_arrays){
    // Assume window_size is a multiple of AVX512_REGISTER_SIZE_BYTES for now
    // Assume num_vectors is even for now - True for most common window sizes. Can fix later via specific check

    __mmask16 cmp_mask = UINT16_MAX;

    uint64_t num_vectors = (end_pos - start_pos) / AVX512_REGISTER_SIZE_BYTES;

    uint64_t step = 2;
    uint64_t half_step = 1;
    
    // Load contents into __m512i structures
    // Could be optimized later as only 16 xmm registers are available per CPU in 64-bit
    for(uint64_t i = 0; i < num_vectors; i++){
        for(uint8_t j = 0; j < DWORD_ROLL_SIZE_BYTES; j++)
            zmm_arrays[j][i] = _mm512_loadu_si512((__m512i const *)(buff + start_pos + (AVX512_REGISTER_SIZE_BYTES * i) + j)); // Offset of j to consider non-overlapping words
    }       

    // Repeat vmaxu until a single register is remaining with maximum values
    // Each iteration calculates maximums between a pair of registers and moves it into the first register in the pair
    // Finally, only one will be left with the maximum values from all pairs
    while(step <= num_vectors){
        
        for(uint64_t i = 0; i < num_vectors; i+=step){
            for(uint8_t j = 0; j < DWORD_ROLL_SIZE_BYTES; j++)
                zmm_arrays[j][i] = _mm512_maskz_max_epu32(cmp_mask, zmm_arrays[j][i], zmm_arrays[j][i+half_step]);
        }
        
        // Multiply step by 2
        half_step = step;
        step = step << 1;
    }
    
    //Run vmaxu on all the remaining registers to get the final maximums

    for(uint8_t j = 1; j < DWORD_ROLL_SIZE_BYTES; j++)
        zmm_arrays[0][0] = _mm512_maskz_max_epu32(cmp_mask, zmm_arrays[0][0], zmm_arrays[j][0]);
    
    // Move the final set of values from the xmm into local memory    
    uint32_t result_store[AVX512_REGISTER_SIZE_BYTES / 4] = {0};
    _mm512_storeu_epi32(&result_store, zmm_arrays[0][0]);
    
    // Sequentially scan the last remaining bytes (512 in this case) to find the max value
    uint32_t max_val = 0;
    for(uint64_t i = 0; i < (AVX512_REGISTER_SIZE_BYTES / 4); i++){
        if(result_store[i] > max_val)
            max_val = result_store[i]; 
    }
    
    // Return maximum value
    return max_val;
}

uint64_t MultiRoll_AVX_Chunking_Technique::find_maximum_avx512_naive_8byteroll(char *buff, uint64_t start_pos, uint64_t end_pos, __m512i **zmm_arrays){
    // Assume window_size is a multiple of AVX512_REGISTER_SIZE_BYTES for now
    // Assume num_vectors is even for now - True for most common window sizes. Can fix later via specific check

    __mmask8 cmp_mask = UINT8_MAX;

    uint64_t num_vectors = (end_pos - start_pos) / AVX512_REGISTER_SIZE_BYTES;

    uint64_t step = 2;
    uint64_t half_step = 1;

    // Load contents into __m512i structures
    // Could be optimized later as only 16 xmm registers are available per CPU in 64-bit
    for(uint64_t i = 0; i < num_vectors; i++){
        for(uint8_t j = 0; j < QWORD_ROLL_SIZE_BYTES; j++)
            zmm_arrays[j][i] = _mm512_loadu_si512((__m512i const *)(buff + start_pos + (AVX512_REGISTER_SIZE_BYTES * i) + j)); // Offset of j to consider non-overlapping words
    }       

    // Repeat vmaxu until a single register is remaining with maximum values
    // Each iteration calculates maximums between a pair of registers and moves it into the first register in the pair
    // Finally, only one will be left with the maximum values from all pairs
    while(step <= num_vectors){
        
        for(uint64_t i = 0; i < num_vectors; i+=step){
            for(uint8_t j = 0; j < QWORD_ROLL_SIZE_BYTES; j++)
                zmm_arrays[j][i] = _mm512_maskz_max_epu64(cmp_mask, zmm_arrays[j][i], zmm_arrays[j][i+half_step]);
        }
        
        // Multiply step by 2
        half_step = step;
        step = step << 1;
    }
    
    //Run vmaxu on all the remaining registers to get the final maximums

    for(uint8_t j = 1; j < QWORD_ROLL_SIZE_BYTES; j++)
        zmm_arrays[0][0] = _mm512_maskz_max_epu64(cmp_mask, zmm_arrays[0][0], zmm_arrays[j][0]);

    // Move the final set of values from the xmm into local memory    
    uint64_t result_store[AVX512_REGISTER_SIZE_BYTES / 8] = {0};
    _mm512_storeu_epi64(&result_store, zmm_arrays[0][0]);

    // Sequentially scan the last remaining bytes (512 in this case) to find the max value
    uint64_t max_val = 0;
    for(uint64_t i = 0; i < (AVX512_REGISTER_SIZE_BYTES / 8); i++){
        if(result_store[i] > max_val)
            max_val = result_store[i]; 
    }
 
    // Return maximum value
    return max_val;
}

uint8_t MultiRoll_AVX_Chunking_Technique::find_maximum_avx512_accumulator(char *buff, uint64_t start_pos, uint64_t end_pos){
    // This function uses a rolling accumulator instead of storing all data first
    __mmask64 cmp_mask = UINT64_MAX;

    uint64_t num_vectors = (end_pos - start_pos) / AVX512_REGISTER_SIZE_BYTES;

    // Initialize accumulator register to zero (minimum value for unsigned)
    __m512i acc = _mm512_setzero_si512();

    __m512i zmm_data;

    // Process data in chunks, updating accumulator as we go
    for(uint64_t i = 0; i < num_vectors; i++){
        // Load data
        zmm_data = _mm512_loadu_si512((__m512i const *)(buff + start_pos + (AVX512_REGISTER_SIZE_BYTES * i)));
        
        // Update accumulator with maximum values
        acc = _mm512_maskz_max_epu8(cmp_mask, acc, zmm_data);
    }

    // Move the final set of values from the zmm register into local memory    
    uint8_t result_store[AVX512_REGISTER_SIZE_BYTES] = {0};
    _mm512_storeu_epi8(&result_store, acc);

    // Sequentially scan the remaining values to find the max value
    uint8_t max_val = 0;
    for(uint64_t i = 0; i < AVX512_REGISTER_SIZE_BYTES; i++){
        if(result_store[i] > max_val)
            max_val = result_store[i];
    }

    // Return maximum value
    return max_val;
}

uint16_t MultiRoll_AVX_Chunking_Technique::find_maximum_avx512_2byteroll_accumulator(char *buff, uint64_t start_pos, uint64_t end_pos){
    // This function uses rolling accumulators instead of storing all data first
    // Two loads at offset 0 and 1, maintaining two accumulator registers
    
    __mmask32 cmp_mask = UINT32_MAX;

    uint64_t num_vectors = (end_pos - start_pos) / AVX512_REGISTER_SIZE_BYTES;

    // Initialize accumulator registers to zero (minimum value for unsigned)
    __m512i acc_offset0 = _mm512_setzero_si512();
    __m512i acc_offset1 = _mm512_setzero_si512();

    __m512i zmm_data0, zmm_data1;

    // Process data in chunks, updating accumulators as we go
    for(uint64_t i = 0; i < num_vectors; i++){
        // Load data at offset 0
        zmm_data0 = _mm512_loadu_si512((__m512i const *)(buff + start_pos + (AVX512_REGISTER_SIZE_BYTES * i)));
        
        // Load data at offset 1 (non-overlapping words)
        zmm_data1 = _mm512_loadu_si512((__m512i const *)(buff + start_pos + (AVX512_REGISTER_SIZE_BYTES * i) + 1));
        
        // Update accumulators with maximum values
        acc_offset0 = _mm512_maskz_max_epu16(cmp_mask, acc_offset0, zmm_data0);
        acc_offset1 = _mm512_maskz_max_epu16(cmp_mask, acc_offset1, zmm_data1);

    }

    // Combine the two accumulators to find the overall maximum
    __m512i final_acc = _mm512_maskz_max_epu16(cmp_mask, acc_offset0, acc_offset1);

    // Move the final set of values from the zmm register into local memory    
    uint16_t result_store[AVX512_REGISTER_SIZE_BYTES / 2] = {0};
    _mm512_storeu_epi16(&result_store, final_acc);

    // Sequentially scan the remaining values to find the max value
    uint16_t max_val = 0;
    for(uint64_t i = 0; i < (AVX512_REGISTER_SIZE_BYTES / 2); i++){
        if(result_store[i] > max_val)
            max_val = result_store[i];
    }

    // Return maximum value
    return max_val;
}

uint32_t MultiRoll_AVX_Chunking_Technique::find_maximum_avx512_4byteroll_accumulator(char *buff, uint64_t start_pos, uint64_t end_pos){
    // This function uses rolling accumulators instead of storing all data first
    // Four loads at offset 0, 1, 2, and 3, maintaining four accumulator registers
    
    __mmask16 cmp_mask = UINT16_MAX;

    uint64_t num_vectors = (end_pos - start_pos) / AVX512_REGISTER_SIZE_BYTES;

    // Initialize accumulator registers to zero (minimum value for unsigned)
    __m512i acc_offset0 = _mm512_setzero_si512();
    __m512i acc_offset1 = _mm512_setzero_si512();
    __m512i acc_offset2 = _mm512_setzero_si512();
    __m512i acc_offset3 = _mm512_setzero_si512();

    __m512i zmm_data0, zmm_data1, zmm_data2, zmm_data3;

    // Process data in chunks, updating accumulators as we go
    for(uint64_t i = 0; i < num_vectors; i++){
        // Load data at offset 0
        zmm_data0 = _mm512_loadu_si512((__m512i const *)(buff + start_pos + (AVX512_REGISTER_SIZE_BYTES * i)));
        
        // Load data at offset 1 (non-overlapping dwords)
        zmm_data1 = _mm512_loadu_si512((__m512i const *)(buff + start_pos + (AVX512_REGISTER_SIZE_BYTES * i) + 1));
        
        // Load data at offset 2 (non-overlapping dwords)
        zmm_data2 = _mm512_loadu_si512((__m512i const *)(buff + start_pos + (AVX512_REGISTER_SIZE_BYTES * i) + 2));
        
        // Load data at offset 3 (non-overlapping dwords)
        zmm_data3 = _mm512_loadu_si512((__m512i const *)(buff + start_pos + (AVX512_REGISTER_SIZE_BYTES * i) + 3));
        
        // Update accumulators with maximum values
        acc_offset0 = _mm512_maskz_max_epu32(cmp_mask, acc_offset0, zmm_data0);
        acc_offset1 = _mm512_maskz_max_epu32(cmp_mask, acc_offset1, zmm_data1);
        acc_offset2 = _mm512_maskz_max_epu32(cmp_mask, acc_offset2, zmm_data2);
        acc_offset3 = _mm512_maskz_max_epu32(cmp_mask, acc_offset3, zmm_data3);
    }

    // Combine the four accumulators to find the overall maximum
    __m512i final_acc = _mm512_maskz_max_epu32(cmp_mask, acc_offset0, acc_offset1);
    final_acc = _mm512_maskz_max_epu32(cmp_mask, final_acc, acc_offset2);
    final_acc = _mm512_maskz_max_epu32(cmp_mask, final_acc, acc_offset3);

    // Move the final set of values from the zmm register into local memory    
    uint32_t result_store[AVX512_REGISTER_SIZE_BYTES / 4] = {0};
    _mm512_storeu_epi32(&result_store, final_acc);

    // Sequentially scan the remaining values to find the max value
    uint32_t max_val = 0;
    for(uint64_t i = 0; i < (AVX512_REGISTER_SIZE_BYTES / 4); i++){
        if(result_store[i] > max_val)
            max_val = result_store[i];
    }

    // Return maximum value
    return max_val;
}

uint64_t MultiRoll_AVX_Chunking_Technique::find_maximum_avx512_8byteroll_accumulator(char *buff, uint64_t start_pos, uint64_t end_pos){
    // This function uses rolling accumulators instead of storing all data first
    // Eight loads at offset 0-7, maintaining eight accumulator registers
    
    __mmask8 cmp_mask = UINT8_MAX;

    uint64_t num_vectors = (end_pos - start_pos) / AVX512_REGISTER_SIZE_BYTES;

    // Initialize accumulator registers to zero (minimum value for unsigned)
    __m512i acc_offset0 = _mm512_setzero_si512();
    __m512i acc_offset1 = _mm512_setzero_si512();
    __m512i acc_offset2 = _mm512_setzero_si512();
    __m512i acc_offset3 = _mm512_setzero_si512();
    __m512i acc_offset4 = _mm512_setzero_si512();
    __m512i acc_offset5 = _mm512_setzero_si512();
    __m512i acc_offset6 = _mm512_setzero_si512();
    __m512i acc_offset7 = _mm512_setzero_si512();

    __m512i zmm_data0, zmm_data1, zmm_data2, zmm_data3, zmm_data4, zmm_data5, zmm_data6, zmm_data7;

    // Process data in chunks, updating accumulators as we go
    for(uint64_t i = 0; i < num_vectors; i++){
        // Load data at offset 0
        zmm_data0 = _mm512_loadu_si512((__m512i const *)(buff + start_pos + (AVX512_REGISTER_SIZE_BYTES * i)));
        
        // Load data at offset 1 (non-overlapping qwords)
        zmm_data1 = _mm512_loadu_si512((__m512i const *)(buff + start_pos + (AVX512_REGISTER_SIZE_BYTES * i) + 1));
        
        // Load data at offset 2 (non-overlapping qwords)
        zmm_data2 = _mm512_loadu_si512((__m512i const *)(buff + start_pos + (AVX512_REGISTER_SIZE_BYTES * i) + 2));
        
        // Load data at offset 3 (non-overlapping qwords)
        zmm_data3 = _mm512_loadu_si512((__m512i const *)(buff + start_pos + (AVX512_REGISTER_SIZE_BYTES * i) + 3));
        
        // Load data at offset 4 (non-overlapping qwords)
        zmm_data4 = _mm512_loadu_si512((__m512i const *)(buff + start_pos + (AVX512_REGISTER_SIZE_BYTES * i) + 4));
        
        // Load data at offset 5 (non-overlapping qwords)
        zmm_data5 = _mm512_loadu_si512((__m512i const *)(buff + start_pos + (AVX512_REGISTER_SIZE_BYTES * i) + 5));
        
        // Load data at offset 6 (non-overlapping qwords)
        zmm_data6 = _mm512_loadu_si512((__m512i const *)(buff + start_pos + (AVX512_REGISTER_SIZE_BYTES * i) + 6));
        
        // Load data at offset 7 (non-overlapping qwords)
        zmm_data7 = _mm512_loadu_si512((__m512i const *)(buff + start_pos + (AVX512_REGISTER_SIZE_BYTES * i) + 7));
        
        // Update accumulators with maximum values
        acc_offset0 = _mm512_maskz_max_epu64(cmp_mask, acc_offset0, zmm_data0);
        acc_offset1 = _mm512_maskz_max_epu64(cmp_mask, acc_offset1, zmm_data1);
        acc_offset2 = _mm512_maskz_max_epu64(cmp_mask, acc_offset2, zmm_data2);
        acc_offset3 = _mm512_maskz_max_epu64(cmp_mask, acc_offset3, zmm_data3);
        acc_offset4 = _mm512_maskz_max_epu64(cmp_mask, acc_offset4, zmm_data4);
        acc_offset5 = _mm512_maskz_max_epu64(cmp_mask, acc_offset5, zmm_data5);
        acc_offset6 = _mm512_maskz_max_epu64(cmp_mask, acc_offset6, zmm_data6);
        acc_offset7 = _mm512_maskz_max_epu64(cmp_mask, acc_offset7, zmm_data7);
    }

    // Combine the eight accumulators to find the overall maximum
    __m512i final_acc = _mm512_maskz_max_epu64(cmp_mask, acc_offset0, acc_offset1);
    final_acc = _mm512_maskz_max_epu64(cmp_mask, final_acc, acc_offset2);
    final_acc = _mm512_maskz_max_epu64(cmp_mask, final_acc, acc_offset3);
    final_acc = _mm512_maskz_max_epu64(cmp_mask, final_acc, acc_offset4);
    final_acc = _mm512_maskz_max_epu64(cmp_mask, final_acc, acc_offset5);
    final_acc = _mm512_maskz_max_epu64(cmp_mask, final_acc, acc_offset6);
    final_acc = _mm512_maskz_max_epu64(cmp_mask, final_acc, acc_offset7);

    // Move the final set of values from the zmm register into local memory    
    uint64_t result_store[AVX512_REGISTER_SIZE_BYTES / 8] = {0};
    _mm512_storeu_epi64(&result_store, final_acc);

    // Sequentially scan the remaining values to find the max value
    uint64_t max_val = 0;
    for(uint64_t i = 0; i < (AVX512_REGISTER_SIZE_BYTES / 8); i++){
        if(result_store[i] > max_val)
            max_val = result_store[i];
    }

    // Return maximum value
    return max_val;
}

#endif

/*
    ======================================================================
    find_minimum implementations for multiroll.
        This section has only the accumulator-based implementations for byte, 2-byte, 4-byte and 8-byte rolls.
    ======================================================================   
*/

#ifdef __AVX512F__

uint8_t MultiRoll_AVX_Chunking_Technique::find_minimum_avx512_accumulator(char *buff, uint64_t start_pos, uint64_t end_pos){
    // This function uses a rolling accumulator instead of storing all data first
    __mmask64 cmp_mask = UINT64_MAX;

    uint64_t num_vectors = (end_pos - start_pos) / AVX512_REGISTER_SIZE_BYTES;

    // Initialize accumulator register to all 1s (maximum value for unsigned)
    __m512i acc = _mm512_set1_epi8(-1);

    __m512i zmm_data;

    // Process data in chunks, updating accumulator as we go
    for(uint64_t i = 0; i < num_vectors; i++){
        // Load data
        zmm_data = _mm512_loadu_si512((__m512i const *)(buff + start_pos + (AVX512_REGISTER_SIZE_BYTES * i)));

        // Update accumulator with minimum values
        acc = _mm512_maskz_min_epu8(cmp_mask, acc, zmm_data);
    }

    // Move the final set of values from the zmm register into local memory
    uint8_t result_store[AVX512_REGISTER_SIZE_BYTES] = {0};
    _mm512_storeu_epi8(&result_store, acc);

    // Sequentially scan the remaining values to find the min value
    uint8_t min_val = UINT8_MAX;
    for(uint64_t i = 0; i < AVX512_REGISTER_SIZE_BYTES; i++){
        if(result_store[i] < min_val)
            min_val = result_store[i];
    }

    // Return minimum value
    return min_val;
}


uint16_t MultiRoll_AVX_Chunking_Technique::find_minimum_avx512_2byteroll_accumulator(char *buff, uint64_t start_pos, uint64_t end_pos){
    // This function uses rolling accumulators instead of storing all data first
    // Two loads at offset 0 and 1, maintaining two accumulator registers
    
    __mmask32 cmp_mask = UINT32_MAX;

    uint64_t num_vectors = (end_pos - start_pos) / AVX512_REGISTER_SIZE_BYTES;

    // Initialize accumulator registers to all 1s (maximum value for unsigned)
    __m512i acc_offset0 = _mm512_set1_epi16(-1);
    __m512i acc_offset1 = _mm512_set1_epi16(-1);

    __m512i zmm_data0, zmm_data1;

    // Process data in chunks, updating accumulators as we go
    for(uint64_t i = 0; i < num_vectors; i++){
        // Load data at offset 0
        zmm_data0 = _mm512_loadu_si512((__m512i const *)(buff + start_pos + (AVX512_REGISTER_SIZE_BYTES * i)));
        
        // Load data at offset 1 (non-overlapping words)
        zmm_data1 = _mm512_loadu_si512((__m512i const *)(buff + start_pos + (AVX512_REGISTER_SIZE_BYTES * i) + 1));
        
        // Update accumulators with minimum values
        acc_offset0 = _mm512_maskz_min_epu16(cmp_mask, acc_offset0, zmm_data0);
        acc_offset1 = _mm512_maskz_min_epu16(cmp_mask, acc_offset1, zmm_data1);

    }

    // Combine the two accumulators to find the overall minimum
    __m512i final_acc = _mm512_maskz_min_epu16(cmp_mask, acc_offset0, acc_offset1);

    // Move the final set of values from the zmm register into local memory    
    uint16_t result_store[AVX512_REGISTER_SIZE_BYTES / 2] = {0};
    _mm512_storeu_epi16(&result_store, final_acc);

    // Sequentially scan the remaining values to find the min value
    uint16_t min_val = UINT16_MAX;
    for(uint64_t i = 0; i < (AVX512_REGISTER_SIZE_BYTES / 2); i++){
        if(result_store[i] < min_val)
            min_val = result_store[i];
    }

    // Return minimum value
    return min_val;
}

uint32_t MultiRoll_AVX_Chunking_Technique::find_minimum_avx512_4byteroll_accumulator(char *buff, uint64_t start_pos, uint64_t end_pos){
    // This function uses rolling accumulators instead of storing all data first
    // Four loads at offset 0, 1, 2, and 3, maintaining four accumulator registers
    
    __mmask16 cmp_mask = UINT16_MAX;

    uint64_t num_vectors = (end_pos - start_pos) / AVX512_REGISTER_SIZE_BYTES;

    // Initialize accumulator registers to all 1s (maximum value for unsigned)
    __m512i acc_offset0 = _mm512_set1_epi32(-1);
    __m512i acc_offset1 = _mm512_set1_epi32(-1);
    __m512i acc_offset2 = _mm512_set1_epi32(-1);
    __m512i acc_offset3 = _mm512_set1_epi32(-1);

    __m512i zmm_data0, zmm_data1, zmm_data2, zmm_data3;

    // Process data in chunks, updating accumulators as we go
    for(uint64_t i = 0; i < num_vectors; i++){
        // Load data at offset 0
        zmm_data0 = _mm512_loadu_si512((__m512i const *)(buff + start_pos + (AVX512_REGISTER_SIZE_BYTES * i)));
        
        // Load data at offset 1 (non-overlapping dwords)
        zmm_data1 = _mm512_loadu_si512((__m512i const *)(buff + start_pos + (AVX512_REGISTER_SIZE_BYTES * i) + 1));
        
        // Load data at offset 2 (non-overlapping dwords)
        zmm_data2 = _mm512_loadu_si512((__m512i const *)(buff + start_pos + (AVX512_REGISTER_SIZE_BYTES * i) + 2));
        
        // Load data at offset 3 (non-overlapping dwords)
        zmm_data3 = _mm512_loadu_si512((__m512i const *)(buff + start_pos + (AVX512_REGISTER_SIZE_BYTES * i) + 3));
        
        // Update accumulators with minimum values
        acc_offset0 = _mm512_maskz_min_epu32(cmp_mask, acc_offset0, zmm_data0);
        acc_offset1 = _mm512_maskz_min_epu32(cmp_mask, acc_offset1, zmm_data1);
        acc_offset2 = _mm512_maskz_min_epu32(cmp_mask, acc_offset2, zmm_data2);
        acc_offset3 = _mm512_maskz_min_epu32(cmp_mask, acc_offset3, zmm_data3);
    }

    // Combine the four accumulators to find the overall minimum
    __m512i final_acc = _mm512_maskz_min_epu32(cmp_mask, acc_offset0, acc_offset1);
    final_acc = _mm512_maskz_min_epu32(cmp_mask, final_acc, acc_offset2);
    final_acc = _mm512_maskz_min_epu32(cmp_mask, final_acc, acc_offset3);

    // Move the final set of values from the zmm register into local memory    
    uint32_t result_store[AVX512_REGISTER_SIZE_BYTES / 4] = {0};
    _mm512_storeu_epi32(&result_store, final_acc);

    // Sequentially scan the remaining values to find the min value
    uint32_t min_val = UINT32_MAX;
    for(uint64_t i = 0; i < (AVX512_REGISTER_SIZE_BYTES / 4); i++){
        if(result_store[i] < min_val)
            min_val = result_store[i];
    }

    // Return minimum value
    return min_val;
}

uint64_t MultiRoll_AVX_Chunking_Technique::find_minimum_avx512_8byteroll_accumulator(char *buff, uint64_t start_pos, uint64_t end_pos){
    // This function uses rolling accumulators instead of storing all data first
    // Eight loads at offset 0-7, maintaining eight accumulator registers
    
    __mmask8 cmp_mask = UINT8_MAX;

    uint64_t num_vectors = (end_pos - start_pos) / AVX512_REGISTER_SIZE_BYTES;

    // Initialize accumulator registers to all 1s (maximum value for unsigned)
    __m512i acc_offset0 = _mm512_set1_epi64(-1);
    __m512i acc_offset1 = _mm512_set1_epi64(-1);
    __m512i acc_offset2 = _mm512_set1_epi64(-1);
    __m512i acc_offset3 = _mm512_set1_epi64(-1);
    __m512i acc_offset4 = _mm512_set1_epi64(-1);
    __m512i acc_offset5 = _mm512_set1_epi64(-1);
    __m512i acc_offset6 = _mm512_set1_epi64(-1);
    __m512i acc_offset7 = _mm512_set1_epi64(-1);

    __m512i zmm_data0, zmm_data1, zmm_data2, zmm_data3, zmm_data4, zmm_data5, zmm_data6, zmm_data7;

    // Process data in chunks, updating accumulators as we go
    for(uint64_t i = 0; i < num_vectors; i++){
        // Load data at offset 0
        zmm_data0 = _mm512_loadu_si512((__m512i const *)(buff + start_pos + (AVX512_REGISTER_SIZE_BYTES * i)));
        
        // Load data at offset 1 (non-overlapping qwords)
        zmm_data1 = _mm512_loadu_si512((__m512i const *)(buff + start_pos + (AVX512_REGISTER_SIZE_BYTES * i) + 1));
        
        // Load data at offset 2 (non-overlapping qwords)
        zmm_data2 = _mm512_loadu_si512((__m512i const *)(buff + start_pos + (AVX512_REGISTER_SIZE_BYTES * i) + 2));
        
        // Load data at offset 3 (non-overlapping qwords)
        zmm_data3 = _mm512_loadu_si512((__m512i const *)(buff + start_pos + (AVX512_REGISTER_SIZE_BYTES * i) + 3));
        
        // Load data at offset 4 (non-overlapping qwords)
        zmm_data4 = _mm512_loadu_si512((__m512i const *)(buff + start_pos + (AVX512_REGISTER_SIZE_BYTES * i) + 4));
        
        // Load data at offset 5 (non-overlapping qwords)
        zmm_data5 = _mm512_loadu_si512((__m512i const *)(buff + start_pos + (AVX512_REGISTER_SIZE_BYTES * i) + 5));
        
        // Load data at offset 6 (non-overlapping qwords)
        zmm_data6 = _mm512_loadu_si512((__m512i const *)(buff + start_pos + (AVX512_REGISTER_SIZE_BYTES * i) + 6));
        
        // Load data at offset 7 (non-overlapping qwords)
        zmm_data7 = _mm512_loadu_si512((__m512i const *)(buff + start_pos + (AVX512_REGISTER_SIZE_BYTES * i) + 7));
        
        // Update accumulators with minimum values
        acc_offset0 = _mm512_maskz_min_epu64(cmp_mask, acc_offset0, zmm_data0);
        acc_offset1 = _mm512_maskz_min_epu64(cmp_mask, acc_offset1, zmm_data1);
        acc_offset2 = _mm512_maskz_min_epu64(cmp_mask, acc_offset2, zmm_data2);
        acc_offset3 = _mm512_maskz_min_epu64(cmp_mask, acc_offset3, zmm_data3);
        acc_offset4 = _mm512_maskz_min_epu64(cmp_mask, acc_offset4, zmm_data4);
        acc_offset5 = _mm512_maskz_min_epu64(cmp_mask, acc_offset5, zmm_data5);
        acc_offset6 = _mm512_maskz_min_epu64(cmp_mask, acc_offset6, zmm_data6);
        acc_offset7 = _mm512_maskz_min_epu64(cmp_mask, acc_offset7, zmm_data7);
    }

    // Combine the eight accumulators to find the overall minimum
    __m512i final_acc = _mm512_maskz_min_epu64(cmp_mask, acc_offset0, acc_offset1);
    final_acc = _mm512_maskz_min_epu64(cmp_mask, final_acc, acc_offset2);
    final_acc = _mm512_maskz_min_epu64(cmp_mask, final_acc, acc_offset3);
    final_acc = _mm512_maskz_min_epu64(cmp_mask, final_acc, acc_offset4);
    final_acc = _mm512_maskz_min_epu64(cmp_mask, final_acc, acc_offset5);
    final_acc = _mm512_maskz_min_epu64(cmp_mask, final_acc, acc_offset6);
    final_acc = _mm512_maskz_min_epu64(cmp_mask, final_acc, acc_offset7);

    // Move the final set of values from the zmm register into local memory    
    uint64_t result_store[AVX512_REGISTER_SIZE_BYTES / 8] = {0};
    _mm512_storeu_epi64(&result_store, final_acc);

    // Sequentially scan the remaining values to find the min value
    uint64_t min_val = UINT64_MAX;
    for(uint64_t i = 0; i < (AVX512_REGISTER_SIZE_BYTES / 8); i++){
        if(result_store[i] < min_val)
            min_val = result_store[i];
    }

    // Return minimum value
    return min_val;
}

#endif

/*
    ======================================================================
    Range Scan implementations for GEQ (greater than or equal to) comparisons.
        This section has both naive and gated implementations for 2-byte and 4-byte rolls.
        Other Range Scans (such as GT) only have the final versions.
    ======================================================================
*/


 #ifdef __AVX512F__
uint64_t MultiRoll_AVX_Chunking_Technique::range_scan_geq_avx512_naive_2byteroll(char *buff, uint64_t start_position, uint64_t end_position, uint16_t target_value){
    
    uint64_t num_vectors = (end_position - start_position) / AVX512_REGISTER_SIZE_BYTES;
    uint64_t curr_scan_start;
    uint64_t return_pos_1, return_pos_2;


    // Structures to store bytes from data stream and comparison results in 512-bit AVX format
    __m512i zmm_array, zmm_array_2;
    __mmask32 cmp_mask, cmp_mask_1, cmp_mask_2;

    // Set return positions to end of scan threshold to start
    return_pos_1 = end_position;
    return_pos_2 = return_pos_1;

    // Load max_value into xmm-format
    __m512i max_val_xmm = _mm512_set1_epi16(target_value);
    
    for(uint64_t i = 0; i < num_vectors; i++){
        curr_scan_start = start_position + (i * AVX512_REGISTER_SIZE_BYTES);

        // Load data into xmm register
        zmm_array = _mm512_loadu_si512((__m512i const *)(buff + curr_scan_start));
        zmm_array_2 = _mm512_loadu_si512((__m512i const *)(buff + curr_scan_start + 1)); // Offset of 1 to consider non-overlapping words
        
        /* 
         Compare values with max_value. If a byte in zmm_array is geq max_val_xmm,  
         ALL the corresponding bits of the corresponding byte in cmp_array are set to 1.
        */
        
        cmp_mask_1 = _mm512_cmpge_epu16_mask(zmm_array, max_val_xmm);
        cmp_mask_2 = _mm512_cmpge_epu16_mask(zmm_array_2, max_val_xmm);

        cmp_mask = cmp_mask_1 | cmp_mask_2;

        // Return index of first non-zero bit in mask
        // This corresponds to the first non-zero byte in cmp_array 
        if(cmp_mask != 0){
            if(cmp_mask_1)
                return_pos_1 = curr_scan_start + 2*(__builtin_ffs(cmp_mask_1) - 1); // Multiply by 2 since we are comparing 2-byte words and we need the start byte's position 
            if(cmp_mask_2)
                return_pos_2 = curr_scan_start + 1 + 2*(__builtin_ffs(cmp_mask_2) - 1); // Add 1 to index to account for offset     
                       
            return std::min(return_pos_1, return_pos_2);
        }
    }
    
    return end_position;
}

uint64_t MultiRoll_AVX_Chunking_Technique::range_scan_geq_avx512_naive_4byteroll(char *buff, uint64_t start_position, uint64_t end_position, uint32_t target_value){
    
    uint64_t num_vectors = (end_position - start_position) / AVX512_REGISTER_SIZE_BYTES;
    uint64_t curr_scan_start;
    uint64_t return_pos_1, return_pos_2, return_pos_3, return_pos_4;

    // Structures to store bytes from data stream and comparison results in 512-bit AVX format
    __m512i zmm_array, zmm_array_2, zmm_array_3, zmm_array_4;
    __mmask16 cmp_mask, cmp_mask_1, cmp_mask_2, cmp_mask_3, cmp_mask_4;

    // Load max_value into xmm-format
    __m512i max_val_xmm = _mm512_set1_epi32(target_value);

      
    // Set return positions to end of scan threshold to start
    return_pos_1 = end_position;
    return_pos_2 = return_pos_1;
    return_pos_3 = return_pos_1;
    return_pos_4 = return_pos_1;


    for(uint64_t i = 0; i < num_vectors; i++){
        curr_scan_start = start_position + (i * AVX512_REGISTER_SIZE_BYTES);
        
        // Load data into xmm register
        zmm_array = _mm512_loadu_si512((__m512i const *)(buff + curr_scan_start));
        zmm_array_2 = _mm512_loadu_si512((__m512i const *)(buff + curr_scan_start + 1)); // Offset of 1 to consider non-overlapping words
        zmm_array_3 = _mm512_loadu_si512((__m512i const *)(buff + curr_scan_start + 2)); // Offset of 2 to consider non-overlapping words
        zmm_array_4 = _mm512_loadu_si512((__m512i const *)(buff + curr_scan_start + 3)); // Offset of 3 to consider non-overlapping words
        
        /* 
         Compare values with max_value. If a byte in zmm_array is geq max_val_xmm,  
         ALL the corresponding bits of the corresponding byte in cmp_array are set to 1.
        */
        
        cmp_mask_1 = _mm512_cmpge_epu32_mask(zmm_array, max_val_xmm);
        cmp_mask_2 = _mm512_cmpge_epu32_mask(zmm_array_2, max_val_xmm);
        cmp_mask_3 = _mm512_cmpge_epu32_mask(zmm_array_3, max_val_xmm);
        cmp_mask_4 = _mm512_cmpge_epu32_mask(zmm_array_4, max_val_xmm);

        cmp_mask = cmp_mask_1 | cmp_mask_2 | cmp_mask_3 | cmp_mask_4;

        // Return index of first non-zero bit in mask
        // This corresponds to the first non-zero byte in cmp_array 
        if(cmp_mask != 0){
            if(cmp_mask_1)
                return_pos_1 = curr_scan_start + 4*(__builtin_ffsll(cmp_mask_1) - 1); // Multiply by 4 since we are comparing 4-byte words and we need the start byte's position
            if(cmp_mask_2)
                return_pos_2 = curr_scan_start + 1 + 4*(__builtin_ffsll(cmp_mask_2) - 1); // Add 1 to index to account for offset
            if(cmp_mask_3)
                return_pos_3 = curr_scan_start + 2 + 4*(__builtin_ffsll(cmp_mask_3) - 1); // Add 2 to index to account for offset
            if(cmp_mask_4)  
                return_pos_4 = curr_scan_start + 3 + 4*(__builtin_ffsll(cmp_mask_4) - 1); // Add 3 to index to account for offset

            return std::min(std::min(return_pos_1, return_pos_2), std::min(return_pos_3, return_pos_4));            
        }
    }
    return end_position;
}

uint64_t MultiRoll_AVX_Chunking_Technique::range_scan_geq_avx512_naive_8byteroll(char *buff, uint64_t start_position, uint64_t end_position, uint64_t target_value){
    
    uint64_t num_vectors = (end_position - start_position) / AVX512_REGISTER_SIZE_BYTES;
    uint64_t curr_scan_start;
    uint64_t return_pos, return_pos_1, return_pos_2, return_pos_3, return_pos_4, return_pos_5, return_pos_6, return_pos_7, return_pos_8;

    // Structures to store bytes from data stream and comparison results in 512-bit AVX format
    __m512i zmm_array_1, zmm_array_2, zmm_array_3, zmm_array_4, zmm_array_5, zmm_array_6, zmm_array_7, zmm_array_8;
    __mmask8 cmp_mask, cmp_mask_1, cmp_mask_2, cmp_mask_3, cmp_mask_4, cmp_mask_5, cmp_mask_6, cmp_mask_7, cmp_mask_8;

    // Load max_value into xmm-format
    __m512i max_val_xmm = _mm512_set1_epi64((int64_t)target_value);

    return_pos = end_position;
    return_pos_1 = return_pos;
    return_pos_2 = return_pos;
    return_pos_3 = return_pos;
    return_pos_4 = return_pos;
    return_pos_5 = return_pos;
    return_pos_6 = return_pos;
    return_pos_7 = return_pos;
    return_pos_8 = return_pos;
    
    for(uint64_t i = 0; i < num_vectors; i++){
        curr_scan_start = start_position + (i * AVX512_REGISTER_SIZE_BYTES);
        
        // Load data into zmm registers
        zmm_array_1 = _mm512_loadu_si512((__m512i const *)(buff + curr_scan_start));
        zmm_array_2 = _mm512_loadu_si512((__m512i const *)(buff + curr_scan_start + 1)); // Offset of 1 to consider non-overlapping words
        zmm_array_3 = _mm512_loadu_si512((__m512i const *)(buff + curr_scan_start + 2)); // Offset of 2 to consider non-overlapping words
        zmm_array_4 = _mm512_loadu_si512((__m512i const *)(buff + curr_scan_start + 3)); // Offset of 3 to consider non-overlapping words
        zmm_array_5 = _mm512_loadu_si512((__m512i const *)(buff + curr_scan_start + 4)); // Offset of 4 to consider non-overlapping words
        zmm_array_6 = _mm512_loadu_si512((__m512i const *)(buff + curr_scan_start + 5)); // Offset of 5 to consider non-overlapping words
        zmm_array_7 = _mm512_loadu_si512((__m512i const *)(buff + curr_scan_start + 6)); // Offset of 6 to consider non-overlapping words
        zmm_array_8 = _mm512_loadu_si512((__m512i const *)(buff + curr_scan_start + 7)); // Offset of 7 to consider non-overlapping words
        
        /* 
         Compare values with max_value. If a byte in zmm_array is geq max_val_xmm,  
         ALL the corresponding bits of the corresponding byte in cmp_array are set to 1.
        */
        
        cmp_mask_1 = _mm512_cmpge_epu64_mask(zmm_array_1, max_val_xmm);
        cmp_mask_2 = _mm512_cmpge_epu64_mask(zmm_array_2, max_val_xmm);
        cmp_mask_3 = _mm512_cmpge_epu64_mask(zmm_array_3, max_val_xmm);
        cmp_mask_4 = _mm512_cmpge_epu64_mask(zmm_array_4, max_val_xmm);
        cmp_mask_5 = _mm512_cmpge_epu64_mask(zmm_array_5, max_val_xmm);
        cmp_mask_6 = _mm512_cmpge_epu64_mask(zmm_array_6, max_val_xmm);
        cmp_mask_7 = _mm512_cmpge_epu64_mask(zmm_array_7, max_val_xmm);
        cmp_mask_8 = _mm512_cmpge_epu64_mask(zmm_array_8, max_val_xmm);

        cmp_mask = cmp_mask_1 | cmp_mask_2 | cmp_mask_3 | cmp_mask_4 | cmp_mask_5 | cmp_mask_6 | cmp_mask_7 | cmp_mask_8;

        // Return index of first non-zero bit in mask
        // This corresponds to the first non-zero byte in cmp_array 
        if(cmp_mask){
            if(cmp_mask_1)
                return_pos_1 = curr_scan_start + QWORD_ROLL_SIZE_BYTES*(__builtin_ffsll(cmp_mask_1) - 1); // Multiply by 8 since we are comparing 8-byte words and we need the start byte's position
            if(cmp_mask_2)
                return_pos_2 = curr_scan_start + 1 + QWORD_ROLL_SIZE_BYTES*(__builtin_ffsll(cmp_mask_2) - 1); // Add 1 to index to account for offset
            if(cmp_mask_3)
                return_pos_3 = curr_scan_start + 2 + QWORD_ROLL_SIZE_BYTES*(__builtin_ffsll(cmp_mask_3) - 1); // Add 2 to index to account for offset
            if(cmp_mask_4)
                return_pos_4 = curr_scan_start + 3 + QWORD_ROLL_SIZE_BYTES*(__builtin_ffsll(cmp_mask_4) - 1); // Add 3 to index to account for offset
            if(cmp_mask_5)
                return_pos_5 = curr_scan_start + 4 + QWORD_ROLL_SIZE_BYTES*(__builtin_ffsll(cmp_mask_5) - 1); // Add 4 to index to account for offset
            if(cmp_mask_6)
                return_pos_6 = curr_scan_start + 5 + QWORD_ROLL_SIZE_BYTES*(__builtin_ffsll(cmp_mask_6) - 1); // Add 5 to index to account for offset
            if(cmp_mask_7)
                return_pos_7 = curr_scan_start + 6 + QWORD_ROLL_SIZE_BYTES*(__builtin_ffsll(cmp_mask_7) - 1); // Add 6 to index to account for offset
            if(cmp_mask_8)
                return_pos_8 = curr_scan_start + 7 + QWORD_ROLL_SIZE_BYTES*(__builtin_ffsll(cmp_mask_8) - 1); // Add 7 to index to account for offset

            return_pos = return_pos_1;
            return_pos = std::min(return_pos, return_pos_2);
            return_pos = std::min(return_pos, return_pos_3);
            return_pos = std::min(return_pos, return_pos_4);
            return_pos = std::min(return_pos, return_pos_5);
            return_pos = std::min(return_pos, return_pos_6);
            return_pos = std::min(return_pos, return_pos_7);
            return_pos = std::min(return_pos, return_pos_8);

            return return_pos;            
        }
    }
    
    return end_position;
}

uint64_t MultiRoll_AVX_Chunking_Technique::range_scan_geq_avx512_2byteroll(char *buff, uint64_t start_position, uint64_t end_position, uint16_t target_value){
    uint64_t curr_scan_start;
    uint64_t return_pos_gt, return_pos_eq;

    // Structures to store bytes from data stream and comparison results in 512-bit AVX format
    __m512i zmm_array, zmm_array_2;
    __mmask64 cmp_mask_geq, cmp_mask_gt, cmp_mask_eq, cmp_mask_low;

    // Load max_value into two separate xmm's, one with high byte, one with low byte
    uint8_t high_byte = (uint8_t)((target_value >> 8) & 0xFF);
    uint8_t low_byte = (uint8_t)(target_value & 0xFF);

    __m512i zmm_high_byte = _mm512_set1_epi8((char)high_byte);
    __m512i zmm_low_byte = _mm512_set1_epi8((char)low_byte);

    uint64_t INCREMENT_SIZE = AVX512_REGISTER_SIZE_BYTES;
 
    return_pos_gt = end_position;
    return_pos_eq = end_position;
    curr_scan_start = start_position; // Initialize to start - increment to account for first addition in loop

    
    while(curr_scan_start + 1 + AVX512_REGISTER_SIZE_BYTES < end_position){
   
        // Load data into xmm register
        zmm_array = _mm512_loadu_si512((__m512i const *)(buff + curr_scan_start + 1)); // Load with offset of 1 to align high bytes
        
        /* 
         Compare values with max_value. If a byte in xmm_array is geq max_val_xmm,  
         ALL the corresponding bits of the corresponding byte in cmp_array are set to 1.
        */
        
        cmp_mask_geq = _mm512_cmpge_epu8_mask(zmm_array, zmm_high_byte);

      
        // If match found for high bytes, check low bytes at those positions with vector cmp
        if(cmp_mask_geq){

            cmp_mask_gt = _mm512_cmpgt_epu8_mask(zmm_array, zmm_high_byte);
            cmp_mask_eq = _mm512_cmpeq_epu8_mask(zmm_array, zmm_high_byte);

            return_pos_gt = end_position;
            return_pos_eq = end_position;

            if(cmp_mask_gt){
                // If any high byte is greater than target high byte, we have a match
                return_pos_gt = curr_scan_start + (__builtin_ffsll(cmp_mask_gt) - 1); // Multiply by 2 since we are comparing 2-byte words and we need the start byte's position 
            }
            
            if(cmp_mask_eq){
                zmm_array_2 = _mm512_loadu_si512((__m512i const *)(buff + curr_scan_start)) ; // Load low bytes by offsetting by 1
                cmp_mask_low = _mm512_cmpge_epu8_mask(zmm_array_2, zmm_low_byte);

                // Mask low byte comparisons with equality mask from high byte comparison
                cmp_mask_low = cmp_mask_low & cmp_mask_eq;

                if(cmp_mask_low){
                    // If any low byte is greater than or equal to target low byte, we have a match
                    return_pos_eq = curr_scan_start + (__builtin_ffsll(cmp_mask_low) - 1); // Add 1 to index to account for offset            
                }
            }
            
            if(return_pos_gt != end_position || return_pos_eq != end_position)
                return std::min(return_pos_gt, return_pos_eq);
               
        }

        curr_scan_start += INCREMENT_SIZE;
    }

    // Account for remaining tail by scanning serially
    for(uint64_t i = curr_scan_start; i < end_position; i++){
        uint16_t val = *((uint16_t *)(buff + i));
        if(val >= target_value){
            return i;
        }
    }
    
    return end_position;
}


uint64_t MultiRoll_AVX_Chunking_Technique::range_scan_geq_avx512_4byteroll(char *buff, uint64_t start_position, uint64_t end_position, uint32_t target_value){

    uint64_t curr_scan_start;
    uint64_t return_pos_0, return_pos_1, return_pos_2, return_pos_3;

    // For 2-byte gating: upper 2 bytes of the 4-byte word
    uint16_t upper_2bytes = (uint16_t)((target_value >> 16) & 0xFFFF);

    __m512i zmm_upper2bytes = _mm512_set1_epi16((int16_t)upper_2bytes);
    __m512i zmm_target_4byte = _mm512_set1_epi32((int32_t)target_value);

    __m512i zmm_gate_offset2, zmm_gate_offset3;
    __m512i zmm_array_3, zmm_array_2, zmm_array_1, zmm_array_0;
    __mmask32 mask_gate_offset2, mask_gate_offset3;
    __mmask16 mask_cmp_0, mask_cmp_1, mask_cmp_2, mask_cmp_3;

    uint64_t INCREMENT_SIZE = AVX512_REGISTER_SIZE_BYTES; // Step by full register; refinement happens only on upper 2-byte matches

    return_pos_0 = end_position;
    return_pos_1 = end_position;
    return_pos_2 = end_position;
    return_pos_3 = end_position;
    curr_scan_start = start_position;

    // Need room for the upper 2-byte load at offset +3
    while(curr_scan_start + AVX512_REGISTER_SIZE_BYTES + 3 < end_position){

        // Load upper 2 bytes (as 16-bit words) at offset +2 and +3 for gating
        zmm_gate_offset2 = _mm512_loadu_si512((__m512i const *)(buff + curr_scan_start + 2));
        zmm_gate_offset3 = _mm512_loadu_si512((__m512i const *)(buff + curr_scan_start + 3));
        
        mask_gate_offset2 = _mm512_cmpge_epu16_mask(zmm_gate_offset2, zmm_upper2bytes);
        mask_gate_offset3 = _mm512_cmpge_epu16_mask(zmm_gate_offset3, zmm_upper2bytes);

        if(mask_gate_offset2 || mask_gate_offset3){
            // Gate passed, perform full 4-byte comparison at all 4 offsets
            
            // Reset return positions for this iteration
            return_pos_0 = end_position;
            return_pos_1 = end_position;
            return_pos_2 = end_position;
            return_pos_3 = end_position;

            // Reuse the already-loaded data from offsets +2 and +3
            zmm_array_3 = zmm_gate_offset3;
            zmm_array_2 = zmm_gate_offset2;
            zmm_array_1 = _mm512_loadu_si512((__m512i const *)(buff + curr_scan_start + 1));
            zmm_array_0 = _mm512_loadu_si512((__m512i const *)(buff + curr_scan_start));

            // Compare each offset as 32-bit unsigned integers
            mask_cmp_0 = _mm512_cmpge_epu32_mask(zmm_array_0, zmm_target_4byte);
            mask_cmp_1 = _mm512_cmpge_epu32_mask(zmm_array_1, zmm_target_4byte);
            mask_cmp_2 = _mm512_cmpge_epu32_mask(zmm_array_2, zmm_target_4byte);
            mask_cmp_3 = _mm512_cmpge_epu32_mask(zmm_array_3, zmm_target_4byte);

            // Find first match in each offset and calculate position
            if(mask_cmp_0)
                return_pos_0 = curr_scan_start + 4 * (__builtin_ffs(mask_cmp_0) - 1);
            if(mask_cmp_1)
                return_pos_1 = curr_scan_start + 1 + 4 * (__builtin_ffs(mask_cmp_1) - 1);
            if(mask_cmp_2)
                return_pos_2 = curr_scan_start + 2 + 4 * (__builtin_ffs(mask_cmp_2) - 1);
            if(mask_cmp_3)
                return_pos_3 = curr_scan_start + 3 + 4 * (__builtin_ffs(mask_cmp_3) - 1);

            // Return the minimum position
            uint64_t min_pos = std::min(std::min(return_pos_0, return_pos_1), std::min(return_pos_2, return_pos_3));
            if(min_pos < end_position){
                return min_pos;
            }
        }
      
        curr_scan_start += INCREMENT_SIZE;
    }

    // Account for remaining tail by scanning serially
    for(uint64_t i = curr_scan_start; i < end_position; i++){
        uint32_t val = *((uint32_t *)(buff + i));
        if(val >= target_value){
            return i;
        }
    }
    
    return end_position;
}

uint64_t MultiRoll_AVX_Chunking_Technique::range_scan_geq_avx512_8byteroll(char *buff, uint64_t start_position, uint64_t end_position, uint64_t target_value){

    uint64_t curr_scan_start;
    uint64_t return_pos_0, return_pos_1, return_pos_2, return_pos_3, return_pos_4, return_pos_5, return_pos_6, return_pos_7;

    // For 2-byte gating: upper 2 bytes of the 8-byte word (bytes 6-7)
    uint16_t upper_2bytes = (uint16_t)((target_value >> 48) & 0xFFFF);

    __m512i zmm_upper2bytes = _mm512_set1_epi16((int16_t)upper_2bytes);
    __m512i zmm_target_8byte = _mm512_set1_epi64((int64_t)target_value);

    __m512i zmm_gate_offset6, zmm_gate_offset7;
    __m512i zmm_array_7, zmm_array_6, zmm_array_5, zmm_array_4, zmm_array_3, zmm_array_2, zmm_array_1, zmm_array_0;
    __mmask32 mask_gate_offset6, mask_gate_offset7;
    __mmask8 mask_cmp_0, mask_cmp_1, mask_cmp_2, mask_cmp_3, mask_cmp_4, mask_cmp_5, mask_cmp_6, mask_cmp_7;

    uint64_t INCREMENT_SIZE = AVX512_REGISTER_SIZE_BYTES; // Step by full register; refinement happens only on upper 2-byte matches

    return_pos_0 = end_position;
    return_pos_1 = end_position;
    return_pos_2 = end_position;
    return_pos_3 = end_position;
    return_pos_4 = end_position;
    return_pos_5 = end_position;
    return_pos_6 = end_position;
    return_pos_7 = end_position;
    curr_scan_start = start_position;

    // Need room for the upper 2-byte load at offset +7
    while(curr_scan_start + AVX512_REGISTER_SIZE_BYTES + 7 < end_position){

        // Load upper 2 bytes (as 16-bit words) at offset +6 and +7 for gating
        zmm_gate_offset6 = _mm512_loadu_si512((__m512i const *)(buff + curr_scan_start + 6));
        zmm_gate_offset7 = _mm512_loadu_si512((__m512i const *)(buff + curr_scan_start + 7));
        
        mask_gate_offset6 = _mm512_cmpge_epu16_mask(zmm_gate_offset6, zmm_upper2bytes);
        mask_gate_offset7 = _mm512_cmpge_epu16_mask(zmm_gate_offset7, zmm_upper2bytes);

        if(mask_gate_offset6 || mask_gate_offset7){
            // Gate passed, perform full 8-byte comparison at all 8 offsets
            
            // Reset return positions for this iteration
            return_pos_0 = end_position;
            return_pos_1 = end_position;
            return_pos_2 = end_position;
            return_pos_3 = end_position;
            return_pos_4 = end_position;
            return_pos_5 = end_position;
            return_pos_6 = end_position;
            return_pos_7 = end_position;

            // Reuse the already-loaded data from offsets +6 and +7
            zmm_array_7 = zmm_gate_offset7;
            zmm_array_6 = zmm_gate_offset6;
            zmm_array_5 = _mm512_loadu_si512((__m512i const *)(buff + curr_scan_start + 5));
            zmm_array_4 = _mm512_loadu_si512((__m512i const *)(buff + curr_scan_start + 4));
            zmm_array_3 = _mm512_loadu_si512((__m512i const *)(buff + curr_scan_start + 3));
            zmm_array_2 = _mm512_loadu_si512((__m512i const *)(buff + curr_scan_start + 2));
            zmm_array_1 = _mm512_loadu_si512((__m512i const *)(buff + curr_scan_start + 1));
            zmm_array_0 = _mm512_loadu_si512((__m512i const *)(buff + curr_scan_start));

            // Compare each offset as 64-bit unsigned integers
            mask_cmp_0 = _mm512_cmpge_epu64_mask(zmm_array_0, zmm_target_8byte);
            mask_cmp_1 = _mm512_cmpge_epu64_mask(zmm_array_1, zmm_target_8byte);
            mask_cmp_2 = _mm512_cmpge_epu64_mask(zmm_array_2, zmm_target_8byte);
            mask_cmp_3 = _mm512_cmpge_epu64_mask(zmm_array_3, zmm_target_8byte);
            mask_cmp_4 = _mm512_cmpge_epu64_mask(zmm_array_4, zmm_target_8byte);
            mask_cmp_5 = _mm512_cmpge_epu64_mask(zmm_array_5, zmm_target_8byte);
            mask_cmp_6 = _mm512_cmpge_epu64_mask(zmm_array_6, zmm_target_8byte);
            mask_cmp_7 = _mm512_cmpge_epu64_mask(zmm_array_7, zmm_target_8byte);

            // Find first match in each offset and calculate position
            if(mask_cmp_0)
                return_pos_0 = curr_scan_start + 8 * (__builtin_ffs(mask_cmp_0) - 1);
            if(mask_cmp_1)
                return_pos_1 = curr_scan_start + 1 + 8 * (__builtin_ffs(mask_cmp_1) - 1);
            if(mask_cmp_2)
                return_pos_2 = curr_scan_start + 2 + 8 * (__builtin_ffs(mask_cmp_2) - 1);
            if(mask_cmp_3)
                return_pos_3 = curr_scan_start + 3 + 8 * (__builtin_ffs(mask_cmp_3) - 1);
            if(mask_cmp_4)
                return_pos_4 = curr_scan_start + 4 + 8 * (__builtin_ffs(mask_cmp_4) - 1);
            if(mask_cmp_5)
                return_pos_5 = curr_scan_start + 5 + 8 * (__builtin_ffs(mask_cmp_5) - 1);
            if(mask_cmp_6)
                return_pos_6 = curr_scan_start + 6 + 8 * (__builtin_ffs(mask_cmp_6) - 1);
            if(mask_cmp_7)
                return_pos_7 = curr_scan_start + 7 + 8 * (__builtin_ffs(mask_cmp_7) - 1);

            // Return the minimum position
            uint64_t min_pos_01 = std::min(return_pos_0, return_pos_1);
            uint64_t min_pos_23 = std::min(return_pos_2, return_pos_3);
            uint64_t min_pos_45 = std::min(return_pos_4, return_pos_5);
            uint64_t min_pos_67 = std::min(return_pos_6, return_pos_7);
            uint64_t min_pos = std::min(std::min(min_pos_01, min_pos_23), std::min(min_pos_45, min_pos_67));
            if(min_pos < end_position){
                return min_pos;
            }
        }
      
        curr_scan_start += INCREMENT_SIZE;
    }

    // Account for remaining tail by scanning serially
    for(uint64_t i = curr_scan_start; i < end_position; i++){
        uint64_t val = *((uint64_t *)(buff + i));
        if(val >= target_value){
            return i;
        }
    }

    return end_position;
}

#endif

/*
    ======================================================================
    Range Scan implementations for GT (strictly greater than) comparisons.
    ======================================================================
*/

#ifdef __AVX512F__

uint64_t MultiRoll_AVX_Chunking_Technique::range_scan_gt_avx512_2byteroll(char *buff, uint64_t start_position, uint64_t end_position, uint16_t target_value){
    
    uint64_t num_vectors = (end_position - start_position) / AVX512_REGISTER_SIZE_BYTES;
    uint64_t curr_scan_start;
    uint64_t return_pos_1, return_pos_2;

    // Structures to store bytes from data stream and comparison results in 512-bit AVX format
    __m512i zmm_array, zmm_array_2;
    __mmask32 cmp_mask, cmp_mask_1, cmp_mask_2;

    // Set return positions to end of scan threshold to start
    return_pos_1 = end_position;
    return_pos_2 = return_pos_1;

    // Load target_value into zmm-format
    __m512i target_val_zmm = _mm512_set1_epi16(target_value);
    
    for(uint64_t i = 0; i < num_vectors; i++){
        curr_scan_start = start_position + (i * AVX512_REGISTER_SIZE_BYTES);

        // Load data into zmm register
        zmm_array = _mm512_loadu_si512((__m512i const *)(buff + curr_scan_start));
        zmm_array_2 = _mm512_loadu_si512((__m512i const *)(buff + curr_scan_start + 1)); // Offset of 1 to consider non-overlapping words
        
        /* 
         Compare values with target_value. If a value in zmm_array is gt target_val_zmm,  
         the corresponding bit in cmp_mask is set to 1.
        */
        
        cmp_mask_1 = _mm512_cmpgt_epu16_mask(zmm_array, target_val_zmm);
        cmp_mask_2 = _mm512_cmpgt_epu16_mask(zmm_array_2, target_val_zmm);

        cmp_mask = cmp_mask_1 | cmp_mask_2;

        // Return index of first non-zero bit in mask
        // This corresponds to the first non-zero byte in cmp_array 
        if(cmp_mask != 0){
            if(cmp_mask_1)
                return_pos_1 = curr_scan_start + 2*(__builtin_ffs(cmp_mask_1) - 1); // Multiply by 2 since we are comparing 2-byte words and we need the start byte's position 
            if(cmp_mask_2)
                return_pos_2 = curr_scan_start + 1 + 2*(__builtin_ffs(cmp_mask_2) - 1); // Add 1 to index to account for offset            

            return std::min(return_pos_1, return_pos_2);
        }
    }
    
    return end_position;
}

uint64_t MultiRoll_AVX_Chunking_Technique::range_scan_gt_avx512_4byteroll(char *buff, uint64_t start_position, uint64_t end_position, uint32_t target_value){

    uint64_t curr_scan_start;
    uint64_t return_pos_0, return_pos_1, return_pos_2, return_pos_3;

    // For 2-byte gating: upper 2 bytes of the 4-byte word
    uint16_t upper_2bytes = (uint16_t)((target_value >> 16) & 0xFFFF);

    __m512i zmm_upper2bytes = _mm512_set1_epi16((int16_t)upper_2bytes);
    __m512i zmm_target_4byte = _mm512_set1_epi32((int32_t)target_value);

    __m512i zmm_gate_offset2, zmm_gate_offset3;
    __m512i zmm_array_3, zmm_array_2, zmm_array_1, zmm_array_0;
    __mmask32 mask_gate_offset2, mask_gate_offset3;
    __mmask16 mask_cmp_0, mask_cmp_1, mask_cmp_2, mask_cmp_3;

    uint64_t INCREMENT_SIZE = AVX512_REGISTER_SIZE_BYTES; // Step by full register; refinement happens only on upper 2-byte matches

    return_pos_0 = end_position;
    return_pos_1 = end_position;
    return_pos_2 = end_position;
    return_pos_3 = end_position;
    curr_scan_start = start_position;

    // Need room for the upper 2-byte load at offset +3
    while(curr_scan_start + AVX512_REGISTER_SIZE_BYTES + 3 < end_position){

        // Load upper 2 bytes (as 16-bit words) at offset +2 and +3 for gating
        zmm_gate_offset2 = _mm512_loadu_si512((__m512i const *)(buff + curr_scan_start + 2));
        zmm_gate_offset3 = _mm512_loadu_si512((__m512i const *)(buff + curr_scan_start + 3));
        
        mask_gate_offset2 = _mm512_cmpge_epu16_mask(zmm_gate_offset2, zmm_upper2bytes);
        mask_gate_offset3 = _mm512_cmpge_epu16_mask(zmm_gate_offset3, zmm_upper2bytes);

        if(mask_gate_offset2 || mask_gate_offset3){
            // Gate passed, perform full 4-byte comparison at all 4 offsets
            
            // Reset return positions for this iteration
            return_pos_0 = end_position;
            return_pos_1 = end_position;
            return_pos_2 = end_position;
            return_pos_3 = end_position;

            // Reuse the already-loaded data from offsets +2 and +3
            zmm_array_3 = zmm_gate_offset3;
            zmm_array_2 = zmm_gate_offset2;
            zmm_array_1 = _mm512_loadu_si512((__m512i const *)(buff + curr_scan_start + 1));
            zmm_array_0 = _mm512_loadu_si512((__m512i const *)(buff + curr_scan_start));

            // Compare each offset as 32-bit unsigned integers (strictly greater than)
            mask_cmp_0 = _mm512_cmpgt_epu32_mask(zmm_array_0, zmm_target_4byte);
            mask_cmp_1 = _mm512_cmpgt_epu32_mask(zmm_array_1, zmm_target_4byte);
            mask_cmp_2 = _mm512_cmpgt_epu32_mask(zmm_array_2, zmm_target_4byte);
            mask_cmp_3 = _mm512_cmpgt_epu32_mask(zmm_array_3, zmm_target_4byte);

            // Find first match in each offset and calculate position
            if(mask_cmp_0)
                return_pos_0 = curr_scan_start + 4 * (__builtin_ffs(mask_cmp_0) - 1);
            if(mask_cmp_1)
                return_pos_1 = curr_scan_start + 1 + 4 * (__builtin_ffs(mask_cmp_1) - 1);
            if(mask_cmp_2)
                return_pos_2 = curr_scan_start + 2 + 4 * (__builtin_ffs(mask_cmp_2) - 1);
            if(mask_cmp_3)
                return_pos_3 = curr_scan_start + 3 + 4 * (__builtin_ffs(mask_cmp_3) - 1);

            // Return the minimum position
            uint64_t min_pos = std::min(std::min(return_pos_0, return_pos_1), std::min(return_pos_2, return_pos_3));
            if(min_pos < end_position){
                return min_pos;
            }
        }
      
        curr_scan_start += INCREMENT_SIZE;
    }

    // Account for remaining tail by scanning serially
    for(uint64_t i = curr_scan_start; i < end_position; i++){
        uint32_t val = *((uint32_t *)(buff + i));
        if(val > target_value){
            return i;
        }
    }
    
    return end_position;
}

uint64_t MultiRoll_AVX_Chunking_Technique::range_scan_gt_avx512_8byteroll(char *buff, uint64_t start_position, uint64_t end_position, uint64_t target_value){

    uint64_t curr_scan_start;
    uint64_t return_pos_0, return_pos_1, return_pos_2, return_pos_3, return_pos_4, return_pos_5, return_pos_6, return_pos_7;

    // For 2-byte gating: upper 2 bytes of the 8-byte word (bytes 6-7)
    uint16_t upper_2bytes = (uint16_t)((target_value >> 48) & 0xFFFF);

    __m512i zmm_upper2bytes = _mm512_set1_epi16((int16_t)upper_2bytes);
    __m512i zmm_target_8byte = _mm512_set1_epi64((int64_t)target_value);

    __m512i zmm_gate_offset6, zmm_gate_offset7;
    __m512i zmm_array_7, zmm_array_6, zmm_array_5, zmm_array_4, zmm_array_3, zmm_array_2, zmm_array_1, zmm_array_0;
    __mmask32 mask_gate_offset6, mask_gate_offset7;
    __mmask8 mask_cmp_0, mask_cmp_1, mask_cmp_2, mask_cmp_3, mask_cmp_4, mask_cmp_5, mask_cmp_6, mask_cmp_7;

    uint64_t INCREMENT_SIZE = AVX512_REGISTER_SIZE_BYTES; // Step by full register; refinement happens only on upper 2-byte matches

    return_pos_0 = end_position;
    return_pos_1 = end_position;
    return_pos_2 = end_position;
    return_pos_3 = end_position;
    return_pos_4 = end_position;
    return_pos_5 = end_position;
    return_pos_6 = end_position;
    return_pos_7 = end_position;
    curr_scan_start = start_position;

    // Need room for the upper 2-byte load at offset +7
    while(curr_scan_start + AVX512_REGISTER_SIZE_BYTES + 7 < end_position){

        // Load upper 2 bytes (as 16-bit words) at offset +6 and +7 for gating
        zmm_gate_offset6 = _mm512_loadu_si512((__m512i const *)(buff + curr_scan_start + 6));
        zmm_gate_offset7 = _mm512_loadu_si512((__m512i const *)(buff + curr_scan_start + 7));
        
        mask_gate_offset6 = _mm512_cmpge_epu16_mask(zmm_gate_offset6, zmm_upper2bytes);
        mask_gate_offset7 = _mm512_cmpge_epu16_mask(zmm_gate_offset7, zmm_upper2bytes);

        if(mask_gate_offset6 || mask_gate_offset7){
            // Gate passed, perform full 8-byte comparison at all 8 offsets
            
            // Reset return positions for this iteration
            return_pos_0 = end_position;
            return_pos_1 = end_position;
            return_pos_2 = end_position;
            return_pos_3 = end_position;
            return_pos_4 = end_position;
            return_pos_5 = end_position;
            return_pos_6 = end_position;
            return_pos_7 = end_position;

            // Reuse the already-loaded data from offsets +6 and +7
            zmm_array_7 = zmm_gate_offset7;
            zmm_array_6 = zmm_gate_offset6;
            zmm_array_5 = _mm512_loadu_si512((__m512i const *)(buff + curr_scan_start + 5));
            zmm_array_4 = _mm512_loadu_si512((__m512i const *)(buff + curr_scan_start + 4));
            zmm_array_3 = _mm512_loadu_si512((__m512i const *)(buff + curr_scan_start + 3));
            zmm_array_2 = _mm512_loadu_si512((__m512i const *)(buff + curr_scan_start + 2));
            zmm_array_1 = _mm512_loadu_si512((__m512i const *)(buff + curr_scan_start + 1));
            zmm_array_0 = _mm512_loadu_si512((__m512i const *)(buff + curr_scan_start));

            // Compare each offset as 64-bit unsigned integers (strictly greater than)
            mask_cmp_0 = _mm512_cmpgt_epu64_mask(zmm_array_0, zmm_target_8byte);
            mask_cmp_1 = _mm512_cmpgt_epu64_mask(zmm_array_1, zmm_target_8byte);
            mask_cmp_2 = _mm512_cmpgt_epu64_mask(zmm_array_2, zmm_target_8byte);
            mask_cmp_3 = _mm512_cmpgt_epu64_mask(zmm_array_3, zmm_target_8byte);
            mask_cmp_4 = _mm512_cmpgt_epu64_mask(zmm_array_4, zmm_target_8byte);
            mask_cmp_5 = _mm512_cmpgt_epu64_mask(zmm_array_5, zmm_target_8byte);
            mask_cmp_6 = _mm512_cmpgt_epu64_mask(zmm_array_6, zmm_target_8byte);
            mask_cmp_7 = _mm512_cmpgt_epu64_mask(zmm_array_7, zmm_target_8byte);

            // Find first match in each offset and calculate position
            if(mask_cmp_0)
                return_pos_0 = curr_scan_start + 8 * (__builtin_ffs(mask_cmp_0) - 1);
            if(mask_cmp_1)
                return_pos_1 = curr_scan_start + 1 + 8 * (__builtin_ffs(mask_cmp_1) - 1);
            if(mask_cmp_2)
                return_pos_2 = curr_scan_start + 2 + 8 * (__builtin_ffs(mask_cmp_2) - 1);
            if(mask_cmp_3)
                return_pos_3 = curr_scan_start + 3 + 8 * (__builtin_ffs(mask_cmp_3) - 1);
            if(mask_cmp_4)
                return_pos_4 = curr_scan_start + 4 + 8 * (__builtin_ffs(mask_cmp_4) - 1);
            if(mask_cmp_5)
                return_pos_5 = curr_scan_start + 5 + 8 * (__builtin_ffs(mask_cmp_5) - 1);
            if(mask_cmp_6)
                return_pos_6 = curr_scan_start + 6 + 8 * (__builtin_ffs(mask_cmp_6) - 1);
            if(mask_cmp_7)
                return_pos_7 = curr_scan_start + 7 + 8 * (__builtin_ffs(mask_cmp_7) - 1);

            // Return the minimum position
            uint64_t min_pos_01 = std::min(return_pos_0, return_pos_1);
            uint64_t min_pos_23 = std::min(return_pos_2, return_pos_3);
            uint64_t min_pos_45 = std::min(return_pos_4, return_pos_5);
            uint64_t min_pos_67 = std::min(return_pos_6, return_pos_7);
            uint64_t min_pos = std::min(std::min(min_pos_01, min_pos_23), std::min(min_pos_45, min_pos_67));
            if(min_pos < end_position){
                return min_pos;
            }
        }
      
        curr_scan_start += INCREMENT_SIZE;
    }

    // Account for remaining tail by scanning serially
    for(uint64_t i = curr_scan_start; i < end_position; i++){
        uint64_t val = *((uint64_t *)(buff + i));
        if(val > target_value){
            return i;
        }
    }

    return end_position;
}

#endif

/*
    ======================================================================
    Range Scan implementations for LT (strictly less than) comparisons.
    ======================================================================
*/

#ifdef __AVX512F__

uint64_t MultiRoll_AVX_Chunking_Technique::range_scan_lt_avx512_2byteroll(char *buff, uint64_t start_position, uint64_t end_position, uint16_t target_value){
    
    uint64_t num_vectors = (end_position - start_position) / AVX512_REGISTER_SIZE_BYTES;
    uint64_t curr_scan_start;
    uint64_t return_pos_1, return_pos_2;

    // Structures to store bytes from data stream and comparison results in 512-bit AVX format
    __m512i zmm_array, zmm_array_2;
    __mmask32 cmp_mask, cmp_mask_1, cmp_mask_2;

    // Set return positions to end of scan threshold to start
    return_pos_1 = end_position;
    return_pos_2 = return_pos_1;

    // Load target_value into zmm-format
    __m512i target_val_zmm = _mm512_set1_epi16(target_value);
    
    for(uint64_t i = 0; i < num_vectors; i++){
        curr_scan_start = start_position + (i * AVX512_REGISTER_SIZE_BYTES);

        // Load data into zmm register
        zmm_array = _mm512_loadu_si512((__m512i const *)(buff + curr_scan_start));
        zmm_array_2 = _mm512_loadu_si512((__m512i const *)(buff + curr_scan_start + 1)); // Offset of 1 to consider non-overlapping words
        
        /* 
         Compare values with target_value. If a value in zmm_array is lt target_val_zmm,  
         the corresponding bit in cmp_mask is set to 1.
        */
        
        cmp_mask_1 = _mm512_cmplt_epu16_mask(zmm_array, target_val_zmm);
        cmp_mask_2 = _mm512_cmplt_epu16_mask(zmm_array_2, target_val_zmm);

        cmp_mask = cmp_mask_1 | cmp_mask_2;

        // Return index of first non-zero bit in mask
        // This corresponds to the first non-zero byte in cmp_array 
        if(cmp_mask != 0){
            if(cmp_mask_1)
                return_pos_1 = curr_scan_start + 2*(__builtin_ffs(cmp_mask_1) - 1); // Multiply by 2 since we are comparing 2-byte words and we need the start byte's position 
            if(cmp_mask_2)
                return_pos_2 = curr_scan_start + 1 + 2*(__builtin_ffs(cmp_mask_2) - 1); // Add 1 to index to account for offset            

            return std::min(return_pos_1, return_pos_2);
        }
    }
    
    return end_position;
}

uint64_t MultiRoll_AVX_Chunking_Technique::range_scan_lt_avx512_4byteroll(char *buff, uint64_t start_position, uint64_t end_position, uint32_t target_value){

    uint64_t curr_scan_start;
    uint64_t return_pos_0, return_pos_1, return_pos_2, return_pos_3;

    // For 2-byte gating: upper 2 bytes of the 4-byte word
    uint16_t upper_2bytes = (uint16_t)((target_value >> 16) & 0xFFFF);

    __m512i zmm_upper2bytes = _mm512_set1_epi16((int16_t)upper_2bytes);
    __m512i zmm_target_4byte = _mm512_set1_epi32((int32_t)target_value);

    __m512i zmm_gate_offset2, zmm_gate_offset3;
    __m512i zmm_array_3, zmm_array_2, zmm_array_1, zmm_array_0;
    __mmask32 mask_gate_offset2, mask_gate_offset3;
    __mmask16 mask_cmp_0, mask_cmp_1, mask_cmp_2, mask_cmp_3;

    uint64_t INCREMENT_SIZE = AVX512_REGISTER_SIZE_BYTES; // Step by full register; refinement happens only on upper 2-byte matches

    return_pos_0 = end_position;
    return_pos_1 = end_position;
    return_pos_2 = end_position;
    return_pos_3 = end_position;
    curr_scan_start = start_position;

    // Need room for the upper 2-byte load at offset +3
    while(curr_scan_start + AVX512_REGISTER_SIZE_BYTES + 3 < end_position){

        // Load upper 2 bytes (as 16-bit words) at offset +2 and +3 for gating
        zmm_gate_offset2 = _mm512_loadu_si512((__m512i const *)(buff + curr_scan_start + 2));
        zmm_gate_offset3 = _mm512_loadu_si512((__m512i const *)(buff + curr_scan_start + 3));
        
        mask_gate_offset2 = _mm512_cmple_epu16_mask(zmm_gate_offset2, zmm_upper2bytes);
        mask_gate_offset3 = _mm512_cmple_epu16_mask(zmm_gate_offset3, zmm_upper2bytes);

        if(mask_gate_offset2 || mask_gate_offset3){
            // Gate passed, perform full 4-byte comparison at all 4 offsets
            
            // Reset return positions for this iteration
            return_pos_0 = end_position;
            return_pos_1 = end_position;
            return_pos_2 = end_position;
            return_pos_3 = end_position;

            // Reuse the already-loaded data from offsets +2 and +3
            zmm_array_3 = zmm_gate_offset3;
            zmm_array_2 = zmm_gate_offset2;
            zmm_array_1 = _mm512_loadu_si512((__m512i const *)(buff + curr_scan_start + 1));
            zmm_array_0 = _mm512_loadu_si512((__m512i const *)(buff + curr_scan_start));

            // Compare each offset as 32-bit unsigned integers (strictly less than)
            mask_cmp_0 = _mm512_cmplt_epu32_mask(zmm_array_0, zmm_target_4byte);
            mask_cmp_1 = _mm512_cmplt_epu32_mask(zmm_array_1, zmm_target_4byte);
            mask_cmp_2 = _mm512_cmplt_epu32_mask(zmm_array_2, zmm_target_4byte);
            mask_cmp_3 = _mm512_cmplt_epu32_mask(zmm_array_3, zmm_target_4byte);

            // Find first match in each offset and calculate position
            if(mask_cmp_0)
                return_pos_0 = curr_scan_start + 4 * (__builtin_ffs(mask_cmp_0) - 1);
            if(mask_cmp_1)
                return_pos_1 = curr_scan_start + 1 + 4 * (__builtin_ffs(mask_cmp_1) - 1);
            if(mask_cmp_2)
                return_pos_2 = curr_scan_start + 2 + 4 * (__builtin_ffs(mask_cmp_2) - 1);
            if(mask_cmp_3)
                return_pos_3 = curr_scan_start + 3 + 4 * (__builtin_ffs(mask_cmp_3) - 1);

            // Return the minimum position
            uint64_t min_pos = std::min(std::min(return_pos_0, return_pos_1), std::min(return_pos_2, return_pos_3));
            if(min_pos < end_position){
                return min_pos;
            }
        }
      
        curr_scan_start += INCREMENT_SIZE;
    }

    // Account for remaining tail by scanning serially
    for(uint64_t i = curr_scan_start; i < end_position; i++){
        uint32_t val = *((uint32_t *)(buff + i));
        if(val < target_value){
            return i;
        }
    }
    
    return end_position;
}

uint64_t MultiRoll_AVX_Chunking_Technique::range_scan_lt_avx512_8byteroll(char *buff, uint64_t start_position, uint64_t end_position, uint64_t target_value){

    uint64_t curr_scan_start;
    uint64_t return_pos_0, return_pos_1, return_pos_2, return_pos_3, return_pos_4, return_pos_5, return_pos_6, return_pos_7;

    // For 2-byte gating: upper 2 bytes of the 8-byte word (bytes 6-7)
    uint16_t upper_2bytes = (uint16_t)((target_value >> 48) & 0xFFFF);

    __m512i zmm_upper2bytes = _mm512_set1_epi16((int16_t)upper_2bytes);
    __m512i zmm_target_8byte = _mm512_set1_epi64((int64_t)target_value);

    __m512i zmm_gate_offset6, zmm_gate_offset7;
    __m512i zmm_array_7, zmm_array_6, zmm_array_5, zmm_array_4, zmm_array_3, zmm_array_2, zmm_array_1, zmm_array_0;
    __mmask32 mask_gate_offset6, mask_gate_offset7;
    __mmask8 mask_cmp_0, mask_cmp_1, mask_cmp_2, mask_cmp_3, mask_cmp_4, mask_cmp_5, mask_cmp_6, mask_cmp_7;

    uint64_t INCREMENT_SIZE = AVX512_REGISTER_SIZE_BYTES; // Step by full register; refinement happens only on upper 2-byte matches

    return_pos_0 = end_position;
    return_pos_1 = end_position;
    return_pos_2 = end_position;
    return_pos_3 = end_position;
    return_pos_4 = end_position;
    return_pos_5 = end_position;
    return_pos_6 = end_position;
    return_pos_7 = end_position;
    curr_scan_start = start_position;

    // Need room for the upper 2-byte load at offset +7
    while(curr_scan_start + AVX512_REGISTER_SIZE_BYTES + 7 < end_position){

        // Load upper 2 bytes (as 16-bit words) at offset +6 and +7 for gating
        zmm_gate_offset6 = _mm512_loadu_si512((__m512i const *)(buff + curr_scan_start + 6));
        zmm_gate_offset7 = _mm512_loadu_si512((__m512i const *)(buff + curr_scan_start + 7));
        
        mask_gate_offset6 = _mm512_cmple_epu16_mask(zmm_gate_offset6, zmm_upper2bytes);
        mask_gate_offset7 = _mm512_cmple_epu16_mask(zmm_gate_offset7, zmm_upper2bytes);

        if(mask_gate_offset6 || mask_gate_offset7){
            // Gate passed, perform full 8-byte comparison at all 8 offsets
            
            // Reset return positions for this iteration
            return_pos_0 = end_position;
            return_pos_1 = end_position;
            return_pos_2 = end_position;
            return_pos_3 = end_position;
            return_pos_4 = end_position;
            return_pos_5 = end_position;
            return_pos_6 = end_position;
            return_pos_7 = end_position;

            // Reuse the already-loaded data from offsets +6 and +7
            zmm_array_7 = zmm_gate_offset7;
            zmm_array_6 = zmm_gate_offset6;
            zmm_array_5 = _mm512_loadu_si512((__m512i const *)(buff + curr_scan_start + 5));
            zmm_array_4 = _mm512_loadu_si512((__m512i const *)(buff + curr_scan_start + 4));
            zmm_array_3 = _mm512_loadu_si512((__m512i const *)(buff + curr_scan_start + 3));
            zmm_array_2 = _mm512_loadu_si512((__m512i const *)(buff + curr_scan_start + 2));
            zmm_array_1 = _mm512_loadu_si512((__m512i const *)(buff + curr_scan_start + 1));
            zmm_array_0 = _mm512_loadu_si512((__m512i const *)(buff + curr_scan_start));

            // Compare each offset as 64-bit unsigned integers (strictly less than)
            mask_cmp_0 = _mm512_cmplt_epu64_mask(zmm_array_0, zmm_target_8byte);
            mask_cmp_1 = _mm512_cmplt_epu64_mask(zmm_array_1, zmm_target_8byte);
            mask_cmp_2 = _mm512_cmplt_epu64_mask(zmm_array_2, zmm_target_8byte);
            mask_cmp_3 = _mm512_cmplt_epu64_mask(zmm_array_3, zmm_target_8byte);
            mask_cmp_4 = _mm512_cmplt_epu64_mask(zmm_array_4, zmm_target_8byte);
            mask_cmp_5 = _mm512_cmplt_epu64_mask(zmm_array_5, zmm_target_8byte);
            mask_cmp_6 = _mm512_cmplt_epu64_mask(zmm_array_6, zmm_target_8byte);
            mask_cmp_7 = _mm512_cmplt_epu64_mask(zmm_array_7, zmm_target_8byte);

            // Find first match in each offset and calculate position
            if(mask_cmp_0)
                return_pos_0 = curr_scan_start + 8 * (__builtin_ffs(mask_cmp_0) - 1);
            if(mask_cmp_1)
                return_pos_1 = curr_scan_start + 1 + 8 * (__builtin_ffs(mask_cmp_1) - 1);
            if(mask_cmp_2)
                return_pos_2 = curr_scan_start + 2 + 8 * (__builtin_ffs(mask_cmp_2) - 1);
            if(mask_cmp_3)
                return_pos_3 = curr_scan_start + 3 + 8 * (__builtin_ffs(mask_cmp_3) - 1);
            if(mask_cmp_4)
                return_pos_4 = curr_scan_start + 4 + 8 * (__builtin_ffs(mask_cmp_4) - 1);
            if(mask_cmp_5)
                return_pos_5 = curr_scan_start + 5 + 8 * (__builtin_ffs(mask_cmp_5) - 1);
            if(mask_cmp_6)
                return_pos_6 = curr_scan_start + 6 + 8 * (__builtin_ffs(mask_cmp_6) - 1);
            if(mask_cmp_7)
                return_pos_7 = curr_scan_start + 7 + 8 * (__builtin_ffs(mask_cmp_7) - 1);

            // Return the minimum position
            uint64_t min_pos_01 = std::min(return_pos_0, return_pos_1);
            uint64_t min_pos_23 = std::min(return_pos_2, return_pos_3);
            uint64_t min_pos_45 = std::min(return_pos_4, return_pos_5);
            uint64_t min_pos_67 = std::min(return_pos_6, return_pos_7);
            uint64_t min_pos = std::min(std::min(min_pos_01, min_pos_23), std::min(min_pos_45, min_pos_67));
            if(min_pos < end_position){
                return min_pos;
            }
        }
      
        curr_scan_start += INCREMENT_SIZE;
    }

    // Account for remaining tail by scanning serially
    for(uint64_t i = curr_scan_start; i < end_position; i++){
        uint64_t val = *((uint64_t *)(buff + i));
        if(val < target_value){
            return i;
        }
    }

    return end_position;
}

#endif
