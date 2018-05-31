#ifndef CLI_OPTION_H
#define CLI_OPTION_H

#include <string>
#include <stdexcept>

#include <ctype.h>
#include <vector>

namespace cli {

enum class ArgumentType {
	NONE,
	MANDATORY,
	OPTIONAL
};

class Option {
	const char m_short_name;
	const std::string m_long_name;
	const ArgumentType m_arg_type;

	bool m_required;
	std::string m_desc;
	std::string m_arg_name;

public:

	static constexpr char SHORT_NAME_NONE = 0;

	Option(char short_name, const std::string& long_name, ArgumentType arg_type) noexcept :
	m_short_name(short_name),
	m_long_name(long_name),
	m_arg_type(arg_type),
	m_required(false),
	m_desc(),
	m_arg_name("arg") { };

	/**
	 * @param value - true if the option cannot be omitted.
	 * @return 
	 */
	Option& required(bool value) noexcept {
		m_required = value;
		return *this;
	}

	/**
	 * @param value - A name of the argument.
	 * @return 
	 */
	Option& description(const std::string& value) noexcept {
		m_desc += value;
		m_desc += "\n";
		return *this;
	}

	/**
	 * @param value - A name of the argument.
	 * @return 
	 */
	Option& arg_name(const std::string& value) noexcept {
		m_arg_name = value;
		return *this;
	}

	bool has_short_name() const noexcept {
		return m_short_name != SHORT_NAME_NONE;
	}

	bool has_long_name() const noexcept {
		return m_long_name.length() > 0;
	}

	// Getters

	char short_name() const noexcept {
		return m_short_name;
	}

	const std::string& long_name() const noexcept {
		return m_long_name;
	}

	ArgumentType arg_type() const noexcept {
		return m_arg_type;
	}

	bool required() const noexcept {
		return m_required;
	}

	const std::string& description() const noexcept {
		return m_desc;
	}

	std::string arg_name() const noexcept {
		if (m_arg_name.length() > 0)
			return m_arg_name;
		return "arg";
	}

private:

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

#endif /* CLI_OPTION_H */

