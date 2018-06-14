#include <iostream>
#include <cstdio>
#include <vector>

#include "../OptionList.h"
#include "../Formatter.h"
#include "../Config.h"
#include "../Parser.h"

#include "../../Log.h"
#include "../../utils/Types.h"

using namespace cli;

void dump(FILE* out, int argc, char** argv, const OptionList& list) noexcept {
    list.dump(out);
    fprintf(out, "==== Presented Argument List ====\n");
    for (int i = 0; i < argc; i++) {
        fprintf(out, "[%u] %s\n", i, argv[i]);
    }
}

void fill_option_list(OptionList& list) noexcept {
    Option& period_log = list.add_long("log-period", ArgumentType::MANDATORY).arg_name("sec");
    period_log.description("Sets the period of log files rotation.\n");
    period_log.description("Default values is 3600.\n");
    period_log.description("Sample: --log-period=600\n");
    period_log.set_arg_value("3600");

    Option& period_cdr = list.add_long("cdr-period", ArgumentType::MANDATORY).arg_name("sec");
    period_cdr.description("Sets the period of CDR files rotation.\n");
    period_cdr.description("Default values is 3600.\n");
    period_cdr.description("Sample: --cdr-period=600\n");
    period_cdr.set_arg_value("3600");

    Option& vlan_include = list.add_long("vlan-include", ArgumentType::MANDATORY).arg_name("list");
    vlan_include.description("Enables VLAN filtering, none of VLAN IDs are allowed except the include list.\n");
    vlan_include.description("Can take a list of integers in octal, decimal and hexadecimal format.\n");
    vlan_include.description("Sample: --vlan-include=1,02,0x003\n");
    vlan_include.description("\tProhibits: --vlan-exclude.\n");

    Option& vlan_exclude = list.add_long("vlan-exclude", ArgumentType::MANDATORY).arg_name("list");
    vlan_exclude.description("Enables VLAN filtering, all the VLAN IDs are allowed except the exclude list.\n");
    vlan_exclude.description("Can take a list of integers in octal, decimal and hexadecimal format.\n");
    vlan_exclude.description("Sample: --vlan-include=1,02,0x003\n");
    vlan_exclude.description("\tProhibits: --vlan-include.\n");

    list.add_long("log-std", false).description("Prints all the logs in stdout instead of log files.\n");
    list.add_long("cdr-std", false).description("Writes all the CDR in stdout instead of CDR files.\n");
    list.add_long("verbose", false).description("Makes some additional logs during the start of the probe.\n");
    list.add_multi('h', "help", false).description("Print this screen and exit\n");
    list.add_multi('v', "version", false).description("Print version and exit\n");
}

void print_usage(const char* bin, const OptionList& list) noexcept {
    LOG("usage: %s [OPTIONS]", bin);
    LOG("Version: %u.%u", 0, 0);
    LOG("Available options:");
    Formatter::print_options(stdout, list);
}

void print_version() noexcept {
    LOG("%u.%u", 0, 0);
}

int main(int argc, char** argv) {

    const char* bin = argv[0];
    OptionList list;
    fill_option_list(list);

    try {
        Parser::parse(argc, argv, list);
    } catch (const std::logic_error& e) {
        print_usage(bin, list);
        return 1;
    }

    const Option& opt_include = list.find_except("vlan-include");
    const Option& opt_exclude = list.find_except("vlan-exclude");
    const std::string& opt_log_per = list.find_except("log-period").arg_value();
    const std::string& opt_cdr_per = list.find_except("cdr-period").arg_value();
    bool opt_log_std = list.find_except("log-std").presented();
    bool opt_cdr_std = list.find_except("cdr-std").presented();
    bool opt_verbose = list.find_except("verbose").presented();
    bool opt_help = list.find_except('h').presented();
    bool opt_version = list.find_except('v').presented();

    // checking input conditions
    if (opt_help) {
        print_usage(bin, list);
        return 0;
    } else if (opt_version) {
        print_version();
        return 0;
    }

    if (opt_include.presented() && opt_exclude.presented()) {
        LOG("Wrong VLAN filtering mode, both of --vlan-include and --vlan-exclude are presented.");
        print_usage(bin, list);
        return 1;
    }

    long log_period;
    long cdr_period;

    if (not utils::Types::str_to_l(opt_log_per, log_period)) {
        LOG("cannt parse long int value '%s'", opt_cdr_per.c_str());
        return 1;
    }

    if (not utils::Types::str_to_l(opt_cdr_per, cdr_period)) {
        LOG("cannt parse long int value '%s'", opt_cdr_per.c_str());
        return 1;
    }
    
    if(opt_verbose){
        dump(stdout, argc, argv, list);
        LOG("log period : %d", log_period);
        LOG("cdr period : %d", cdr_period);
    }

    if (opt_include.has_arg()) {
        printf("INCLUDE:'%s'\n", opt_include.arg_value().c_str());
    }

    std::cout << "<---- the end of main_cli() ---->\n";
    return 0;
}


