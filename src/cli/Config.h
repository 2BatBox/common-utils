#ifndef CLI_CONFIG_H
#define CLI_CONFIG_H

namespace cli {

class Config {
public:
	const std::string short_prefix;
	const std::string long_prefix;
	const std::string long_arg_start;
	const std::string terminator;
	
	const unsigned name_offset;
	const unsigned description_offset;

	Config(
	std::string short_prefix = "-",
	std::string long_prefix = "--",
	std::string long_arg_start = "=",
	std::string terminator = "--",
	unsigned left_offset = 4,
	unsigned description_offset = 12
		) noexcept :
	short_prefix(short_prefix),
	long_prefix(long_prefix),
	long_arg_start(long_arg_start),
	terminator(terminator),
	name_offset(left_offset),
	description_offset(description_offset)
	{ }

};

}; // namespace cli;

#endif /* CLI_CONFIG_H */

