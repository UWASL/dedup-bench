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

        Fixed_Chunking() {
            /**
             * @brief Default constructor. Initializes fixed_chunk_size to DEFAULT_CHUNK_SIZE
             * @return: void
             */
            fixed_chunk_size = DEFAULT_FIXED_CHUNK_SIZE;
            technique_name = "Fixed Chunking";
        }

        Fixed_Chunking(const Config& config){
            /**
             * @brief Constructor with custom fixed_chunk_size
             * @param _chunk_size: Value for fixed_chunk_size
             * @return: void
             */
            fixed_chunk_size = config.get_fc_size();
            technique_name = "Fixed Chunking";
        }

        // Set and Get functions for fixed_chunk_size
        bool set_fixed_chunk_size(uint64_t _chunk_size);
        uint64_t get_fixed_chunk_size();

        // Implementation of chunk_file from Chunking_Technique
        std::vector<File_Chunk> chunk_file(std::string file_path);
        void chunk_stream(std::vector<File_Chunk>& result, std::istream& stream) override;
};

#endif