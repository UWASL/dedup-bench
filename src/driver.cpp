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

#include "chunking.hpp"
#include <fstream>

static std::vector<File_Chunk> chunk_single_file(std::string file_path, std::vector<File_Chunk> (*chunking_technique)(std::string)){
    /**
     * @brief Returns file chunks based on the chunking technique selected
     * @param file_path: Path to input file
     * @param chunking_technique: Function pointer to chunking technique
     *      All supported chunking techniques must return a std::vector<File_Chunk> and accept a std::string (file_name) as the sole parameter
     */

    return chunking_technique(file_path);    
}

static void driver_function(std::string file_path, std::string chunking_technique){
    /**
     * @brief Uses the specified chunking technique to chunk the file
     * @todo: Change this to iterate over all files in directory
     * 
     * @param chunking_technique: Technique to use. Currently supported: Fixed
     * 
     */
    
    std::vector<File_Chunk> file_chunks;

    // Identify technique to use
    if(chunking_technique == "Fixed"){
        file_chunks = chunk_single_file(file_path, Chunking_Techniques::fixed_size_chunks);
    }
    else{
        std::cout << "Unsupported technique: " << chunking_technique << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << "Total chunks: " << file_chunks.size() << std::endl;

    // Print chunks
    print_chunks(file_chunks);

    // Clean up chunk memory
    for(File_Chunk chunk:file_chunks){
        delete chunk.chunk_data;
    }

}

int main(int argc, char * argv[]){
/**
 * @brief Main entry point for dedup.exe. Parses arguments and calls driver_function()
 * 
 */
    if(argc != 4){
        std::cout << "Usage: ./driver <file_path> <chunking_technique> <chunk_size_for_fixed>" << std::endl;
        std::cout << "\t Supported techniques: Fixed" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Set default fixed chunk size before doing anything else
    Chunking_Techniques::set_fixed_chunk_size(DEFAULT_FIXED_CHUNK_SIZE);

    std::string file_path = std::string(argv[1]);
    std::string chunking_technique = std::string(argv[2]);
    
    // Set parameters for fixed chunking and call driver_function
    if(chunking_technique == "Fixed"){
        uint64_t chunk_size = atoi(argv[3]);
        Chunking_Techniques::set_fixed_chunk_size(chunk_size);
    }

    driver_function(file_path, chunking_technique);
    exit(EXIT_SUCCESS);

}