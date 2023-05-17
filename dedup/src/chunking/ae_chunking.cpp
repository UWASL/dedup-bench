/**
 * @file ae_chunking.cpp
 * @author WASL
 * @brief Implementations for AE chunking technique
 * @version 0.1
 * @date 2023-3-10
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <fstream>
#include "ae_chunking.hpp"

AE_Chunking::AE_Chunking() {
    avg_block_size = DEFAULT_AE_AVG_BLOCK_SIZE;
    extreme_mode = MAX;
    technique_name = "AE Chunking";
    read_buffer = new char[BUFFER_SIZE];
}

AE_Chunking::AE_Chunking(const Config& config) {
    extreme_mode = config.get_ae_extreme_mode();
    avg_block_size = config.get_ae_avg_block_size();
    window_size = avg_block_size / (exp(1) - 1);  // avg_block size / e-1
    read_buffer = new char[BUFFER_SIZE];

    technique_name = "AE Chunking";
}

AE_Chunking::~AE_Chunking() {
    delete[] read_buffer;
}

bool AE_Chunking::is_extreme(uint64_t new_val, uint64_t current_extr) {
    if (extreme_mode == MAX) {
        return new_val > current_extr;
    } else {
        return new_val < current_extr;
    }
}

uint64_t AE_Chunking::find_cutpoint(char* buff, uint64_t size) {
    uint32_t i = 0;
    int max_value = buff[i];
    int max_pos = i;
    i++;
    while (i < size) {
        if (!is_extreme(buff[i], max_value)) {
            if (i == max_pos + window_size) {
                return i;
            }
        } else {
            max_value = buff[i];
            max_pos = i;
        }
        i++;
    }
    return size;
}


void AE_Chunking::chunk_stream(std::vector<File_Chunk>& result, std::istream& stream) {
    // uint64_t read_buff_end = 0;

    // uint64_t file_size_bytes = get_file_size(stream);
    // uint64_t bytes_to_read =
    //     std::min((uint64_t)BUFFER_SIZE, file_size_bytes);
    // uint64_t curr_bytes_read = 0;
    // while (curr_bytes_read < file_size_bytes) {
    //     stream.read(&read_buffer[read_buff_end], bytes_to_read);
    //     // mark the end of logical buffer
    //     read_buff_end += stream.gcount() - 1;
    //     // find cutpoint
    //     uint32_t cutpoint = find_cutpoint(read_buffer, read_buff_end+1);
    //     // create new chunk and push it to the vector
    //     uint32_t chunk_size = cutpoint + 1;
    //     File_Chunk new_chunk{chunk_size};
    //     memccpy(new_chunk.get_data(), read_buffer, 0, chunk_size);
    //     result.emplace_back(std::move(new_chunk));

    //     /* there's a partial block at the end
    //         * of the buffer; move it to the beginning of the buffer
    //         * so we can append more from input stream
    //         */
    //     memmove(read_buffer, &read_buffer[cutpoint + 1],
    //             read_buff_end - cutpoint);
    //     curr_bytes_read += stream.gcount();
    //     read_buff_end -= cutpoint;
    //     // Handles the last chunk being smaller than buffer size
    //     bytes_to_read = std::min(BUFFER_SIZE - read_buff_end,
    //                                 file_size_bytes - curr_bytes_read);
    // }
    // // file fully loaded into the buffer
    // // chunk the rest of the buffer
    // int pos = 0;
    // while ((int)read_buff_end > pos) {
    //     uint32_t cutpoint =
    //         find_cutpoint(&read_buffer[pos], read_buff_end - pos);
    //     uint32_t chunk_size = cutpoint + 1;
    //     File_Chunk new_chunk{chunk_size};
    //     memccpy(new_chunk.get_data(), &read_buffer[pos], 0, chunk_size);
    //     result.emplace_back(std::move(new_chunk));
    //     pos += chunk_size;
    // }
    return;
}
