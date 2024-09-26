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
}

AE_Chunking::AE_Chunking(const Config& config) {
    extreme_mode = config.get_ae_extreme_mode();
    avg_block_size = config.get_ae_avg_block_size();
    window_size = avg_block_size - 256;

    // window_size = avg_block_size / (exp(1) - 1);  // avg_block size / e-1

    technique_name = "AE Chunking";
}

AE_Chunking::~AE_Chunking() {
}

uint64_t AE_Chunking::find_cutpoint(char* buff, uint64_t size) {
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

