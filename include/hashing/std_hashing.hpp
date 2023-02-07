#ifndef _STD_HASHING_
#define _STD_HASHING_

#include "hashing_common.hpp"

class Std_Hashing: public virtual Hashing_Technique{
    public:
        // Hash chunk using standard hashing from CPP std library
        std::string hash_chunk(File_Chunk file_chunk);

        Std_Hashing(){
            technique_name = "Std-Hashing";
        }
};

#endif