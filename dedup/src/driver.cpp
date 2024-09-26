/**
 * @file driver.cpp
 * @author WASL
 * @brief Driver program for dedup
 * @version 0.1
 * @date 2023-01-31
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <ios>
#include <memory>
#include <sstream>
#include <tuple>
#include <vector>

#include "chunking_common.hpp"
#include "config.hpp"
#include "config_error.hpp"

#include "ae_chunking.hpp"
#include "fastcdc.hpp"
#include "fixed_chunking.hpp"
#include "gear_chunking.hpp"
#include "rabins_chunking.hpp"
#include "ram_chunking.hpp"
#include "crc_chunking.hpp"
#include "seq_chunking.hpp"
#include "tttd_chunking.hpp"

#include "md5_hashing.hpp"
#include "sha1_hashing.hpp"
#include "sha256_hashing.hpp"
#include "sha512_hashing.hpp"

bool disable_hashing = false;

static void driver_function(const std::filesystem::path& dir_path,
                            std::unique_ptr<Chunking_Technique>& chunk_method,
                            const std::string& output_file) {
    /**
     * @brief Uses the specified chunking technique to chunk the file, hash it
     * using the specified hashing technique and print the hashes
     * @param chunk_method: Chunking Technique Object. Object from a class
     * inheriting the Chunking_Technique interface.
     * @param hash_method: Hash Technique Object. Object from a class inheriting
     * the Hashing_Technique interface.
     * @return: void
     *
     */
    const std::string delimiter = ", ";
    uint64_t chunk_count = 0;
    uint64_t total_bytes = 0;
    
    if (!std::filesystem::is_directory(dir_path)) {
        std::cerr << dir_path << " is not a directory" << std::endl;
        return;
    }

    // open the output file for writing the hashes to
    std::ofstream out_file(output_file, std::ios::out | std::ios::trunc);

    if (!out_file.is_open()) {
        std::cerr << "Failed to open " << output_file << " for writing"
                  << std::endl;
        return;
    }

    for (const auto& entry :
        std::filesystem::recursive_directory_iterator(dir_path)) {
        std::filesystem::path file_path = entry.path();
        if (std::filesystem::is_directory(file_path)) {
            // Avoid trying to chunk a directory path
            continue;
        }

        std::ifstream file_ptr;
        file_ptr.open(file_path, std::ios::in);
        if (!file_ptr.is_open()) {
            std::cerr << "Failed to open " << file_path << " for reading"
                      << std::endl;
            continue;
        }
        // Chunk file using specified Chunking_Technique
        std::vector<std::string> hashes =
            chunk_method->chunk_file(file_path);
        // get the size of the file
        total_bytes += chunk_method->get_file_size(&file_ptr);
        chunk_count += hashes.size();

        for (const auto& hash : hashes) {
            out_file << hash << std::endl;
            // fc.print();
        }
    }

    out_file.close();
    uint64_t total_mb = total_bytes / (1024*1024);
    double total_seconds_chunking =  chunk_method->total_time_chunking.count() /1000;
    double total_seconds_hashing =  chunk_method->total_time_hashing.count() /1000;
     // Print stats
    std::cout << "Total number of chunks: " << chunk_count << std::endl;
    std::cout << "Total bytes chunked: " << total_bytes << std::endl;
    std::cout << "Avg Chunk size: " << total_bytes / chunk_count
              << std::endl;

    std::cout << "Chunking Throughput (MB/sec): "
              << total_mb / total_seconds_chunking << std::endl;
    std::cout << "Hashing Throughput (MB/sec): "
              << total_mb / total_seconds_hashing << std::endl;
}

int main(int argc, char* argv[]) {
    /**
     * @brief Main entry point for dedup.exe. Parses arguments and calls
     * driver_function()
     * @todo: Add Config class which takes in parameters
     */
    if (argc > 4 || argc < 3) {
        std::cout << "Usage: ./dedup.exe <file_path> <config_file_path> [bool]"
                  << std::endl;
        std::cout
            << "\t  <file_path>: Path to file to run chunking and hashing on."
            << std::endl;
        std::cout << "\t  <config_file_path>: Path to the config file."
                  << std::endl;
        std::cout << "\t  [bool]: t or f indicating whether to only perform "
                     "chunking. {f}"
                  << std::endl;

        exit(EXIT_FAILURE);
    }

    if (argc == 4) {
        std::string bool_val = std::string(argv[3]);
        if (bool_val != "t" && bool_val != "f") {
            std::cerr << bool_val << " is not valid. Must be either 't' or 'f'"
                      << std::endl;
            exit(EXIT_FAILURE);
        }
        if (bool_val == "t") {
            disable_hashing = true;
        }
    }

    std::string dir_path = std::string(argv[1]);
    std::string output_file;
    try {
        Config config{std::string(argv[2])};
        ChunkingTech chunking_technique = config.get_chunking_tech();
        HashingTech hashing_technique = config.get_hashing_tech();
        output_file = config.get_output_file();
        
        std::unique_ptr<Chunking_Technique> chunk_method;

        // Set parameters for hashing technique and call relevant constructors
        switch (chunking_technique) {
            case ChunkingTech::FIXED:
                chunk_method = std::make_unique<Fixed_Chunking>(config);
                break;
            case ChunkingTech::RABINS:
                chunk_method = std::make_unique<Rabins_Chunking>(config);
                break;
            case ChunkingTech::AE:
                chunk_method = std::make_unique<AE_Chunking>(config);
                break;
            case ChunkingTech::GEAR:
                chunk_method = std::make_unique<Gear_Chunking>(config);
                break;
            case ChunkingTech::FASTCDC:
                chunk_method = std::make_unique<FastCDC>(config);
                break;
            case ChunkingTech::RAM:
                chunk_method = std::make_unique<RAM_Chunking>(config);
                break;
            case ChunkingTech::CRC:
                chunk_method = std::make_unique<Crc_Chunking>(config);
                break;
            case ChunkingTech::SEQ:
                chunk_method = std::make_unique<Seq_Chunking>(config);
                break;
            case ChunkingTech::TTTD:
                chunk_method = std::make_unique<TTTD_Chunking>(config);
                break;
            default:
                std::cerr << "Unimplemented chunking technique" << std::endl;
                exit(EXIT_FAILURE);
        }
        if (!disable_hashing) {
            switch (hashing_technique) {
                case HashingTech::MD5:
                    chunk_method -> hash_method = std::make_unique<MD5_Hashing>();
                    break;
                case HashingTech::SHA1:
                    chunk_method -> hash_method = std::make_unique<SHA1_Hashing>();
                    break;
                case HashingTech::SHA256:
                    chunk_method -> hash_method = std::make_unique<SHA256_Hashing>();
                    break;
                case HashingTech::SHA512:
                    chunk_method -> hash_method = std::make_unique<SHA512_Hashing>();
                    break;
                default:
                    std::cerr << "Unimplemented hashing technique" << std::endl;
                    exit(EXIT_FAILURE);
            }
        }
        //set buffer size 
        chunk_method -> stream_buffer_size = config.get_buffer_size();

        // Call driver function
        driver_function(dir_path, chunk_method, output_file);
        // driver_function_stream(dir_path, chunk_method, hash_method, output_file);
    } catch (const ConfigError& e) {
        std::cerr << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
