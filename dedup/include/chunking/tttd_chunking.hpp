#ifndef _TTTD_CHUNKING_
#define _TTTD_CHUNKING_

#include <math.h>
#include <iostream>

#include "chunking_common.hpp"
#include "rabins_chunking.hpp"
#include "config.hpp"

#include <cstring>

class TTTD_Chunking : public virtual Rabins_Chunking{
    private:
        uint64_t rabin_mask, backup_mask;

        uint64_t avg_block_size, min_block_size, max_block_size;
    public:

    uint64_t find_cutpoint(char* buff, uint64_t size) override;

    //  TTTD_Chunking();

     TTTD_Chunking(const Config & config);

     ~TTTD_Chunking();
};

#endif