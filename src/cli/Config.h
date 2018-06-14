#ifndef CLI_CONFIG_H
#define CLI_CONFIG_H

#include <string>

namespace cli {

class Config {
public:
	static const std::string short_prefix;
	static const std::string long_prefix;
	static const std::string long_arg_start;
	static const std::string terminator;
	
	static const unsigned name_offset;
	static const unsigned description_offset;
	
};

}; // namespace cli;

#endif /* CLI_CONFIG_H */

