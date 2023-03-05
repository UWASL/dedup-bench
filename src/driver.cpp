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
#include "config.hpp"
#include "config_error.hpp"
#include "chunking_common.hpp"

#include <fstream>
#include <memory>

static void driver_function(std::string file_path, Chunking_Technique *chunk_method, std::unique_ptr<Hashing_Technique>& hash_method){
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
    
    std::vector<File_Chunk> file_chunks;

    // Chunk file using specified Chunking_Technique
    file_chunks = chunk_method->chunk_file(file_path);

    std::cout << "Total chunks: " << file_chunks.size() << std::endl;

    // Hash chunks using specified Hashing_Technique
    std::vector<Hash> hash_list = hash_method->hash_chunks(file_chunks);

    // Print hashes
    print_hashes(hash_list);

    // Cleanup chunk memory
    cleanup_chunks(file_chunks);
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

    std::string file_path = std::string(argv[1]);
    try {
        Config config{std::string(argv[2])};
        ChunkingTech chunking_technique = config.get_chunking_tech();
        HashingTech hashing_technique = config.get_hashing_tech();

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
            default:
                std::cerr << "Unimplemented chunking technique" << std::endl;
                exit(EXIT_FAILURE);
        }

        switch (hashing_technique) {
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
        driver_function(file_path, chunk_method, hash_method);

        // Cleanup pointers
        delete chunk_method;
    } catch (const ConfigError& e) {
        std::cerr << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}