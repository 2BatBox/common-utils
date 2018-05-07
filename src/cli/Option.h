#ifndef CLI_OPTION_H
#define CLI_OPTION_H

#include <string>
#include <stdexcept>

#include <ctype.h>
#include <vector>

namespace cli {

class OptionValue {
	bool m_presented;
	bool m_has_value;
	std::string m_value;

public:

	OptionValue() noexcept : m_presented(false), m_has_value(false), m_value() { }

	bool presented() const noexcept {
		return m_presented;
	}

	bool has_argument() const noexcept {
		return m_has_value;
	}

	const std::string& argument() const noexcept {
		return m_value;
	}

	void set_presented() noexcept {
		m_presented = true;
	}

	void set_argument(const std::string& value) noexcept {
		m_has_value = true;
		m_value = value;
	}

	void clear() noexcept {
		m_presented = false;
		m_has_value = false;
		m_value.clear();
	}

};

class Option {
	static constexpr char SHORT_NAME_NONE = 0;

public:

	enum ArgType {
		NONE,
		MANDATORY,
		OPTIONAL
	};

	const char m_short_name;
	const std::string m_long_name;
	const std::string m_desc;

	bool m_required;
	ArgType m_arg_type;
	std::string m_arg_name;

	OptionValue m_value;

	/**
	 * Creates an option with an argument.
	 * @param short_name - A short name of the option, must be an alphabetic symbol. 
	 * @param short_name - A long name of the option, must start with an alphabetic symbol,
	 * may contains digits, dash and low dash.
	 * @param desc - A description of the options, must not be empty.
	 */
	Option(char short_name, const std::string& long_name, const std::string& description) throw (std::logic_error) :
	m_short_name(short_name),
	m_long_name(long_name),
	m_desc(description),
	m_required(false),
	m_arg_type(ArgType::NONE),
	m_arg_name(),
	m_value() {
		if (not isalpha(m_short_name))
			throw std::logic_error("option has no valid short name");

		if (not validate_name(m_long_name))
			throw std::logic_error("option has no valid long name");

		if (not (has_short_name() || has_long_name()))
			throw std::logic_error("option has no valid names");

		if (m_desc.length() == 0)
			throw std::logic_error("option has no description");
	}

	/**
	 * Creates an option with an argument.
	 * @param short_name - A long name of the option, must start with an alphabetic symbol,
	 * may contains digits, dash and low dash.
	 * @param desc - A description of the options, must not be empty.
	 */
	Option(const std::string& long_name, const std::string& description) throw (std::logic_error) :
	m_short_name(SHORT_NAME_NONE),
	m_long_name(long_name),
	m_desc(description),
	m_required(false),
	m_arg_type(ArgType::NONE),
	m_arg_name(),
	m_value() {
		if (not validate_name(m_long_name))
			throw std::logic_error("option has no valid long name");

		if (m_desc.length() == 0)
			throw std::logic_error("option has no description");
	}

	/**
	 * @param value - true if the option cannot be omitted.
	 * @return 
	 */
	Option& required(bool value) noexcept {
		m_required = value;
		return *this;
	}

	/**
	 * @param value - type of the argument.
	 * @return 
	 */
	Option& arg_type(ArgType value) noexcept {
		m_arg_type = value;
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

	// Getters

	char short_name() const noexcept {
		return m_short_name;
	}

	const std::string& long_name() const noexcept {
		return m_long_name;
	}

	const std::string& description() const noexcept {
		return m_desc;
	}

	bool required() const noexcept {
		return m_required;
	}

	ArgType arg_type() const noexcept {
		return m_arg_type;
	}

	std::string arg_name() const noexcept {
		if (m_arg_name.length() > 0)
			return m_arg_name;
		return "arg";
	}

	bool has_short_name() const noexcept {
		return m_short_name != SHORT_NAME_NONE;
	}

	bool has_long_name() const noexcept {
		return m_long_name.length() > 0;
	}

	OptionValue& value() noexcept {
		return m_value;
	}

	const OptionValue& value() const noexcept {
		return m_value;
	}

private:

	static bool validate_name(const std::string& name) noexcept {
		if (name.length() == 0)
			return true;

		if (not isalpha(name.c_str()[0]))
			return false;

		for (unsigned i = 1; i < name.length(); i++) {
			char ch = name.c_str()[i];
			if (not (isalnum(ch) || ch == '_' || ch == '-'))
				return false;
		}
		return true;
	}

};

}; // namespace cli;

#endif /* CLI_OPTION_H */

