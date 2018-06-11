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

class OptionValue {
	bool m_presented;
	bool m_has_argument;
	std::string m_argument;

public:

	OptionValue() noexcept : m_presented(false), m_has_argument(false), m_argument() {
	}

	bool presented() const noexcept {
		return m_presented;
	}

	bool has_argument() const noexcept {
		return m_has_argument;
	}

	const std::string& argument() const noexcept {
		return m_argument;
	}

	void set_presented() noexcept {
		m_presented = true;
	}

	void set_argument(const std::string& value) noexcept {
		m_presented = true;
		m_has_argument = true;
		m_argument = value;
	}

	void clear() noexcept {
		m_presented = false;
		m_has_argument = false;
		m_argument.clear();
	}

};

class Option {
	const char m_short_name;
	const std::string m_long_name;
	const ArgumentType m_arg_type;

	bool m_required;
	std::string m_desc;
	std::string m_arg_name;
	OptionValue m_value;

public:

	static constexpr char SHORT_NAME_NONE = 0;

	Option(char short_name, const std::string& long_name, ArgumentType arg_type) noexcept :
	m_short_name(short_name),
	m_long_name(long_name),
	m_arg_type(arg_type),
	m_required(false),
	m_desc(),
	m_arg_name(),
	m_value() {
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

	const std::string& name() const noexcept {
		std::string result;
		if (has_short_name()) {
			result += m_short_name;
			result += " ";
		}
		if (has_long_name()) {
			result += m_long_name;
		}
		return std::move(result);
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
		return m_arg_name;
	}

	const OptionValue& value() const noexcept {
		return m_value;
	}

	OptionValue& value() noexcept {
		return m_value;
	}

	// setters

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

};

}; // namespace cli;

#endif /* CLI_OPTION_H */

