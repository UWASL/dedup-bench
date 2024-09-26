/**
 * @file seq_chunking.cpp
 * @author WASL
 * @brief Implementations for sequential chunking technique with thresholding
 * @version 0.1
 * @date 2023-3-10
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <fstream>
#include "seq_chunking.hpp"

Seq_Chunking::Seq_Chunking() {
    seq_length_threshold = DEFAULT_SEQ_LENGTH;
    jump_trigger = DEFAULT_JUMP_TRIGGER;

    jump_size = DEFAULT_SEQ_JUMP_SIZE;
    op_mode = DEFAULT_SEQ_OP_MODE;

    min_block_size = DEFAULT_SEQ_MIN_BLOCK_SIZE;
    avg_block_size = DEFAULT_SEQ_AVG_BLOCK_SIZE;
    max_block_size = DEFAULT_SEQ_MAX_BLOCK_SIZE;
    
    technique_name = "Seq Chunking";
}

Seq_Chunking::Seq_Chunking(const Config& config) {
    seq_length_threshold = config.get_seq_threshold();
    jump_trigger = config.get_seq_jump_trigger();
    
    jump_size = config.get_seq_jump_size();
    op_mode = config.get_seq_op_mode();

    min_block_size = config.get_seq_min_block_size();
    avg_block_size = config.get_seq_avg_block_size();
    max_block_size = config.get_seq_max_block_size();
   
    
    technique_name = "Seq Chunking";
}

Seq_Chunking::~Seq_Chunking() {

}

uint64_t Seq_Chunking::find_cutpoint_increasing(char *buff, uint64_t size){
    uint64_t curr_pos = min_block_size;
    uint16_t opposing_slope_count = 0; 
    uint16_t curr_seq_length = 0;

    int cmp_result = 0;
    bool cmp_sign = false;

     while(curr_pos < size){

                // Compare current and previous byte
                cmp_result = (uint8_t)buff[curr_pos] - (uint8_t)buff[curr_pos - 1];
                curr_pos++;

                // Low Entropy Absorption
                if(cmp_result == 0)
                    continue;

                // Get result of comparison (signbit is true if negative)
                cmp_sign = signbit(cmp_result);

                // Opposing slope is only incremented when curr_byte < prev_byte i.e. cmp_result is negative i.e. cmp_sign == 1
                opposing_slope_count += cmp_sign;

                // Curr seq length is incremented only when cmp_sign == 0
                curr_seq_length = (curr_seq_length * !cmp_sign) + !cmp_sign;


                // Return chunk boundary if sequence threshold reached
                if(curr_seq_length == seq_length_threshold)
                    return curr_pos - 1;
                // Jump skip_size bytes if jump_trigger hit
                else if(opposing_slope_count == jump_trigger){
                    curr_pos += jump_size;
                    opposing_slope_count = 0;
                }
     
     }

    return size;    
}

uint64_t Seq_Chunking::find_cutpoint_decreasing(char *buff, uint64_t size){
    uint64_t curr_pos = min_block_size;
    uint16_t opposing_slope_count = 0; 
    uint16_t curr_seq_length = 0;

    int cmp_result = 0;
    bool cmp_sign = false;  

       while(curr_pos < size){

                // Compare current and previous byte
                cmp_result = (uint8_t)buff[curr_pos] - (uint8_t)buff[curr_pos - 1];
                curr_pos++;

                // Low Entropy Absorption
                if(cmp_result == 0)
                    continue;

                // Get result of comparison (signbit is true if negative)
                // Note that signbit is negated here to invert all comparisons
                cmp_sign = !(signbit(cmp_result));

                // Opposing slope is only incremented when curr_byte > prev_byte
                opposing_slope_count += cmp_sign;

                // Curr seq length is incremented only when cmp_sign == 0
                curr_seq_length = (curr_seq_length * !cmp_sign) + !cmp_sign;


                // Return chunk boundary if sequence threshold reached
                if(curr_seq_length == seq_length_threshold)
                    return curr_pos - 1;
                // Jump skip_size bytes if jump_trigger hit
                else if(opposing_slope_count == jump_trigger){
                    curr_pos += jump_size;
                    opposing_slope_count = 0;
                }
       }
     

    return size;

}

uint64_t Seq_Chunking::find_cutpoint(char* buff, uint64_t size) {
   
    if(size < min_block_size){
        return size;
    }
        
    if(size > max_block_size)
        size = max_block_size;

    switch(op_mode){

        // Seq in INCREASING operation mode
        case Seq_Op_Mode::INCREASING: 
            return find_cutpoint_increasing(buff, size);
            
        // Seq in DECREASING operation mode
        case Seq_Op_Mode::DECREASING:
            return find_cutpoint_decreasing(buff, size);
        
        default:
            return size;
    }
}

