#ifndef OPTION_H
#define OPTION_H

#include <string>
#include <assert.h>
#include <ctype.h>
#include <vector>

namespace cli_helper {

class Option {
public:
    const std::string name;
    const std::string desc;
    const bool required;
    const bool has_args;
    const std::string arg_name;

    /**
     * Creates an option with an argument.
     * @param name - A name of the option, must start with an alphabetic symbol, may contains digits. 
     * @param desc - A description of the options, must not be empty.
     * @param required - The option cannot be omitted.
     * @param arg_name - A name of the option's argument.
     */
    Option(const std::string& name, const std::string desc, bool required, const std::string arg_name)
    : name(name), desc(desc), required(required), has_args(true), arg_name(arg_name)
    {
        validate_name(name);
        validate_string(desc);
        validate_string(arg_name);
    }
    
    /**
     * Creates an option with no arguments.
     * @param name - A name of the option, must start with an alphabetic symbol, may contains digits. 
     * @param desc - A description of the options, must not be empty.
     * @param required - The option cannot be omitted.
     */
    Option(const std::string& name, const std::string desc, bool required)
    : name(name), desc(desc), required(required), has_args(false), arg_name()
    {
        validate_name(name);
        validate_string(desc);
    }
    
    /**
     * Gets the option name with its argument.
     * @return - full name of the option.
     */
    std::string full_name() const
    {
        std::string result(name);
        if(has_args){
            result += "=<";
            result += arg_name;
            result += ">";
        }
        return std::move(result);
    }
    
    bool operator<(const Option& opt) const
    {
        return name < opt.name;
    }

private:
    
    static inline void validate_name(const std::string& name)
    {
        assert(name.length() > 0);
        assert(isalpha(name.c_str()[0]));
        for (unsigned i = 1; i < name.length(); i++) {
            char ch = name.c_str()[i];
            assert(isalnum(ch) || ch == '_');
        }
    }
    
    static inline void validate_string(const std::string& desc)
    {
        assert(desc.length() > 0);
    }
};

}; // namespace cli_helper;

#endif /* OPTION_H */

