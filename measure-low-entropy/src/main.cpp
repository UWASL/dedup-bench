/**
 * @file ae_chunking.cpp
 * @author WASL
 * @brief Implementations for Experiment_Chunking technique
 * @version 0.1
 * @date 2023-3-10
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <set>
#include <string>
#include <cstring>

#define MAX_LENGTH 500000
#define NUMBER_OF_GROUPS 10
#define MIN_LENGTH 10
#define ENABLE_REWRITE 0

int main(int argc, char* argv[]) {
    uint64_t max_length = MAX_LENGTH;
    uint64_t num_of_groups = NUMBER_OF_GROUPS;
    uint64_t min_length = MIN_LENGTH;
    bool enable_rewrite = false;

    if (argc < 2) {
        std::cout << "Usage: ./measure-low-entropy.exe <Directory> "
                     "[enable rewrite {f}] [max_length] [number_of_groups] "
                     "[min_detectable_length]"
                  << std::endl;
        std::cout << "\t  [max_length]: The length of the longest string to be "
                     "detected. Default: 500000"
                  << std::endl;
        std::cout << "\t  [number_of_groups]: Number of groups  that different "
                     "length will be combined into. Default: 10"
                  << std::endl;
        std::cout << "\t  [min_detectable_length]: the smallest number of "
                     "bytes to be treated as a low entrpy. Default: 10"
                  << std::endl;
        exit(EXIT_FAILURE);
    } else if (argc == 3) {
        std::string bool_val = std::string(argv[2]);
        if (bool_val != "t" && bool_val != "f") {
            std::cerr << bool_val << " is not valid. Must be either 't' or 'f'"
                      << std::endl;
            exit(EXIT_FAILURE);
        }
        if (bool_val == "t") {
            enable_rewrite = true;
        }
    } else if (argc == 4) {
        std::string bool_val = std::string(argv[2]);
        if (bool_val != "t" && bool_val != "f") {
            std::cerr << bool_val << " is not valid. Must be either 't' or 'f'"
                      << std::endl;
            exit(EXIT_FAILURE);
        }
        if (bool_val == "t") {
            enable_rewrite = true;
        }
        try {
            max_length = std::stoull(argv[3]);
        } catch (std::invalid_argument const &) {
            std::cerr << argv[2] << " is not a valid Number." << std::endl;
            exit(EXIT_FAILURE);
        }
    } else if (argc == 5) {
        std::string bool_val = std::string(argv[2]);
        if (bool_val != "t" && bool_val != "f") {
            std::cerr << bool_val << " is not valid. Must be either 't' or 'f'"
                      << std::endl;
            exit(EXIT_FAILURE);
        }
        if (bool_val == "t") {
            enable_rewrite = true;
        }
        try {
            max_length = std::stoull(argv[3]);
        } catch (std::invalid_argument const &) {
            std::cerr << argv[2] << " is not a valid Number." << std::endl;
            exit(EXIT_FAILURE);
        }
        try {
            num_of_groups = std::stoull(argv[4]);
        } catch (std::invalid_argument const &) {
            std::cerr << argv[3] << " is not a valid Number." << std::endl;
            exit(EXIT_FAILURE);
        }
    } else if (argc == 6) {
        std::string bool_val = std::string(argv[2]);
        if (bool_val != "t" && bool_val != "f") {
            std::cerr << bool_val << " is not valid. Must be either 't' or 'f'"
                      << std::endl;
            exit(EXIT_FAILURE);
        }
        if (bool_val == "t") {
            enable_rewrite = true;
        }
        try {
            max_length = std::stoull(argv[3]);
        } catch (std::invalid_argument const &) {
            std::cerr << argv[2] << " is not a valid Number." << std::endl;
            exit(EXIT_FAILURE);
        }
        try {
            num_of_groups = std::stoull(argv[4]);
        } catch (std::invalid_argument const &) {
            std::cerr << argv[3] << " is not a valid Number." << std::endl;
            exit(EXIT_FAILURE);
        }
        try {
            min_length = std::stoull(argv[5]);
        } catch (std::invalid_argument const &) {
            std::cerr << argv[4] << " is not a valid Number." << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    uint64_t histogram[max_length];
    memset( histogram, 0, max_length*sizeof(int) );

    double cumulative_percent = 0;
    std::string dir_path = std::string(argv[1]);
    uint64_t total_size = 0;

    if (!std::filesystem::is_directory(dir_path)) {
        std::cerr << dir_path << " is not a directory" << std::endl;
        exit(0);
    }
    std::ofstream output_file;
    if(enable_rewrite)
        output_file.open("removed_low_ent.bin",
                     std::ios::out | std::ios::binary | std::ios::app);

    for (const auto& entry :
         std::filesystem::recursive_directory_iterator(dir_path)) {
        std::filesystem::path file_path = entry.path();
        if (std::filesystem::is_directory(file_path)) {
            // Avoid trying to analyzing a directory path
            continue;
        }
        std::cout << "Analyzing: " << file_path << " ..." << std::endl;

        std::ifstream file_ptr;
        file_ptr.open(file_path, std::ios::in);

        // Get size of file
        file_ptr.seekg(0, std::ios_base::end);
        uint64_t file_size_bytes = file_ptr.tellg();
        total_size += file_size_bytes;

        // Seek back to beginning and set up bytes_to_read
        file_ptr.seekg(0, std::ios_base::beg);
        uint64_t bytes_to_read =
            std::min((uint64_t)max_length, file_size_bytes);

        uint64_t curr_bytes_read = 0;

        char buff[max_length - 1];
        while (curr_bytes_read < file_size_bytes) {
            file_ptr.read(buff, bytes_to_read);
            uint64_t i = 0;
            uint64_t length = 1;
            while (i < bytes_to_read) {
                int j = i;
                while ((++i < bytes_to_read) && (buff[i-1] == buff[i])) {
                    length++;
                }
                if(length > min_length){
                    histogram[length]++;
                }
                if (length <= min_length && enable_rewrite) {
                    output_file.write(&buff[j], length);
                }
                length = 1;
            }
            curr_bytes_read += bytes_to_read;
            bytes_to_read = std::min((uint64_t)max_length - 1,
                                     file_size_bytes - curr_bytes_read);
        }
    }

    // combine the length into groups
    uint64_t group_size = max_length / num_of_groups;
    uint64_t groups_hist[50] = {0};
    double groups_hist_percentage[50] = {0};

    for (uint64_t i = 2; i < max_length; i++) {
        double pct = ((histogram[i] * i) / (double)total_size) * 100;
        // std:: cout<<"length:" << i << ": "<< "Percentage: " << pct <<
        // "count:" << (histogram[i])<<std::endl;
        cumulative_percent += pct;
        groups_hist[i / (group_size + 1)] += histogram[i];
        groups_hist_percentage[i / (group_size + 1)] += pct;
    }
    std::cout << "cumulative_percent:" << cumulative_percent << std::endl;
    std::cout << "Single bytes "
              << "Percentage: "
              << ((histogram[1] * 1) / (double)total_size) * 100
              << " Count:" << histogram[1] << std::endl;
    for (uint64_t i = 0; i < num_of_groups; i++) {
        std::cout << i * group_size + 1 << " - " << (i + 1) * group_size << ": "
                  << "Percentage: " << groups_hist_percentage[i]
                  << " Count:" << groups_hist[i] << std::endl;
    }

    exit(EXIT_SUCCESS);
}
