#include <iostream>
#include <cstdio>
#include <vector>

#include "../OptionList.h"
#include "../Formatter.h"
#include "../Config.h"
#include "../Parser.h"

#include "../../Log.h"

using namespace cli;

class DummyHandler : public EventHandler {
    OptionList& m_list;
    Parser m_parser;
    Option* m_short;
    std::vector<std::string> arguments;

public:

    DummyHandler(OptionList& list) noexcept :
    m_list(list), m_parser(), m_short(nullptr), arguments() {
    }

    virtual void begin() throw (std::logic_error) {
        LOG("begin()");
        m_list.clean_values();
        m_short = nullptr;
    }

    virtual void end() throw (std::logic_error) {
        LOG("end()");
        m_list.validate();
    }

    virtual void start_short(char opt) throw (std::logic_error) {
        LOG("start_short('%c')", opt);
        m_short = m_list.find_short_name(opt);
        if (m_short == nullptr) {
            fprintf(stderr, "unknown short option '%c'\n", opt);
            throw std::logic_error("unknown short option");
        }
        m_short->value().set_presented();
    }

    virtual bool start_short(char opt, std::string arg) throw (std::logic_error) {
        LOG("start_short('%c', '%s')", opt, arg.c_str());
        m_short = m_list.find_short_name(opt);
        if (m_short == nullptr) {
            fprintf(stderr, "unknown short option '%c'\n", opt);
            throw std::logic_error("unknown short option");
        }

        switch (m_short->arg_type()) {
            case ArgumentType::MANDATORY:
                m_short->value().set_argument(arg);
                return false;

            case ArgumentType::NONE:
                m_short->value().set_presented();
                return true;

            default:
                break;
        }
        return true;

    }

    virtual void start_long(std::string opt) throw (std::logic_error) {
        LOG("start_long('%s')", opt.c_str());
        Option* long_opt = m_list.find_long_name(opt);
        if (long_opt == nullptr) {
            fprintf(stderr, "unknown long option '%s'\n", opt.c_str());
            throw std::logic_error("unknown long option");
        }
        if (long_opt->arg_type() == ArgumentType::MANDATORY) {
            fprintf(stderr, "long option '%s' must have an argument\n", opt.c_str());
            throw std::logic_error("long option must have an argument");
        }
        long_opt->value().set_presented();
    }

    virtual void start_long(std::string opt, std::string arg) throw (std::logic_error) {
        LOG("start_long('%s', '%s')", opt.c_str(), arg.c_str());
        Option* long_opt = m_list.find_long_name(opt);
        if (long_opt == nullptr) {
            fprintf(stderr, "unknown long option '%s'\n", opt.c_str());
            throw std::logic_error("unknown long option");
        }
        if (long_opt->arg_type() == ArgumentType::NONE) {
            fprintf(stderr, "long option '%s' cannot have an argument\n", opt.c_str());
            throw std::logic_error("long option cannot have an argument");
        }
        long_opt->value().set_argument(arg);
    }

    virtual void start_argument(std::string arg) throw (std::logic_error) {
        LOG("start_argument('%s')", arg.c_str());
        if (m_short != nullptr) {
            m_short->value().set_argument(arg);
        } else {
            arguments.push_back(arg);
        }
    }

    void parse(int argc, char** argv) throw (std::logic_error) {
        m_parser.parse(argc, argv, this);
    }

private:

    void short_option(char opt, std::string arg) throw (std::logic_error) {
        LOG("short_option('%c', '%s')", opt, arg.c_str());
        m_short = m_list.find_short_name(opt);
        if (m_short == nullptr) {
            fprintf(stderr, "unknown short option '%c'\n", opt);
            throw std::logic_error("unknown short option");
        };
    }

};

int main(int argc, char** argv) {

    OptionList list;
    Option& opt_short = list.add_multi('B', "buffer-size", true);
    opt_short.required(true).arg_name("buffer_size");
    opt_short.description("Set the operating system capture buffer size to buffer_size, in units of KiB (1024 bytes).");
    opt_short.description("in units of KiB (1024 bytes).");

    list.add_multi('D', "list-interfaces", true).description("Print the list of the network interfaces available on the system and on which tcpdump can capture packets.");

    list.add_short('A', false).description("Print each packet (minus its link level header) in ASCII.  Handy for capturing web pages.");
    list.add_multi('l', "long", true).description("Use 'blkid' to print the universally unique identifier for a");
    list.add_multi('d', "long-long", true).description("# that works even if disks are added and removed.\nSee fstab(5).# device; this may be used with UUID= as a more robust way to name devices");

    list.add_long("type-config", ArgumentType::OPTIONAL).description("# that works even if disks are added and removed.\nSee fstab(5).# device; this may be used with UUID= as a more robust way to name devices");

    list.add_multi('h', "help", false).description("Print this screen and exit");
    list.add_multi('v', "version", false).description("Print version and exit");

    Formatter form;
    form.print_options(list, stdout);

    DummyHandler dummy(list);
    dummy.parse(argc, argv);

    list.dump(stdout);

    std::cout << "<---- the end of main_cli() ---->\n";
    return 0;
}


