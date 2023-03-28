#include "parser.hpp"
#include "config_error.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <cctype>
#include <algorithm>
#define WHITESPACES " \t\n\r\f\v"


// trim from left & right
inline std::string trim(std::string s) {
    // trim from the right first and then trim from the left
    return s.erase(s.find_last_not_of(WHITESPACES) + 1).erase(0, s.find_first_not_of(WHITESPACES));
}

inline void to_lower(std::string& s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::tolower(c); });
}

Parser::Parser(const std::string& config_file_path) {
    // read the config file
    std::ifstream infile(config_file_path);
    if (infile.fail()) {
        throw ConfigError("Failed to open config file");
    }

    std::string line;
    while(std::getline(infile, line)) {
        // skip empty lines or comment lines
        if (line.length() == 0 || line[0] == comment) {
            continue;
        }
        to_lower(line);
        size_t idx = line.find(delimiter);
        dict[trim(line.substr(0, idx))] = trim(line.substr(idx+1, line.length()-idx-1));
    }
    infile.close();
}

std::string Parser::get_property(const std::string& key) const {
    return dict.at(key);
}

void Parser::print() const {
    for (auto it = dict.begin(); it != dict.end(); ++it) {
        std::cout << it->first << " = " << it->second << std::endl;
    }
}