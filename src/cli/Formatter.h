#ifndef CLI_FORMATTER_H
#define CLI_FORMATTER_H

#include <vector>
#include <cstdio>

#include "Option.h"
#include "Config.h"

namespace cli {

class Formatter {
	Config m_config;

public:

	Formatter(const Config& config) noexcept : m_config(config) { }

	void print_options(const std::vector<Option>& options, FILE* out) noexcept {
		unsigned prefix_max_len = get_max_prefix_len(options);
		print_options(options, prefix_max_len, out);
	}

private:

	void print_options(const std::vector<Option>& options, unsigned pref_max_len, FILE* out) const noexcept {
		std::string pref;
		unsigned prefix_with_padding_len;
		for (unsigned i = 0; i < options.size(); i++) {
			pref = option_prefix(options[i]);
			fprintf(out, "%*s", m_config.left_offset, "");
			fprintf(out, "%-*s", pref_max_len, pref.c_str());
			fprintf(out, "%*s", m_config.description_offset, "");
			prefix_with_padding_len = pref_max_len + m_config.left_offset + m_config.description_offset;
			print_option_desc(out, prefix_with_padding_len, options[i].description().c_str());
		}
	}

	static void print_option_desc(FILE* out, unsigned pref_len, const char* desc) noexcept {
		unsigned desc_len = strlen(desc);
		for (unsigned i = 0; i < desc_len; i++) {
			if (desc[i] == '\n') {
				fprintf(out, "\n%*s\t", pref_len, "");
			} else {
				fprintf(out, "%c", desc[i]);
			}
		}
		fprintf(out, "\n");
	}

	unsigned get_max_prefix_len(const std::vector<Option>& options) const noexcept {
		unsigned pref_max_len = 0;
		std::string pref;
		for (unsigned i = 0; i < options.size(); i++) {
			pref = option_prefix(options[i]);
			if (pref.length() > pref_max_len)
				pref_max_len = pref.length();
		}
		return pref_max_len;
	}

	std::string option_prefix(const Option& opt) const {
		std::string result;

		if (opt.has_short_name()) {
			result += m_config.short_prefix;
			result += opt.short_name();
			if (opt.has_long_name()) {
				result += ", ";
			}
		} else {
			for (unsigned i = 0; i < m_config.short_prefix.length() + 3; i++) {
				result += " ";
			}
		}

		if (opt.has_long_name()) {
			result += m_config.long_prefix;
			result += opt.long_name();

			switch (opt.arg_type()) {
			case Option::NONE:
				break;

			case Option::MANDATORY:
				result += m_config.long_arg_start;
				result += opt.arg_name();
				break;

			case Option::OPTIONAL:
				result += "[";
				result += m_config.long_arg_start;
				result += opt.arg_name();
				result += "]";
				break;
			}

		}

		return std::move(result);
	}

};

}; // namespace cli;

#endif /* CLI_FORMATTER_H */

