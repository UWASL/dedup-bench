#ifndef _MD5_HASHING_
#define _MD5_HASHING_

#include "hashing_common.hpp"


class MD5_Hashing: public virtual Hashing_Technique{
    /**
     * @brief Class to implement MD5 Hashing
     * 
     */
    public:
        // Function to hash a given chunk
        void hash_chunk(File_Chunk& file_chunk) override;

        MD5_Hashing() {
            technique_name = "MD5-Hashing";
        }
};

#endif