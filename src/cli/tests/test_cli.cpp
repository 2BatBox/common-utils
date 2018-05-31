#include <iostream>
#include <cstdio>

#include "../OptionList.h"
#include "../Formatter.h"
#include "../Config.h"

using namespace cli;

int main_cli(int, char**) {
	
	OptionList list;
	Option& opt_short = list.add_multi('B', "buffer-size", true);
	opt_short.required(true).arg_name("buffer_size");
	opt_short.description("Set the operating system capture buffer size to buffer_size, in units of KiB (1024 bytes).");
	opt_short.description("in units of KiB (1024 bytes).");
	
	list.add_multi('D', "list-interfaces", true).description("Print the list of the network interfaces available on the system and on which tcpdump can capture packets.");
	
	list.add_short('A', false).description("Print each packet (minus its link level header) in ASCII.  Handy for capturing web pages.");
	list.add_multi('s', "sss-w", true).description("Use 'blkid' to print the universally unique identifier for a");
	list.add_multi('e', "sss--w", true).description("# that works even if disks are added and removed.\nSee fstab(5).# device; this may be used with UUID= as a more robust way to name devices");
	
	list.add_long("type-config", ArgumentType::OPTIONAL).description("# that works even if disks are added and removed.\nSee fstab(5).# device; this may be used with UUID= as a more robust way to name devices");
	
	list.add_multi('h', "help", false).description("Print this screen and exit");
	list.add_multi('v', "version", false).description("Print version and exit");
	
	Config config;
	Formatter form(config);
	form.print_options(list, stdout);
	
	std::cout << "<---- the end of main_cli() ---->\n";
	return 0;
}


