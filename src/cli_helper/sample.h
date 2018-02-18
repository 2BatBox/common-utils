#ifndef CLI_HELPER_SAMPLE_H
#define CLI_HELPER_SAMPLE_H

#include <thread>
#include <vector>
#include <cstring>
#include <stdio.h>

#include "CliHelper.h"

using namespace cli_helper;

const char* COMMAND_DESC =
"Commands \n"
"\tfrag - \n"
"\tdefrag - \n"
"\tel2 - \n"
"\tel3 - ";

void append_options(CliHelper& helper)
{
    Option opt_cmd("cmd", COMMAND_DESC, true, "command");
    Option opt_input("input", "Input file", true, "file");
    Option opt_ouput("output", "Ouput file", true, "file");
    
    Option opt_mtu("mtu", "MTU value", false, "mtu");
    Option opt_skip("s", "Skip fragmented packets", false, "mtu");
    Option opt_verbose("verbose", "Verbose output", false);

    helper.append(opt_cmd).append(opt_input).append(opt_ouput);
    helper.append(opt_mtu).append(opt_skip).append(opt_verbose);
}

int sample_main(int argc, char** argv)
{

    CliHelper helper(argv[0]);
    append_options(helper);
    
    if(helper.parse_args(argc, argv) < 0){
        helper.print_usage();
        return -1;
    }
    
    std::string cmd = helper.get_value("cmd");
    std::string input = helper.get_value("input");
    std::string output = helper.get_value("output");
    std::string mtu = helper.get_value("mtu", "1500");
    bool verbose = helper.has_value("verbose");
    
    printf("cmd: \"%s\"\n", cmd.c_str());
    printf("input: \"%s\"\n", input.c_str());
    printf("output: \"%s\"\n", output.c_str());
    printf("mtu: \"%s\"\n", mtu.c_str());
    printf("verbose: %d\n", verbose);
    
    printf("<---- the end of sample_main() ---->");
    return 0;
}


#endif /* CLI_HELPER_SAMPLE_H */

