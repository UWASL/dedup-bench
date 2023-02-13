#ifndef _CONFIG_ERROR_
#define _CONFIG_ERROR_
#include <exception>
#include <string>

class ConfigError: public std::exception {
	const char* msg;
	public:
		ConfigError(const std::string& msg): msg{msg.c_str()} {};

		const char* what() const noexcept override {
			return msg;
		}
};

#endif
