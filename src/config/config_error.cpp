#include "config_error.hpp"


ConfigError::ConfigError(const std::string& msg): msg{msg} {};

const char* ConfigError::what() const noexcept {
    return msg.c_str();
}