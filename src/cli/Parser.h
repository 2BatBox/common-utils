#ifndef CLI_PARSER_H
#define CLI_PARSER_H

#include <string>

#include "Config.h"
#include "OptionList.h"

namespace cli {

class EventHandler {
public:

	virtual ~EventHandler() noexcept = default;

	virtual void begin() throw (std::logic_error) = 0;
	virtual void end() throw (std::logic_error) = 0;

	virtual void start_short(char opt) throw (std::logic_error) = 0;

	// returns true if 'arg' has been processed as an option argument
	virtual bool start_short(char opt, std::string arg) throw (std::logic_error) = 0;
	virtual void start_long(std::string opt) throw (std::logic_error) = 0;
	virtual void start_long(std::string opt, std::string arg) throw (std::logic_error) = 0;

	// returns true if 'arg' has been processed as an option argument
	virtual bool start_argument(std::string arg) throw (std::logic_error) = 0;

};

class DefaultHandler : public EventHandler {
	OptionList& m_list;
	Option* m_short;

public:

	DefaultHandler(OptionList& list) noexcept :
	m_list(list), m_short(nullptr) {
	}

	void begin() throw (std::logic_error) {
//		LOG("begin()");
		m_list.clean();
		m_short = nullptr;
	}

	void end() throw (std::logic_error) {
//		LOG("end()");
		m_list.validate();
	}

	void start_short(char opt) throw (std::logic_error) {
//		LOG("start_short('%c')", opt);
		m_short = m_list.find(opt);
		if (m_short == nullptr) {
			fprintf(stderr, "unknown short option '%c'\n", opt);
			throw std::logic_error("unknown short option");
		}
		m_short->set_presented();
	}

	bool start_short(char opt, std::string arg) throw (std::logic_error) {
//		LOG("start_short('%c', '%s')", opt, arg.c_str());
		m_short = m_list.find(opt);
		if (m_short == nullptr) {
			fprintf(stderr, "unknown short option '%c'\n", opt);
			throw std::logic_error("unknown short option");
		}

		switch (m_short->arg_type()) {
		case ArgumentType::MANDATORY:
			m_short->set_arg_value(arg);
			return true;

		case ArgumentType::NONE:
			m_short->set_presented();
			return false;

		default:
			break;
		}
		return false;

	}

	void start_long(std::string opt) throw (std::logic_error) {
//		LOG("start_long('%s')", opt.c_str());
		Option* long_opt = m_list.find(opt);
		if (long_opt == nullptr) {
			fprintf(stderr, "unknown long option '%s'\n", opt.c_str());
			throw std::logic_error("unknown long option");
		}
		if (long_opt->arg_type() == ArgumentType::MANDATORY) {
			fprintf(stderr, "long option '%s' must have an argument\n", opt.c_str());
			throw std::logic_error("long option must have an argument");
		}
		long_opt->set_presented();
	}

	void start_long(std::string opt, std::string arg) throw (std::logic_error) {
//		LOG("start_long('%s', '%s')", opt.c_str(), arg.c_str());
		Option* long_opt = m_list.find(opt);
		if (long_opt == nullptr) {
			fprintf(stderr, "unknown long option '%s'\n", opt.c_str());
			throw std::logic_error("unknown long option");
		}
		if (long_opt->arg_type() == ArgumentType::NONE) {
			fprintf(stderr, "long option '%s' cannot have an argument\n", opt.c_str());
			throw std::logic_error("long option cannot have an argument");
		}
		long_opt->set_arg_value(arg);
	}

	bool start_argument(std::string arg) throw (std::logic_error) {
//		LOG("start_argument('%s')", arg.c_str());
		if (m_short != nullptr) {
			m_short->set_arg_value(arg);
			m_short = nullptr;
			return true;
		}
		return false;
	}

};

class Parser {
public:

	static void parse(int& argc, char**& argv, OptionList& list) throw (std::logic_error) {
		DefaultHandler handler(list);
		
		unsigned arg_index = 0;
		handler.begin();

		for (int i = 0; i < argc; i++) {
			std::string argument(argv[i]);

			if (argument == Config::terminator) {
				break;
			}
			if (argument.compare(0, Config::long_prefix.length(), Config::long_prefix) == 0) {
				// long option
				argument = argument.substr(Config::long_prefix.length());
				parser_long_option(argument, &handler);
			} else if (argument.compare(0, Config::short_prefix.length(), Config::short_prefix) == 0) {
				// short option
				argument = argument.substr(Config::short_prefix.length());
				parser_short_option(argument, &handler);
			} else {
				if (not handler.start_argument(argument)) {
					argv[arg_index] = argv[i];
					arg_index++;
				}
			}
		}
		argc = arg_index;
		handler.end();
	}
	
private:

	static void parser_long_option(std::string argument, EventHandler* eh) {
		std::string option;
		std::string option_argument;
		auto eq_sym = argument.find(Config::long_arg_start);

		if (eq_sym != std::string::npos) {
			// with argument
			option = argument.substr(0, eq_sym);
			option_argument = argument.substr(eq_sym + 1);
			validate_long(option);
			eh->start_long(option, option_argument);
		} else {
			// with no argument
			validate_long(argument);
			eh->start_long(argument);
		}
	}

	static void parser_short_option(std::string argument, EventHandler* eh) {
		if (argument.length() == 0) {
			return;
		}

		char option = argument.at(0);
		argument = argument.substr(1);

		validate_short(option);
		if (argument.length() > 0) {
			if (not eh->start_short(option, argument)) {
				parser_short_option(argument, eh);
			}
		} else {
			eh->start_short(option);
		}

	}

	static void validate_short(char opt) throw (std::logic_error) {
		if (opt == 0 || not isalpha(opt)) {
			fprintf(stderr, "invalid short option name '%c'\n", opt);
			throw std::logic_error("invalid short option name");
		}
	}

	static void validate_long(std::string opt) throw (std::logic_error) {
		if (opt.length() == 0 || not isalpha(opt.c_str()[0])) {
			fprintf(stderr, "invalid long option name '%s'\n", opt.c_str());
			throw std::logic_error("invalid long option name");
		}
	}

};

}; // namespace cli;

#endif /* CLI_PARSER_H */

