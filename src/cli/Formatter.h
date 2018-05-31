#ifndef CLI_FORMATTER_H
#define CLI_FORMATTER_H

#include <cstdio>
#include <cstring>

#include "OptionList.h"
#include "Config.h"

namespace cli {

class Formatter {
	Config m_config;

public:

	Formatter(const Config& config) noexcept : m_config(config) { }

	void print_options(const OptionList& list, FILE* out) noexcept {
		for (unsigned i = 0; i < list.size(); i++) {
			const Option& opt = list[i];
			fprintf(out, "%*s", m_config.name_offset, "");

			if (opt.has_short_name()) {
				print_short(opt, out);
			}

			if (opt.has_long_name()) {
				print_long(opt, out);
			}
			fprintf(out, "\n");

			if (opt.description().length()) {
				print_desc(opt.description().c_str(), out);
			}

		}
	}

private:

	void print_short(const Option& opt, FILE* out) const noexcept {
		fprintf(out, "%s%c", m_config.short_prefix.c_str(), opt.short_name());

		switch (opt.arg_type()) {
		case ArgumentType::NONE:
			break;

		case ArgumentType::MANDATORY:
			fprintf(out, " %s", opt.arg_name().c_str());
			break;

		default:
			break;
		}

		if (opt.has_long_name()) {
			fprintf(out, ", ");
		}
	}

	void print_long(const Option& opt, FILE* out) const noexcept {
		fprintf(out, "%s%s", m_config.long_prefix.c_str(), opt.long_name().c_str());

		switch (opt.arg_type()) {
		case ArgumentType::NONE:
			break;

		case ArgumentType::MANDATORY:
			fprintf(out, "%s%s", m_config.long_arg_start.c_str(), opt.arg_name().c_str());
			break;

		case ArgumentType::OPTIONAL:
			fprintf(out, "[%s%s]", m_config.long_arg_start.c_str(), opt.arg_name().c_str());
			break;
		}
	}

	void print_desc(const char* desc, FILE* out) const noexcept {
		fprintf(out, "%*s", m_config.description_offset, "");
		unsigned desc_len = strlen(desc);
		for (unsigned i = 0; i < desc_len; i++) {
			if (desc[i] == '\n') {

				fprintf(out, "\n%*s", m_config.description_offset, "");
			} else {
				fprintf(out, "%c", desc[i]);
			}
		}
		fprintf(out, "\n");
	}

};

}; // namespace cli;

#endif /* CLI_FORMATTER_H */

