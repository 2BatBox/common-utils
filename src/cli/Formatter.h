#ifndef CLI_FORMATTER_H
#define CLI_FORMATTER_H

#include <cstdio>
#include <cstring>

#include "OptionList.h"
#include "Config.h"

namespace cli {

class Formatter {
public:

	static void print_options(FILE* out, const OptionList& list) noexcept {
		for (unsigned i = 0; i < list.size(); i++) {
			const Option& opt = list[i];
			fprintf(out, "%*s", Config::name_offset, "");

			if (opt.has_short_name()) {
				print_short(out, opt);
			}

			if (opt.has_long_name()) {
				print_long(out, opt);
			}
			fprintf(out, "\n");

			if (opt.description().length()) {
				print_desc(out, opt.description().c_str());
			}
		}
	}

private:

	static void print_short(FILE* out, const Option& opt) noexcept {
		fprintf(out, "%s%c", Config::short_prefix.c_str(), opt.short_name());

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

	static void print_long(FILE* out, const Option& opt) noexcept {
		fprintf(out, "%s%s", Config::long_prefix.c_str(), opt.long_name().c_str());

		switch (opt.arg_type()) {
		case ArgumentType::NONE:
			break;

		case ArgumentType::MANDATORY:
			fprintf(out, "%s%s", Config::long_arg_start.c_str(), opt.arg_name().c_str());
			break;

		case ArgumentType::OPTIONAL:
			fprintf(out, "[%s%s]", Config::long_arg_start.c_str(), opt.arg_name().c_str());
			break;
		}
	}

	static void print_desc(FILE* out, const char* desc) noexcept {
		fprintf(out, "%*s", Config::description_offset, "");
		unsigned desc_len = strlen(desc);
		for (unsigned i = 0; i < desc_len; i++) {
			if (desc[i] == '\n') {
				fprintf(out, "\n%*s", Config::description_offset, "");
			} else {
				fprintf(out, "%c", desc[i]);
			}
		}
		fprintf(out, "\n");
	}

};

}; // namespace cli;

#endif /* CLI_FORMATTER_H */

