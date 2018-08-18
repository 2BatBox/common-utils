#ifndef UTILS_TYPES_H
#define UTILS_TYPES_H

#include <cstdio>
#include <string>

namespace utils {

class Types {
public:

	// long long int

	static long long int str_to_ll(const std::string& str, int base = 0) throw (std::logic_error) {
		long long int result;
		if (not str_to_ll(str, result, base)) {
			fprintf(stderr, "cannot parse long long int value '%s'\n", str.c_str());
			throw std::logic_error("cannot parse long long int value");
		}
		return result;
	}

	static bool str_to_ll(const std::string& str, long long int& value, int base = 0) noexcept {
		const char* strptr = str.c_str();
		char* endptr;
		long long int result = strtoll(strptr, &endptr, base);
		if (str.size() && size_t(endptr - strptr) == strlen(strptr)) {
			value = result;
			return true;
		}
		return false;
	}

	// long int

	static long int str_to_l(const std::string& str, int base = 0) throw (std::logic_error) {
		long int result;
		if (not str_to_l(str, result, base)) {
			fprintf(stderr, "cannot parse long int value '%s'\n", str.c_str());
			throw std::logic_error("cannot parse long int value");
		}
		return result;
	}

	static bool str_to_l(const std::string& str, long int& value, int base = 0) noexcept {
		const char* strptr = str.c_str();
		char* endptr;
		long int result = strtol(strptr, &endptr, base);
		if (str.size() && size_t(endptr - strptr) == strlen(strptr)) {
			value = result;
			return true;
		}
		return false;
	}

	// unsigned long int

	static unsigned long int str_to_ul(const std::string& str, int base = 0) throw (std::logic_error) {
		unsigned long int result;
		if (not str_to_ul(str, result, base)) {
			fprintf(stderr, "cannot parse unsigned long int value '%s'\n", str.c_str());
			throw std::logic_error("cannot parse unsigned long int value");
		}
		return result;
	}

	static bool str_to_ul(const std::string& str, unsigned long int& value, int base = 0) noexcept {
		const char* strptr = str.c_str();
		char* endptr;
		unsigned long int result = strtoul(strptr, &endptr, base);
		if (str.size() && size_t(endptr - strptr) == strlen(strptr)) {
			value = result;
			return true;
		}
		return false;
	}

};

}; // namespace utils

#endif /* UTILS_TYPES_H */

