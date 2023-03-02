#ifndef _PARSER_
#define _PARSER_
#include <string>
#include <unordered_map>


class Parser {
    const char delimiter = '=';
    const char comment = '#';
    std::unordered_map<std::string, std::string> dict;

    public:
        Parser(const std::string& config_file_path);

        /**
         * @brief Get the value of the given key if it exist.
         * throws std::out_of_range if key does not exist in the dictionary
         * 
         * @param key 
         * @return std::string 
         */
        std::string get_property(const std::string& key) const;

        /**
         * @brief Prints out all the stored key value mapping
         */
        void print() const;
};

#endif