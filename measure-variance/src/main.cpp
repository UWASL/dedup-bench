#include <iostream>
#include <fstream>
#include <set>
#include <string>
#include <iomanip>


int main(int argc, char * argv[]){
    if(argc != 2){
        std::cout << "Usage: ./measure-variance.exe <hash_file_path>" << std::endl;
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
    uint64_t total_size = 0;

    std::string line;
    uint64_t sizes[18]= {0};
    while(std::getline(infile, line)) {
        size_t idx = line.find(',');
        std::string hash = line.substr(0, idx);
        uint64_t size = std::stoull(line.substr(idx+1, line.length()-idx-1));
        sizes[size/1024]++;
        count++;
        total_size+=size;
    }

    std::cout << "Read " << count << " records in total" << std::endl;
    std::cout.precision(2);
    for(int i = 0; i < 17; i++){
        std::cout << i*1024 << " - " << (i*1024) +1023 << " count: " << sizes[i] << " Percentage: " << sizes[i]*100/(double)count << "%" << std::endl;
    }
    std::cout << "Average Chunk Size: " << total_size/count << std::endl;

    exit(EXIT_SUCCESS);
}
