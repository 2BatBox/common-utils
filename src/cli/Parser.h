#ifndef CLI_PARSER_H
#define CLI_PARSER_H

#include <string>

#include "Config.h"

namespace cli {

class EventHandler {
public:

	virtual ~EventHandler() noexcept = default;

	virtual bool begin() = 0;
	virtual bool end() = 0;

	virtual bool start_opt_short(char opt) = 0;
	virtual bool start_opt_long(std::string opt) = 0;
	virtual bool start_opt_long(std::string opt, std::string arg) = 0;
	virtual bool start_argument(std::string arg) = 0;

};

class Parser {
	Config m_config;

public:

	Parser(const Config& config) noexcept :
	m_config(config) { }

	bool parse(int argc, char** argv, EventHandler* eh) {

		if (not eh->begin()) {
			return false;
		}

		for (int i = 0; i < argc; i++) {
			std::string argument(argv[i]);

			if (argument.compare(0, m_config.long_prefix.length(), m_config.long_prefix) == 0) {
				// long option
				argument = argument.substr(m_config.long_prefix.length());
				if (not parser_long_option(argument, eh)) {
					return false;
				}

			} else if (argument.compare(0, m_config.short_prefix.length(), m_config.short_prefix) == 0) {
				// short option
				argument = argument.substr(m_config.short_prefix.length());
				if (not parser_short_option(argument, eh)) {
					return false;
				}

			} else {
				if (not eh->start_argument(argument)) {
					return false;
				}
			}
		}
		return eh->end();
	}

	bool parser_long_option(std::string argument, EventHandler* eh) {
		std::string option;
		std::string option_argument;
		auto eq_sym = argument.find(m_config.long_arg_start);

		// with argument
		if (eq_sym != std::string::npos) {
			option = argument.substr(0, eq_sym);
			option_argument = argument.substr(eq_sym + 1);

			if (not validate_long_option(option) || option_argument.length() == 0) {
				return false;
			}

			if (not eh->start_opt_long(option, option_argument)) {
				return false;
			}

			// with no argument
		} else {
			if (not (validate_long_option(argument) && eh->start_opt_long(argument))) {
				return false;
			}
		}
		return true;
	}

	bool parser_short_option(std::string argument, EventHandler* eh) {
		for (auto elem : argument) {
			if (not (validate_short_option(elem) && eh->start_opt_short(elem))) {
				return false;
			}
		}
		return true;
	}

	bool validate_short_option(char opt) noexcept {
		return isalpha(opt);
	}

	bool validate_long_option(std::string opt) noexcept {
		if (opt.length() == 0)
			return false;

		if (not isalpha(opt.c_str()[0]))
			return false;

		for (unsigned i = 1; i < opt.length(); i++) {
			char ch = opt.c_str()[i];
			if (not (isalnum(ch) || ch == '_' || ch == '-'))
				return false;
		}
		return true;
	}

};

}; // namespace cli;

#endif /* CLI_PARSER_H */

