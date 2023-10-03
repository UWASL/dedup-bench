#ifndef _SHA512_HASHING_
#define _SHA512_HASHING_

#include "hashing_common.hpp"


class SHA512_Hashing: public virtual Hashing_Technique{
    /**
     * @brief Class to implement SHA512 Hashing
     *
     */
    public:
        // Function to hash a given chunk
        void hash_chunk(File_Chunk& file_chunk) override;

        SHA512_Hashing() {
            technique_name = "SHA512-Hashing";
        }
};

#endif
