#ifndef _SHA1_HASHING_
#define _SHA1_HASHING_

#include "hashing_common.hpp"


class SHA1_Hashing: public virtual Hashing_Technique{
    /**
     * @brief Class to implement SHA1 Hashing
     * 
     */
    public:
        // Function to hash a given chunk
        std::string hash_chunk(File_Chunk file_chunk);

        SHA1_Hashing() {
            technique_name = "SHA1-Hashing";
        }
};

#endif