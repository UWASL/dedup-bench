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
#include <fstream>
#include <filesystem>


File_Chunk::File_Chunk(uint64_t _chunk_size) {
    chunk_size = _chunk_size;
    chunk_data = std::make_unique<char[]>(_chunk_size);
}

File_Chunk::File_Chunk(const File_Chunk& other): File_Chunk(other.chunk_size) {
    memcpy(this->chunk_data.get(), other.chunk_data.get(), this->chunk_size);
    if (other.chunk_hash) {
        this->chunk_hash = std::make_unique<Hash>(*other.chunk_hash);
    }
}

File_Chunk::File_Chunk(File_Chunk&& other) noexcept {
    this->chunk_size = other.chunk_size;
    this->chunk_data = std::move(other.chunk_data);
    this->chunk_hash = std::move(other.chunk_hash);
    // release ownership of the data in the other object
    other.chunk_data.release();
    other.chunk_hash.release();
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
    std::cout << "\tChunk Size: " << chunk_size << std::endl;
    if (chunk_hash) {
        std::cout << "\tChunk Hash: " << chunk_hash->toString() << std::endl;
    }
    std::cout << "\tChunk Data: ";
    for (uint64_t i = 0; i < chunk_size; ++i) {
        // this only works if the data is ASCII
        std::cout << chunk_data[i];
    }
    std::cout << std::endl;
}

// ========== Chunking_Techniques =============
std::vector<std::istringstream> Chunking_Technique::read_files_to_buffers(std::string dir_path) {
    std::vector<std::istringstream> buffers;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(dir_path)) {
        std::filesystem::path file_path = entry.path();
        if (std::filesystem::is_directory(file_path)) {
            continue;
        }

        std::ifstream file_ptr;
        file_ptr.open(file_path, std::ios::in);
        // get length of file:
        file_ptr.seekg (0, std::ios::end);
        long length = file_ptr.tellg();
        file_ptr.seekg (0, std::ios::beg);
        // allocate memory:
        char *buffer = new char[length + 1];
        // read data from file into the buffer
        file_ptr.read(buffer, length);
        buffer[length] = '\0';
        // create string stream from the buffer (string will create a copy of the buffer)
        buffers.emplace_back(std::string{buffer});
        // cleanup
        delete[] buffer;
        file_ptr.close();
    }
    return buffers;
}