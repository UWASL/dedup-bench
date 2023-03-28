/**
 * @file chunking_common.cpp
 * @author WASL
 * @brief Implementations of functions common across all chunking techniques
 * @version 0.1
 * @date 2023-02-02
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "chunking_common.hpp"
#include <memory>
#include <cstring>
#include <iostream>
#include <string>


File_Chunk::File_Chunk(uint32_t _chunk_size) {
    chunk_size = _chunk_size;
    chunk_data = std::make_unique<char[]>(_chunk_size);
}

File_Chunk::File_Chunk(const File_Chunk& other): File_Chunk(other.chunk_size) {
    memcpy(this->chunk_data.get(), other.chunk_data.get(), this->chunk_size);
}

File_Chunk::File_Chunk(File_Chunk&& other) noexcept {
    this->chunk_size = other.chunk_size;
    this->chunk_data = std::move(other.chunk_data);
    // release ownership of the chunk data
    other.chunk_data.release();
}

uint64_t File_Chunk::get_size() const {
    return chunk_size;
}

char* File_Chunk::get_data() const {
    return chunk_data.get();
}

BYTE* File_Chunk::get_hash() const {
    if (chunk_hash) {
        return chunk_hash->getHash();
    }
    return nullptr;
}

void File_Chunk::init_hash(HashingTech hashing_tech, uint64_t size) {
    chunk_hash = std::make_unique<Hash>(hashing_tech, size);
}

std::string File_Chunk::to_string() const {
    if (chunk_hash) {
        return chunk_hash->toString() + "," + std::to_string(chunk_size);
    }
    return "INVALID HASH";
}

void File_Chunk::print() const {
    std::cout << "\tChunk Size: " <<  chunk_size << std::endl;
    std::cout << "\tChunk Hash: " << chunk_hash->toString() << std::endl;
    std::cout << "\tChunk Data: " << chunk_data << std::endl;
}