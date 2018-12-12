#ifndef CLI_CONTEXT_H
#define CLI_CONTEXT_H

#include "cli/OptionList.h"
#include "cli/Parser.h"
#include "cli/Formatter.h"
#include "binio/OptValue.h"
#include "utils/Types.h"
#include "utils/Tokenizer.h"

#include <cstdint>
#include <string>
#include <vector>

class CliContext {
public:

	static std::vector<std::string> files;

	static long log_period;
	static long cdr_period;
	static bool log_stdout;
	static bool verbose;
	static bool help;
	static bool version;

	static void dump(FILE* out) noexcept {
		fprintf(out, "---- launch configuration ----\n");
		fprintf(out, "files  : ");
		for (auto elem : CliContext::files) {
			fprintf(out, "'%s' ", elem.c_str());
		}
		fprintf(out, "\n");

		fprintf(out, "log_period : %lu sec\n", log_period);
		fprintf(out, "cdr_period : %lu sec\n", cdr_period);

		if (log_stdout) {
			fprintf(out, "+log_stdout\n");
		}
	}

	static void parse_args(int argc, char **argv) throw (std::logic_error) {
		cli::OptionList list;
		fill_option_list(list);
		cli::Parser::parse(argc, argv, list);

		// log_period & cdr_period
		std::string arg_log_period = list.find_except("log-period").arg_value();
		std::string arg_cdr_period = list.find_except("cdr-period").arg_value();
		log_period = utils::Types::str_to_l(arg_log_period);
		cdr_period = utils::Types::str_to_l(arg_cdr_period);

		if (log_period < 1) {
			throw std::logic_error("log-period value must be positive");
		}

		if (cdr_period < 1) {
			throw std::logic_error("cdr_period value must be positive");
		}

		// vlan filtering
		cli::Option opt_vlan_reject_all = list.find_except("vlan-reject-all");
		cli::Option opt_vlan_include = list.find_except("vlan-include");
		cli::Option opt_vlan_exclude = list.find_except("vlan-exclude");

		// log to stdout
		log_stdout = list.find_except("log-stdout").presented();

		// verbose
		verbose = list.find_except("verbose").presented();

		// help & version
		help = list.find_except('h').presented();
		version = list.find_except('v').presented();

		for (int i = 1; i < argc; i++) {
			files.push_back(argv[i]);
		}

	}

	static void print_usage(FILE* out, const char* ver) noexcept {
		cli::OptionList list;
		fill_option_list(list);
		print_info(out, ver);
		fprintf(out, "usage: dpdk-replay [options] <pcap_file(s)>\n");
		fprintf(out, "available options:\n");
		cli::Formatter::print_options(out, list);
	}

	static void print_info(FILE* out, const char* ver) noexcept {
		fprintf(out, "replay network traffic stored in pcap files via Intel DPDK\n");
		fprintf(out, "version: %s\n", ver);
		fprintf(out, "2SilentJay@gmail.com\n");
	}

private:

	static void fill_option_list(cli::OptionList& list) noexcept {
		cli::Option& period_log = list.add_long("log-period", cli::ArgumentType::MANDATORY).arg_name("sec");
		period_log.description("Sets the period of log files rotation.\n");
		period_log.description("Default values is 3600 sec (60 min).\n");
		period_log.description("Sample: --log-period=600\n");
		period_log.set_arg_value("3600");

		cli::Option& period_cdr = list.add_long("cdr-period", cli::ArgumentType::MANDATORY).arg_name("sec");
		period_cdr.description("Sets the period of CDR files rotation.\n");
		period_cdr.description("Default values is 3600 sec (60 min).\n");
		period_cdr.description("Sample: --cdr-period=600\n");
		period_cdr.set_arg_value("3600");

		cli::Option& vlan_disable = list.add_long("vlan-reject-all", false);
		vlan_disable.description("Enables VLAN filtering, none of VLAN IDs are allowed.\n");
		vlan_disable.description("\tProhibits: --vlan-include, --vlan-exclude.\n");

		cli::Option& vlan_include = list.add_long("vlan-include", cli::ArgumentType::MANDATORY).arg_name("list");
		vlan_include.description("Enables VLAN filtering, none of VLAN IDs are allowed except the include list.\n");
		vlan_include.description("Can take a list of integers in octal, decimal and hexadecimal format.\n");
		vlan_include.description("Sample: --vlan-include=1,02,0x003\n");
		vlan_include.description("\tProhibits: --vlan-exclude, --vlan-reject-all.\n");

		cli::Option& vlan_exclude = list.add_long("vlan-exclude", cli::ArgumentType::MANDATORY).arg_name("list");
		vlan_exclude.description("Enables VLAN filtering, all the VLAN IDs are allowed except the exclude list.\n");
		vlan_exclude.description("Can take a list of integers in octal, decimal and hexadecimal format.\n");
		vlan_exclude.description("Sample: --vlan-include=1,02,0x003\n");
		vlan_exclude.description("\tProhibits: --vlan-include, --vlan-reject-all.\n");

		list.add_long("log-stdout", false).description("Prints all the logs in stdout instead of log files.\n");
		list.add_long("verbose", false).description("Makes some additional logs during the start of the probe.\n");
		list.add_multi('h', "help", false).description("Print this screen and exit\n");
		list.add_multi('v', "version", false).description("Print version and exit\n");
	}

};

#endif /* CLI_CONTEXT_H */

