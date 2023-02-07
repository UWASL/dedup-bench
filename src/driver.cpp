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
#include "std_hashing.hpp"
#include "fnv_hashing.hpp"

#include <fstream>

static void driver_function(std::string file_path, Chunking_Technique *chunk_method, Hashing_Technique *hash_method){
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
    std::vector<std::string> hash_list = hash_method->hash_chunks(file_chunks);

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
    if(argc != 5){
        std::cout << "Usage: ./driver <file_path> <chunking_technique> <hashing_technique> <chunk_size_for_fixed>" << std::endl;
        std::cout << "\t  <file_path>: Path to file to run chunking and hashing on." << std::endl;
        std::cout << "\t  Supported <chunking_technique>s: fixed" << std::endl;
        std::cout << "\t  Supported <hashing_technique>s: std, fnv" << std::endl;
        std::cout << "\t  <chunk_size_for_fixed>: Positive integer representing size of each chunk in the fixed chunking technique." << std::endl;

        exit(EXIT_FAILURE);
    }

    std::string file_path = std::string(argv[1]);
    std::string chunking_technique = std::string(argv[2]);
    std::string hashing_technique = std::string(argv[3]);

    // Pointers used to hold derived instances of Chunking_Technique and Hashing_Technique
    Chunking_Technique *chunk_method = nullptr;
    Hashing_Technique *hash_method = nullptr;
    
    // Set parameters for hashing technique and call relevant constructors
    if(hashing_technique == "std"){
        hash_method = (Hashing_Technique *)new Std_Hashing();
    }
    else if(hashing_technique == "fnv"){
        hash_method = (Hashing_Technique *)new Fnv_Hashing();
    }
    else{
        std::cout << "Unsupported hashing technique: " << hashing_technique << std::endl;
        exit(EXIT_FAILURE);
    }

    // Set parameters for fixed chunking and call relevant constructors
    if(chunking_technique == "fixed"){
        /**
         * @todo: Add checks to make sure random negative stuff isn't passed. Can be done within the new Config class.
         * 
         */
        uint64_t chunk_size = atoi(argv[4]);
        chunk_method = (Chunking_Technique *)new Fixed_Chunking(chunk_size);
    }
    else{
        std::cout << "Unsupported chunking technique: " << chunking_technique << std::endl;
        exit(EXIT_FAILURE);
    }

    // Call driver function
    driver_function(file_path, chunk_method, hash_method);

    // Cleanup pointers
    delete hash_method;
    delete chunk_method;

    exit(EXIT_SUCCESS);

}