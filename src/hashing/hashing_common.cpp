/**
 * @file hashing_common.cpp
 * @author WASL
 * @brief File to hold implementations of functions common across hashing techniques
 * @version 0.1
 * @date 2023-02-06
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "hashing_common.hpp"

#include <iostream>
#include <fstream>
#include <tuple>
#include "hash.hpp"


void print_hashes(std::vector<Hash> hash_list){
    /**
     * @brief Print all hash values from the given list
     * @param hash_list: Vector containing hash values
     * 
     */
    for (const Hash& hash: hash_list){
        std::cout << "Hash: " << hash.toString() << std::endl;
    }
}

void write_hashes_to_file(std::vector<Hash> hash_list, std::string out_file_path){
    /**
     * @brief Write all given hash values to file
     * @param hash_list: Vector containing hash values
     * @param out_file_path: Path to output file where hashes will be written to
     * 
     */
    
    std::ofstream out_stream;
    out_stream.open(out_file_path, std::ios::out);
    
    // Iterate over hash values and write them to file
    for (Hash hash: hash_list) {
        out_stream << hash.toString() << std::endl;
    }

    out_stream.close();
}

void Hashing_Technique::hash_chunks(std::vector<File_Chunk>& file_chunks){
    /**
     * @brief Hash all chunks in a given vector using the relevant hash_chunk() implementation
     * @param file_chunks: Vector containing struct File_Chunk
     * @return: void
     */

    // Iterate over all chunks and generate hash values
    for (File_Chunk& fc: file_chunks) {
        hash_chunk(fc);
    }
}
