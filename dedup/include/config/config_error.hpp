#ifndef _CONFIG_ERROR_
#define _CONFIG_ERROR_
#include <exception>
#include <string>

class ConfigError: public std::exception {
    const std::string msg;
    public:
        ConfigError(const std::string& msg);

        const char* what() const noexcept override;
};

#endif
