#ifndef _FIXED_CHUNKING_
#define _FIXED_CHUNKING_

#include "chunking_common.hpp"
#include "config.hpp"
#include <istream>

#define DEFAULT_FIXED_CHUNK_SIZE 4096

class Fixed_Chunking: public virtual Chunking_Technique{
    /**
     * @brief Class to hold all supported chunking techniques
     * 
     */

    private:
        uint64_t fixed_chunk_size;

    public:
        /**
        * @brief Default constructor. Initializes fixed_chunk_size to DEFAULT_CHUNK_SIZE
        * @return: void
        */
        Fixed_Chunking() {
            fixed_chunk_size = DEFAULT_FIXED_CHUNK_SIZE;
            technique_name = "Fixed Chunking";
        }

        /**
        * @brief Constructor with custom fixed_chunk_size
        * @param _chunk_size: Value for fixed_chunk_size
        * @return: void
        */
        Fixed_Chunking(const Config& config){
            fixed_chunk_size = config.get_fc_size();
            technique_name = "Fixed Chunking";
        }

        /**
        * @brief finds the next cut point in an array of bytes
        * @param buff: the buff to find the cutpoint in.
        * @param size: the size of the buffer
        * @return: cutpoint position in the buffer 
        */

        uint64_t find_cutpoint(char* buff, uint64_t size) override;

        // Set and Get functions for fixed_chunk_size
        bool set_fixed_chunk_size(uint64_t _chunk_size);
        
        uint64_t get_fixed_chunk_size();
        
};

#endif