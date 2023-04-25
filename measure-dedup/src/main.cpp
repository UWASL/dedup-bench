#include <iostream>
#include <fstream>
#include <set>
#include <string>
#include <iomanip>


int main(int argc, char * argv[]){
    if(argc != 2){
        std::cout << "Usage: ./measure-dedup.exe <hash_file_path>" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string hash_file_path = std::string(argv[1]);

    // read the hash file
    std::ifstream infile(hash_file_path);
    if (infile.fail()) {
        std::cerr << "Failed to open hash file: " << hash_file_path << std::endl;
        exit(EXIT_FAILURE);
    }

    uint64_t count = 0;
    uint64_t non_dedup_bytes = 0;
    uint64_t actual_bytes = 0;
    std::string line;
    std::set<std::string> hash_set;
    while(std::getline(infile, line)) {
        size_t idx = line.find(',');
        std::string hash = line.substr(0, idx);
        uint64_t size = std::stoull(line.substr(idx+1, line.length()-idx-1));
        if (hash_set.count(hash) == 0) {
            hash_set.insert(hash);
            actual_bytes += size;
        }
        non_dedup_bytes += size;
        count++;
    }

    std::cout << "Read " << count << " records in total" << std::endl;
    std::cout << "Total bytes without dedup: " << non_dedup_bytes << std::endl;
    std::cout << "Total bytes with dedup: " << actual_bytes << std::endl;
    std::cout.precision(4);
    std::cout << std::setw(7) << "Dedup ratio: " <<  (double)non_dedup_bytes / (double)actual_bytes << std::endl;
    exit(EXIT_SUCCESS);
}
