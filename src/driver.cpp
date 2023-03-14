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

#include "fixed_chunking.hpp"
#include "sha1_hashing.hpp"
#include "sha256_hashing.hpp"
#include "md5_hashing.hpp"
#include "config.hpp"
#include "config_error.hpp"
#include "chunking_common.hpp"
#include "rabins_chunking.hpp"

#include <ios>
#include <fstream>
#include <memory>
#include <vector>
#include <tuple>
#include <filesystem>


static void driver_function(
    const std::filesystem::path& dir_path, Chunking_Technique *chunk_method, std::unique_ptr<Hashing_Technique>& hash_method,
    const std::string& output_file) {
    /**
     * @brief Uses the specified chunking technique to chunk the file, hash it using the specified hashing technique 
     *        and print the hashes
     * @todo: Change this to iterate over all files in directory
     * @todo: Change the function to write hashes to file. Supporting function already exists in hashing_common.cpp
     * 
     * @param chunk_method: Chunking Technique Object. Object from a class inheriting the Chunking_Technique interface.
     * @param hash_method: Hash Technique Object. Object from a class inheriting the Hashing_Technique interface.
     * @return: void
     * 
     */
    const std::string delimiter = ", ";
    uint64_t chunk_count = 0;
    
    if (!std::filesystem::is_directory(dir_path)) {
        std::cerr << dir_path << " is not a directory" << std::endl;
        return;
    }

    // open the output file for writing the hashes to
    std::ofstream out_file(output_file, std::ios::out | std::ios::trunc);
    if (!out_file.is_open()) {
        std::cerr << "Failed to open " << output_file << " for writing" << std::endl;
        return;
    }

    for (const auto& entry : std::filesystem::recursive_directory_iterator(dir_path)) {
        std::filesystem::path file_path = entry.path();
        if (std::filesystem::is_directory(file_path)) {
            // Avoid trying to chunk a directory path
            continue;
        }
        std::cout << "Chunking " << file_path << " ..." << std::endl;

        // Chunk file using specified Chunking_Technique
        std::vector<File_Chunk> file_chunks = chunk_method->chunk_file(file_path);
        chunk_count += file_chunks.size();

        // Hash chunks using specified Hashing_Technique
        std::vector<std::tuple<Hash, uint64_t>> hash_list = hash_method->hash_chunks(file_chunks);

        for (const std::tuple<Hash, uint64_t> tup: hash_list){
            const Hash hash = std::get<0>(tup);
            uint64_t chunk_size = std::get<1>(tup);
            out_file << hash.toString() << delimiter << chunk_size << std::endl;
        }

        // Cleanup chunk memory
        cleanup_chunks(file_chunks);
    }

    out_file.close();
    std::cout << "Total number of chunks: " << chunk_count << std::endl;
}


int main(int argc, char * argv[]){
/**
 * @brief Main entry point for dedup.exe. Parses arguments and calls driver_function()
 * @todo: Add Config class which takes in parameters
 */
    if(argc != 3){
        std::cout << "Usage: ./dedup.exe <file_path> <config_file_path>" << std::endl;
        std::cout << "\t  <file_path>: Path to file to run chunking and hashing on." << std::endl;
        std::cout << "\t  <config_file_path>: Path to the config file." << std::endl;

        exit(EXIT_FAILURE);
    }

    std::string dir_path = std::string(argv[1]);
    std::string output_file;
    try {
        Config config{std::string(argv[2])};
        ChunkingTech chunking_technique = config.get_chunking_tech();
        HashingTech hashing_technique = config.get_hashing_tech();
        output_file = config.get_output_file();

        // Pointers used to hold derived instances of Chunking_Technique and Hashing_Technique
        /**
         * @todo: Change this to use RAII instead to avoid possible memory leak
         * 
         */
        Chunking_Technique *chunk_method = nullptr;
        std::unique_ptr<Hashing_Technique> hash_method;
        
        // Set parameters for hashing technique and call relevant constructors
        switch (chunking_technique) {
            case ChunkingTech::FIXED:
                chunk_method = (Chunking_Technique *)new Fixed_Chunking(config);
                break;
            case ChunkingTech::RABINS:
                chunk_method = (Chunking_Technique *)new Rabins_Chunking(config);
                break;
            default:
                std::cerr << "Unimplemented chunking technique" << std::endl;
                exit(EXIT_FAILURE);
        }

        switch (hashing_technique) {
            case HashingTech::MD5:
                hash_method = std::make_unique<MD5_Hashing>();
                break;
            case HashingTech::SHA1:
                hash_method = std::make_unique<SHA1_Hashing>();
                break;
            case HashingTech::SHA256:
                hash_method = std::make_unique<SHA256_Hashing>();
                break;
            default:
                std::cerr << "Unimplemented hashing technique" << std::endl;
                exit(EXIT_FAILURE);
        }

        // Call driver function
        driver_function(dir_path, chunk_method, hash_method, output_file);

        // Cleanup pointers
        delete chunk_method;
    } catch (const ConfigError& e) {
        std::cerr << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}