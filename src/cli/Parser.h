#ifndef CLI_PARSER_H
#define CLI_PARSER_H

#include <string>

#include "Config.h"

namespace cli {

class EventHandler {
public:

	virtual ~EventHandler() noexcept = default;

	virtual void begin() throw (std::logic_error) = 0;
	virtual void end() throw (std::logic_error) = 0;

	virtual void start_short(char opt) throw (std::logic_error) = 0;
	virtual bool start_short(char opt, std::string arg) throw (std::logic_error) = 0;
	virtual void start_long(std::string opt) throw (std::logic_error) = 0;
	virtual void start_long(std::string opt, std::string arg) throw (std::logic_error) = 0;
	virtual void start_argument(std::string arg) throw (std::logic_error) = 0;

};

class Parser {
public:

	Parser() noexcept {}

	void parse(int argc, char** argv, EventHandler* eh) throw (std::logic_error) {
		eh->begin();

		for (int i = 0; i < argc; i++) {
			std::string argument(argv[i]);

			if (argument == Config::terminator) {
				break;
			}
			if (argument.compare(0, Config::long_prefix.length(), Config::long_prefix) == 0) {
				// long option
				argument = argument.substr(Config::long_prefix.length());
				parser_long_option(argument, eh);
			} else if (argument.compare(0, Config::short_prefix.length(), Config::short_prefix) == 0) {
				// short option
				argument = argument.substr(Config::short_prefix.length());
				parser_short_option(argument, eh);
			} else {
				eh->start_argument(argument);
			}
		}
		eh->end();
	}

	void parser_long_option(std::string argument, EventHandler* eh) {
		std::string option;
		std::string option_argument;
		auto eq_sym = argument.find(Config::long_arg_start);

		if (eq_sym != std::string::npos) {
			// with argument
			option = argument.substr(0, eq_sym);
			option_argument = argument.substr(eq_sym + 1);
			validate_long(option);
			validate_long_arg(option_argument);
			eh->start_long(option, option_argument);
		} else {
			// with no argument
			validate_long(argument);
			eh->start_long(argument);
		}
	}

	void parser_short_option(std::string argument, EventHandler* eh) {
		if(argument.length() == 0){
			return;
		}
		
		char option = argument.at(0);
		argument = argument.substr(1);
		
		validate_short(option);
		if(argument.length() > 0){
			if(eh->start_short(option, argument)){
				parser_short_option(argument, eh);
			}
		} else {
			eh->start_short(option);
		}
		
	}

	void validate_short(char opt) throw (std::logic_error) {
		if (opt == 0 || not isalpha(opt)) {
			fprintf(stderr, "invalid short option name '%c'\n", opt);
			throw std::logic_error("invalid short option name");
		}
	}

	void validate_long(std::string opt) throw (std::logic_error) {
		if (opt.length() == 0 || not isalpha(opt.c_str()[0])) {
			fprintf(stderr, "invalid long option name '%s'\n", opt.c_str());
			throw std::logic_error("invalid long option name");
		}
	}

	void validate_long_arg(std::string arg) throw (std::logic_error) {
		if (arg.length() == 0) {
			fprintf(stderr, "invalid long option argument '%s'\n", arg.c_str());
			throw std::logic_error("invalid long option argument");
		}
	}

};

}; // namespace cli;

#endif /* CLI_PARSER_H */

