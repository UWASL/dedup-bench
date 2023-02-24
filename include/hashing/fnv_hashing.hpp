#ifndef _FNV_HASHING_
#define _FNV_HASHING_

#include "hashing_common.hpp"


class Fnv_Hashing: public virtual Hashing_Technique{
    /**
     * @brief Class to implement FNV (Fowler-No-Voll) Hashing
     * 
     */
    private:
        /**
         * These are algorithmic constants for FNV hashing and must not be modified
         * 
         */
        static const uint64_t fnv_offset_basis = 0xcbf29ce484222325;
        static const uint64_t fnv_prime = 0x100000001b3;
    public:
        // Function to hash a given chunk
        std::string hash_chunk(File_Chunk file_chunk) override;

        Fnv_Hashing(){
            technique_name = "FNV-Hashing";
        }
};

#endif