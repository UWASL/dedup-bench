#include "hash.hpp"
#include "config.hpp"
#include <iomanip>
#include <sstream>
#include <cstring>

Hash::Hash(HashingTech hashType, unsigned int size, BYTE* hash) noexcept: hashType{hashType}, size{size}, hash{hash}  {}

Hash::Hash(HashingTech hashType, unsigned int size): hashType{hashType}, size{size}, hash{new BYTE[size]}  {}

Hash::Hash(const Hash& other): Hash(other.hashType, other.size) {
    memcpy(this->hash, other.hash, this->size);
}

Hash::Hash(Hash&& other) noexcept: Hash(other.hashType, other.size, other.hash) {
    other.hash = nullptr;
}

Hash::~Hash() {
    delete[] hash;
}

bool Hash::operator==(const Hash& other) const {
    if ((this->hashType != other.hashType) || (this->size != other.size)) {
        return false;
    }
    for (unsigned int i = 0; i < this->size; ++i) {
        if (this->hash[i] != other.hash[i]) {
            return false;
        }
    }
    return true;
}

std::string Hash::toString() const {
    std::stringstream ss;
    ss << std::hex;
    for (unsigned int i = 0; i < this->size; ++i) {
        ss << std::setw(2) << std::setfill('0') << (uint32_t)this->hash[i];
    }
    return ss.str();
}

BYTE* Hash::getHash() const {
    return this->hash;
}