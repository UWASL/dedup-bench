#ifndef _SEQ_CHUNKING_
#define _SEQ_CHUNKING_

#include <math.h>
#include <iostream>

#include "chunking_common.hpp"
#include "config.hpp"

#include <cstring>

#define DEFAULT_SEQ_LENGTH 5
#define DEFAULT_JUMP_TRIGGER 50

#define DEFAULT_SEQ_JUMP_SIZE 256

#define DEFAULT_SEQ_MIN_BLOCK_SIZE 4096
#define DEFAULT_SEQ_AVG_BLOCK_SIZE 8192
#define DEFAULT_SEQ_MAX_BLOCK_SIZE 16384

#define DEFAULT_SEQ_OP_MODE INCREASING;


class Seq_Chunking : public virtual Chunking_Technique {
   private:
    uint64_t seq_length_threshold;
    uint64_t jump_trigger;
    uint64_t jump_size;

    Seq_Op_Mode op_mode;
    
    uint64_t min_block_size;
    uint64_t avg_block_size;
    uint64_t max_block_size;
   
    /**
     * @brief finds the next cut point in an array of bytes
     * @param buff: the buff to find the cutpoint in.
     * @param size: the size of the buffer
     * @return: cutpoint position in the buffer 
     */
    uint64_t find_cutpoint(char* buff, uint64_t size) override;
    uint64_t find_cutpoint_decreasing(char *buff, uint64_t size);
    uint64_t find_cutpoint_increasing(char *buff, uint64_t size);

   public:
    /**
     * @brief Default constructor.
     * @return: void
     */
    Seq_Chunking();

    /**
     * @brief Constructor with custom config from a config object
     * @param config: the config object
     * @return: void
     */
    Seq_Chunking(const Config& config);

    ~Seq_Chunking();

};

#endif