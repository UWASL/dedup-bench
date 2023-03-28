#ifndef _HASH_
#define _HASH_

#include <string>
#include "config.hpp"

typedef unsigned char BYTE;

class Hash {
    const HashingTech hashType;
    const unsigned int size;
    // byte array to hold the hash value
    BYTE* hash;

    /**
     * @brief Private constructor used by the move constructor
     * 
     * @param hashType 
     * @param size 
     * @param hash 
     */
    Hash(HashingTech hashType, unsigned int size, BYTE* hash) noexcept;

    public:
        Hash(HashingTech hashType, unsigned int size);

        Hash(const Hash& other);

        Hash& operator=(const Hash& other) = delete;

        Hash(Hash&& other) noexcept;

        Hash& operator=(Hash&& other) noexcept = delete;

        ~Hash();

        bool operator==(const Hash& other) const;

        /**
         *  @brief return the hash as a hex string
         * 
         *  @return std::string
        */
        std::string toString() const;

        /**
         *  @brief return the underlying byte array for the hash
         * 
         *  @return BYTE*
        */
        BYTE* getHash() const;
};

#endif