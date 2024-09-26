#ifndef _RAM_Chunking_
#define _RAM_Chunking_

#include <math.h>
#include <iostream>

#include "chunking_common.hpp"
#include "config.hpp"

#include <cstring>

#define DEFAULT_RAM_AVG_BLOCK_SIZE 4096

class RAM_Chunking : public virtual Chunking_Technique {
   private:
    uint64_t avg_block_size;
    uint64_t max_block_size;
    uint64_t window_size;
    uint64_t curr_pos;

    /**
     * @brief finds the next cut point in an array of bytes
     * @param buff: the buff to find the cutpoint in.
     * @param size: the size of the buffer
     * @return: cutpoint position in the buffer 
     */
    uint64_t find_cutpoint(char* buff, uint64_t size);
    
   public:
    /**
     * @brief Default constructor.
     * @return: void
     */
    RAM_Chunking();

    /**
     * @brief Constructor with custom config from a config object
     * @param config: the config object
     * @return: void
     */
    RAM_Chunking(const Config& config);

    ~RAM_Chunking();

};

#endif