#ifndef CLI_OPTION_LIST_H
#define CLI_OPTION_LIST_H

#include <string>
#include <stdexcept>

#include <ctype.h>
#include <cstdlib>
#include <vector>

#include "Option.h"

namespace cli {

class OptionList {
	std::vector<Option> m_options;

public:

	OptionList() noexcept : m_options() { }

	Option& add_short(char short_name, bool has_argument) throw (std::logic_error) {
		validate_short_name(short_name);
		Option result = Option(short_name, "", (has_argument) ? ArgumentType::MANDATORY : ArgumentType::NONE);
		return append(result);
	}

	Option& add_long(const std::string& long_name, bool has_argument) throw (std::logic_error) {
		validate_long_name(long_name);
		Option result = Option(Option::SHORT_NAME_NONE, long_name, (has_argument) ? ArgumentType::MANDATORY : ArgumentType::NONE);
		return append(result);
	}

	Option& add_long(const std::string& long_name, ArgumentType arg_type) throw (std::logic_error) {
		validate_long_name(long_name);
		Option result = Option(Option::SHORT_NAME_NONE, long_name, arg_type);
		return append(result);
	}

	Option& add_multi(char short_name, const std::string& long_name, bool has_argument) throw (std::logic_error) {
		validate_short_name(short_name);
		validate_long_name(long_name);
		Option result = Option(short_name, long_name, (has_argument) ? ArgumentType::MANDATORY : ArgumentType::NONE);
		return append(result);
	}

	const Option* find(char name) const noexcept {
		for (unsigned i = 0; i < m_options.size(); i++) {
			if (m_options[i].has_short_name() && m_options[i].short_name() == name)
				return &m_options[i];
		}
		return nullptr;
	}

	Option* find(char name) noexcept {
		for (unsigned i = 0; i < m_options.size(); i++) {
			if (m_options[i].has_short_name() && m_options[i].short_name() == name)
				return &m_options[i];
		}
		return nullptr;
	}

	const Option* find(const std::string& name) const noexcept {
		for (unsigned i = 0; i < m_options.size(); i++) {
			if (m_options[i].has_long_name() && m_options[i].long_name() == name)
				return &m_options[i];
		}
		return nullptr;
	}

	Option* find(const std::string& name) noexcept {
		for (unsigned i = 0; i < m_options.size(); i++) {
			if (m_options[i].has_long_name() && m_options[i].long_name() == name)
				return &m_options[i];
		}
		return nullptr;
	}

	const Option& find_except(char name) const throw (std::logic_error) {
		const Option* opt = find(name);
		if (opt == nullptr) {
			fprintf(stderr, "option '%c' has not been found\n", name);
			throw std::logic_error("option has not been found");

		}
		return *opt;
	}

	const Option& find_except(const std::string& name) const throw (std::logic_error) {
		const Option* opt = find(name);
		if (opt == nullptr) {
			fprintf(stderr, "option '%s' has not been found\n", name.c_str());
			throw std::logic_error("option has not been found");
		}
		return *opt;
	}

	const std::string& value_except(char name) const throw (std::logic_error) {
		const Option& opt = find_except(name);
		if (not opt.presented()) {
			fprintf(stderr, "option '%c' is not presented\n", name);
			throw std::logic_error("option is not presented");
		}
		return opt.arg_value();
	}

	const std::string& value_except(const std::string& name) const throw (std::logic_error) {
		const Option& opt = find_except(name);
		if (not opt.presented()) {
			fprintf(stderr, "option '%s' is not presented\n", name.c_str());
			throw std::logic_error("option is not presented");
		}
		return opt.arg_value();
	}

	size_t size() const noexcept {
		return m_options.size();
	}

	const Option& operator[](unsigned index)const noexcept {
		return m_options[index];
	}

	Option& operator[](unsigned index)noexcept {
		return m_options[index];
	}

	void clean() noexcept {
		for (auto elem : m_options) {
			elem.clear();
		}
	}

	void validate() throw (std::logic_error) {
		for (auto elem : m_options) {
			if (
				elem.arg_type() == ArgumentType::MANDATORY
				&& elem.presented()
				&& not elem.has_arg()
				) {
				fprintf(stderr, "option '%s' has a mandatory argument\n", elem.name().c_str());
				throw std::logic_error("a mandatory argument has not been set");
			}
		}
	}

	void dump(FILE* out) const noexcept {
		fprintf(out, "==== Presented Option List ====\n");
		for (unsigned i = 0; i < size(); i++) {
			const Option& opt = m_options[i];
			if (opt.presented()) {
				fprintf(out, "%s : ", opt.name().c_str());
				if (opt.has_arg()) {
					fprintf(out, "value='%s'", opt.arg_value().c_str());
				}
				fprintf(out, "\n");
			}
		}
	}

private:

	Option& append(Option& opt) throw (std::logic_error) {
		if (opt.has_short_name() && find(opt.short_name())) {
			fprintf(stderr, "option '%c' has already been appended\n", opt.short_name());
			throw std::logic_error("option has already been appended");
		}

		if (opt.has_long_name() && find(opt.long_name())) {
			fprintf(stderr, "option '%s' has already been appended\n", opt.long_name().c_str());
			throw std::logic_error("option has already been appended");
		}

		if (opt.arg_type() != ArgumentType::NONE) {
			opt.arg_name("arg");
		}
		m_options.push_back(opt);
		return *(m_options.rbegin());
	}

	static void validate_short_name(char name) throw (std::logic_error) {
		if (not isalpha(name)) {
			fprintf(stderr, "Name '%c' is not a valid short name, the name must be alphabetical\n", name);
			throw std::logic_error("invalid short name");
		}
	}

	static void validate_long_name(const std::string& name) throw (std::logic_error) {
		if (not check_long_name(name)) {
			fprintf(stderr, "Name '%s' is not a valid long name, ", name.c_str());
			fprintf(stderr, "the name must be start with an alphabetical symbol, it also can contain dashes. ");
			fprintf(stderr, "The name must have two symbols at least.\n");
			throw std::logic_error("invalid long name");
		}
	}

	static bool check_long_name(const std::string& name) noexcept {
		if (name.length() < 2)
			return false;

		if (not isalpha(name.c_str()[0]))
			return false;

		for (unsigned i = 1; i < name.length(); i++) {
			char ch = name.c_str()[i];
			if (not (isalpha(ch) || ch == '-'))
				return false;
		}
		return true;
	}

};

}; // namespace cli;

#endif /* CLI_OPTION_LIST_H */

