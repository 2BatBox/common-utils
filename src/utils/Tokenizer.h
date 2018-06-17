#ifndef UTILS_TOKENIZER_H
#define UTILS_TOKENIZER_H

#include <string>

namespace utils {

class Tokenizer {
    std::string m_string;
    std::string m_splitter;
    size_t m_current;
    
public:
    
    Tokenizer(const std::string& string, const std::string& splitter) noexcept : m_string(string), m_splitter(splitter), m_current(0) {
    }
    
    bool has_next() noexcept {
        return m_current != std::string::npos;
    }
    
    std::string next() noexcept {
        auto new_position = m_string.find_first_of(m_splitter, m_current);
        std::string result;
        if(new_position == std::string::npos){
            result = m_string.substr(m_current);
            m_current = new_position;
        } else {
            result = m_string.substr(m_current, new_position - m_current);
            m_current = new_position + 1;
        }
        return std::move(result);
    }

};

}; // namespace utils

#endif /* UTILS_TOKENIZER_H */

