#ifndef _Crc_Chunking_
#define _Crc_Chunking_

#include "chunking_common.hpp"
#include "config.hpp"
#include "crc32c_internal.h"


class Crc_Chunking : public virtual Chunking_Technique {
    /**
     * @brief Class implementing crc's based chunking
     *
     */

   private:
    uint64_t min_block_size;
    uint64_t max_block_size;
    uint64_t avg_block_size;
    uint64_t mask;

    uint64_t window_size;
    uint64_t window_step;

    /**
     * @brief finds the next cut point in an array of bytes
     * @param buff: the buff to find the cutpoint in.
     * @param size: the size of the buffer
     * @return: cutpoint position in the buffer
     */
    uint64_t find_cutpoint(char* buff, uint64_t size) override;

   public:
    /**
     * @brief Default constructor. defines all parameters to defualt values
     * @return: void
     */
    Crc_Chunking();

    /**
     * @brief Defines all parameters based on values from the config file
     * @return: void
     */
    Crc_Chunking(const Config& config);
};

#endif