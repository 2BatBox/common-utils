#ifndef CLI_CLI_H
#define CLI_CLI_H

#include <cstdio>
#include <vector>

#include "Option.h"
#include "Config.h"
#include "Parser.h"
#include "Formatter.h"

namespace cli {

class Cli : public cli::EventHandler {
	const Config m_config;
	std::vector<Option> m_options;
	Option* m_current_option;
	std::vector<std::string> m_arguments;

public:

	Cli() noexcept :
	m_config(),
	m_options(),
	m_current_option(nullptr),
	m_arguments() { }

	Cli(Config config) noexcept :
	m_config(config),
	m_options(),
	m_current_option(nullptr),
	m_arguments() { }

	Cli(const Cli&) = default;
	Cli& operator=(const Cli&) = default;

	/**
	 * Appends an option to the parser.
	 * Options with the same names cannot be appended.
	 * @param opt
	 * @return 
	 */


	void print_options(FILE* out) const noexcept {
		Formatter formatter(m_config);
		formatter.print_options(m_options, out);
	}

	bool parse_args(int argc, char** argv) noexcept {
		Parser parser(m_config);
		return parser.parse(argc, argv, this);
	}

	unsigned arguments() const noexcept {
		return m_arguments.size();
	}

	const std::string& arguments(unsigned index) const noexcept {
		return m_arguments.at(index);
	}

	void dump() noexcept {
		printf("---- Cli state ----\n");
		printf("presented options:\n");
		for (auto opt : m_options) {
			const OptionValue& value = opt.value();
			if (value.presented()) {

				if (opt.has_short_name()) {
					if (opt.value().has_argument())
						printf("%c = '%s'\n", opt.short_name(), value.argument().c_str());
					else
						printf("%c\n", opt.short_name());
				} else {
					if (opt.value().has_argument())
						printf("%s = '%s'\n", opt.long_name().c_str(), value.argument().c_str());
					else
						printf("%s\n", opt.long_name().c_str());
				}
			}
		}
		printf("\n");

		printf("presented arguments:\n");
		for (unsigned i = 0; i < m_arguments.size(); i++) {
			printf("[%u] '%s'\n", i, m_arguments[i].c_str());
		}
	}

private:

	// parser events

	bool begin() {
		for (auto opt : m_options) {
			opt.value().clear();
		}
		m_current_option = nullptr;
		m_arguments.clear();
		return true;
	}

	bool end() {
//		for (auto opt : m_options) {
//			if (opt.required() && (not opt.value().presented())) {
//				if (opt.has_short_name()) {
//					fprintf(stderr, "required option '%c' has not been presented\n", opt.short_name());
//				} else {
//					fprintf(stderr, "required option '%s' has not been presented\n", opt.long_name().c_str());
//				}
//				return false;
//			}
//
//			if (opt.value().presented() && opt.arg_type() == Option::MANDATORY && (not opt.value().has_argument())) {
//				if (opt.has_short_name()) {
//					fprintf(stderr, "argument of option '%c' has not been presented\n", opt.short_name());
//				} else {
//					fprintf(stderr, "argument of option '%s' has not been presented\n", opt.long_name().c_str());
//				}
//				return false;
//			}
//		}
		return true;
	}

	bool start_short(char name) {
		Option* opt = find_short_name(name);
		if (opt == nullptr) {
			fprintf(stderr, "unknown option '%c'\n", name);
			return false;
		}

		opt->value().set_presented();
		m_current_option = opt;
		return true;
	}

	bool start_short(char name, std::string arg) {
		Option* opt = find_short_name(name);
		if (opt == nullptr) {
			fprintf(stderr, "unknown option '%c'\n", name);
			return false;
		}
		
		if(opt->arg_type() == Option::MANDATORY){
			opt->value().set_presented();
			opt->value().set_argument(arg);
		} else {
			fprintf(stderr, "option '%c' has no argument\n", name);
			return false;
		}
		return true;
	}

	bool start_long(std::string name) {
		Option* opt = find_long_name(name);
		if (opt == nullptr) {
			fprintf(stderr, "unknown option '%s'\n", name.c_str());
			return false;
		}

		if (opt->value().presented()) {
			fprintf(stderr, "option '%s' has already been presented\n", name.c_str());
			return false;
		}

		opt->value().set_presented();
		return true;
	}

	bool start_long(std::string name, std::string arg) {
		Option* opt = find_long_name(name);
		if (opt == nullptr) {
			fprintf(stderr, "unknown option '%s'\n", name.c_str());
			return false;
		}

		if (opt->value().presented()) {
			fprintf(stderr, "option '%s' has already been presented\n", name.c_str());
			return false;
		}

		opt->value().set_presented();
		opt->value().set_argument(arg);
		return true;
	}

	bool start_argument(std::string arg) {
		if (m_current_option && m_current_option->arg_type() != Option::NONE) {
			m_current_option->value().set_argument(arg);
			m_current_option = nullptr;
		} else {
			m_arguments.push_back(arg);
		}
		return true;
	}

};

}; // namespace cli;

#endif /* CLI_CLI_H */

