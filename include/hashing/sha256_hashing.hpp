#ifndef _SHA256_HASHING_
#define _SHA256_HASHING_

#include "hashing_common.hpp"


class SHA256_Hashing: public virtual Hashing_Technique{
    /**
     * @brief Class to implement SHA256 Hashing
     * 
     */
    public:
        // Function to hash a given chunk
        void hash_chunk(File_Chunk& file_chunk) override;

        SHA256_Hashing() {
            technique_name = "SHA256-Hashing";
        }
};

#endif