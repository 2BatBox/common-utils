#ifndef CLIHELPER_H
#define CLIHELPER_H

#include <cstdio>
#include <set>
#include <map>

#include "Option.h"

namespace cli_helper {

class CliHelper {
    std::string binary;
    std::set<Option> opts;
    std::map<std::string, std::string> values;
    
public:
    /**
     * @param binary - The name of the binary file.
     */
    CliHelper(const char* binary) : binary(binary), opts(), values()
    {
        assert(binary);
    }
    
    /**
     * Appends an option to the helper.
     * Options with the same name cannot be appended.
     * @param opt
     * @return 
     */
    CliHelper& append(const Option& opt)
    {
        assert(opts.find(opt) == opts.cend());
        opts.insert(opt);
        return *this;
    }

    void print_usage(FILE* file = stdout) const
    {
        unsigned max_len = 0;
        if (not opts.empty()) {
            std::string name_req = option_full_names(true, &max_len);
            std::string name_add = option_full_names(false, &max_len);
            fprintf(file, "usage: %s %s [%s]\n", binary.c_str(), name_req.c_str(), name_add.c_str());
            
            for (auto it = opts.cbegin(); it != opts.cend(); ++it) {
                const Option& opt = *it;
                fprintf(file, "  %-*s  %s\n", max_len, opt.full_name().c_str(), opt.desc.c_str());
            }
        } else {
            fprintf(file, "usage: %s\n", binary.c_str());
        }
    }
    
    int parse_args(int argc, char** argv, FILE* file = stdout)
    {
        for (int i = 1; i < argc; i++) {
            std::string opt(argv[i]);
            std::string name;
            std::string value;
            bool has_arg = false;
            
            auto div_pos = opt.find("=");
            if(div_pos != std::string::npos){
                name = opt.substr(0, div_pos);
                value = opt.substr(div_pos + 1);
                has_arg = true;
            } else {
                name = opt;
            }
            if(put_value(file, name, value, has_arg) < 0)
                return -1;
        }
        return validate_values(file);
    }
    
    bool has_value(const std::string& name) const
    {
        return values.find(name) != values.cend();
    }
    
    std::string get_value(const std::string& name) const
    {
        auto it = values.find(name);
        assert(it != values.cend());
        return (*it).second;
    }
    
    std::string get_value(const std::string& name, const std::string& default_value) const
    {
        auto it = values.find(name);
        if(it == values.cend())
            return default_value;
        return (*it).second;
    }

private:
    int put_value(FILE* file, std::string name, std::string value, bool has_arg)
    {
        Option tmp(name, "_", false);
        auto it = opts.find(tmp);
        
        if(it == opts.cend()){
            fprintf(file, "unknown option \"%s\"\n", name.c_str());
            return -1;
        }
        
        const Option& opt = *it;
        if(opt.has_args != has_arg){
            if(opt.has_args)
                fprintf(file, "option \"%s\" must have value\n", name.c_str());
            else
                fprintf(file, "option \"%s\" must not have value\n", name.c_str());
            return -1;
        } else if(opt.has_args && value.length() == 0){
            fprintf(file, "option \"%s\" must have value\n", name.c_str());
            return -1;
        } else if(values.find(name) != values.cend()){
            fprintf(file, "option \"%s\" has been set more than once\n", name.c_str());
            return -1;
        }
        
        values.insert(std::pair<std::string, std::string>(name, value));
        return 0;
    }
    
    int validate_values(FILE* file)
    {
        int result = 0;
        for (auto it = opts.cbegin(); it != opts.cend(); ++it) {
            const Option& opt = *it;
            if(opt.required && values.find(opt.name) == values.cend()){
                fprintf(file, "option \"%s\" has been missed\n", opt.name.c_str());
                result = -1;
            }
        }
        return result;
    }
    
    std::string option_full_names(bool required, unsigned* max_len) const
    {
        std::string result;
        for (auto it = opts.cbegin(); it != opts.cend(); ++it) {
            const Option& opt = *it;
            if(opt.required == required){
                if(result.length() > 0)
                    result += ",";
                result += opt.name;
                
                std::string full_name = opt.full_name();
                if(full_name.length() > *max_len)
                    *max_len = full_name.length();
            }
        }
        return std::move(result);
    }

};

}; // namespace cli_helper;

#endif /* CLIHELPER_H */

