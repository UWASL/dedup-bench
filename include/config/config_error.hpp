#ifndef _CONFIG_ERROR_
#define _CONFIG_ERROR_
#include <string>

class ConfigError {
	std::string msg;
	public:
		ConfigError(const std::string& msg): msg{msg} {};

		std::string get_msg() {
			return msg;
		}
};

#endif
