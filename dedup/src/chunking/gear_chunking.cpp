/**
 * @file gear_chunking.cpp
 * @author WASL
 * @brief Implementations for gear chunking technique
 * @version 0.1
 * @date 2023-4-1
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "gear_chunking.hpp"

#include <iostream>
#include <cstring>
#include <fstream>

Gear_Chunking::Gear_Chunking(const Config& config) {
    min_block_size = config.get_gear_min_block_size();
    max_block_size = config.get_gear_max_block_size();
}

uint64_t Gear_Chunking::ghash(uint64_t h, unsigned char ch) {
    return ((h << 1) + GEAR_TABLE[ch]);
}

uint64_t Gear_Chunking::cut(const std::vector<unsigned char>& data) {
    uint64_t hash = 0;
    uint64_t idx = min_block_size;

    // avarage size 8kb
    uint64_t mask = 0x0000d90303530000;

    // If given data is lower than the minimum chunk size, return data length.
    if (data.size() <= min_block_size) {
        return data.size();
    }

    while (idx < data.size()) {
        hash = ghash(hash, data[idx]);
        if (!(hash & mask)) {
            return idx;
        }
        idx += 1;
    }

    return idx;
}

std::vector<File_Chunk> Gear_Chunking::chop(
    const std::vector<unsigned char>& data) {
    uint64_t ck_start = 0;
    uint64_t ck_end = 0;
    uint64_t ct_idx = data.size();

    std::vector<File_Chunk> chunks;
    
    while (ct_idx != ck_end) {
        ck_end =
            ck_start + cut(std::vector<unsigned char>(data.begin() + ck_start,
                                                      data.begin() + ct_idx));
        File_Chunk new_chunk{ck_end - ck_start };

        memccpy(new_chunk.get_data(), data.data(), ck_start,
                ck_end - ck_start );

        chunks.push_back(new_chunk);

        ck_start = ck_end;
        ct_idx = std::min(ck_end + max_block_size, data.size());

    }
    return chunks;
}

std::vector<File_Chunk> Gear_Chunking::chunk_file(std::string file_path) {
    std::vector<File_Chunk> file_chunks;

    std::vector<unsigned char> remain;
    std::vector<unsigned char> buffer(max_block_size);

    std::ifstream file_ptr(file_path, std::ios::binary);

    while (true) {
        file_ptr.read((char*)buffer.data(), max_block_size);
        uint64_t read_bytes = file_ptr.gcount();

        if (read_bytes == 0) {
            break;
        }

        // prepend remaining data from previous iteration before chopping.
        buffer.insert(buffer.begin(), remain.begin(), remain.end());

        // read data can be lower than actual buffer size.

        uint64_t bf_end = read_bytes + remain.size();

        std::vector<File_Chunk> chunks = chop(std::vector<unsigned char>(
            buffer.begin(), buffer.begin() + bf_end));

        File_Chunk last = chunks.back();
        remain = std::vector<unsigned char>(last.get_data(),
                                            last.get_data() + last.get_size());
        chunks.pop_back();
        for (uint32_t i = 0; i < chunks.size(); i++) {
            file_chunks.push_back(chunks[i]);
        }
        buffer.resize(max_block_size);
    }

    if (remain.size() > 0) {
        std::vector<File_Chunk> chunks = chop(remain);
        for (uint32_t i = 0; i < chunks.size(); i++) {
            file_chunks.push_back(chunks[i]);
        }
    }

    return file_chunks;
}