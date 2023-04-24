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

#include <cmath>
#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>

Gear_Chunking::Gear_Chunking(const Config& config) {
    min_block_size = config.get_gear_min_block_size();
    max_block_size = config.get_gear_max_block_size();
    avg_block_size = config.get_gear_avg_block_size();

    int number_of_ones = log2(avg_block_size);

    // to produce chunks that are 8KB. Using the
    // formula log2(8192) = 13, we can use a mask with the 13 most-significant
    // bits set to 1
    mask = 0x0000000000000000;
    for (int i = 0; i < 64; i++) {
        if (number_of_ones-- >= 0) {
            mask += 1;
        }
        mask = mask << 1;
    }
    // std::cout << mask;
}

uint64_t Gear_Chunking::ghash(uint64_t h, unsigned char ch) {
    return ((h << 1) + GEAR_TABLE[ch]);
}

uint64_t Gear_Chunking::cut(const char* data, size_t size) {
    uint64_t hash = 0;
    uint64_t idx = min_block_size;

    // If given data is lower than the minimum chunk size, return data length.
    if (size <= min_block_size) {
        return size;
    }

    while (idx < size && idx < max_block_size) {
        hash = ghash(hash, data[idx]);
        if (!(hash & mask)) {
            // std::cout << data[idx] <<std::endl;
            return idx;
        }
        idx += 1;
    }

    return idx;
}

std::vector<File_Chunk> Gear_Chunking::chop(const char* data, size_t size) {
    uint64_t ck_start = 0;
    uint64_t ck_end = 0;
    uint64_t ct_idx = size;

    std::vector<File_Chunk> chunks;

    while (ck_end != ct_idx) {
        ck_end =
            ck_start + cut(data + ck_start, ct_idx - ck_start);

        
        File_Chunk new_chunk{ck_end - ck_start};

        memcpy(new_chunk.get_data(), data+ck_start, ck_end - ck_start);

        chunks.push_back(new_chunk);

        ck_start = ck_end;
        // casting size_t to uint64_t which may be an issue if size_t is larger
        // than what uint64_t can hold
        ct_idx = std::min(ck_end + max_block_size, (uint64_t)size);
        // std::cout << ck_end <<std::endl; 
    }
    return chunks;
}

std::vector<File_Chunk> Gear_Chunking::chunk_file(std::string file_path) {
    std::vector<File_Chunk> file_chunks;

    const uint64_t BUFFER_SIZE = 2 * max_block_size;
    std::unique_ptr<char[]> buffer = std::make_unique<char[]>(BUFFER_SIZE);
    char* const ptr = buffer.get();
    size_t prev_num_bytes_chunked = 0;
    size_t prev_remain_size = 0;

    std::ifstream file_ptr(file_path, std::ios::binary);

    while (true) {
        // prepend remaining data from previous iteration before chopping.
        memcpy(ptr, ptr + prev_num_bytes_chunked, prev_remain_size);

        char* start = ptr + prev_remain_size;
        file_ptr.read(start, BUFFER_SIZE - prev_remain_size);
        uint64_t read_bytes = file_ptr.gcount();
        if (read_bytes == 0) {
            break;
        }

        std::vector<File_Chunk> chunks =
            chop(ptr, prev_remain_size + read_bytes);

        File_Chunk last = chunks.back();
        prev_num_bytes_chunked = prev_remain_size + read_bytes - last.get_size();
        prev_remain_size = last.get_size();
        // remove last chunk because it is incomplete chunk
        chunks.pop_back();

        // append the proper chunks
        for (uint32_t i = 0; i < chunks.size(); i++) {
            file_chunks.emplace_back(std::move(chunks[i]));
        }
    }

    // add the very last chunk
    if (prev_remain_size > 0) {
        File_Chunk last = File_Chunk{prev_remain_size};
        memcpy(last.get_data(), ptr, prev_remain_size);
        file_chunks.emplace_back(std::move(last));
    }

    uint64_t sum = 0;

    // std::cout << "sum : " <<sum << std::endl; 
    return file_chunks;
}

void Gear_Chunking::chunk_stream(std::vector<File_Chunk>& result,
                                 std::istream& stream) {
    return;

    // std::vector<unsigned char> remain;
    // std::vector<unsigned char> buffer(max_block_size);

    // std::ifstream file_ptr(file_path, std::ios::binary);

    // while (true) {
    //     file_ptr.read((char*)buffer.data(), max_block_size);
    //     uint64_t read_bytes = file_ptr.gcount();

    //     if (read_bytes == 0) {
    //         break;
    //     }

    //     // prepend remaining data from previous iteration before chopping.
    //     buffer.insert(buffer.begin(), remain.begin(), remain.end());

    //     // read data can be lower than actual buffer size.

    //     uint64_t bf_end = read_bytes + remain.size();

    //     std::vector<File_Chunk> chunks = chop(std::vector<unsigned char>(
    //         buffer.begin(), buffer.begin() + bf_end));

    //     File_Chunk last = chunks.back();
    //     remain = std::vector<unsigned char>(last.get_data(),
    //                                         last.get_data() +
    //                                         last.get_size());
    //     chunks.pop_back();
    //     for (uint32_t i = 0; i < chunks.size(); i++) {
    //         file_chunks.push_back(chunks[i]);
    //     }
    //     buffer.resize(max_block_size);
    // }

    // if (remain.size() > 0) {
    //     std::vector<File_Chunk> chunks = chop(remain);
    //     for (uint32_t i = 0; i < chunks.size(); i++) {
    //         file_chunks.push_back(chunks[i]);
    //     }
    // }

    // return file_chunks;
}
