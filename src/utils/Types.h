#ifndef UTILS_TYPES_H
#define UTILS_TYPES_H

namespace utils {

class Types {
public:

	static bool str_to_ll(const std::string& str, long long int& value, int base = 0) noexcept {
		const char* strptr = str.c_str();
		char* endptr;
		long long int result = strtoll(strptr, &endptr, base);
		if (size_t(endptr - strptr) == strlen(strptr)) {
			value = result;
			return true;
		}
		return false;
	}

	static bool str_to_l(const std::string& str, long int& value, int base = 0) noexcept {
		const char* strptr = str.c_str();
		char* endptr;
		long int result = strtol(strptr, &endptr, base);
		if (size_t(endptr - strptr) == strlen(strptr)) {
			value = result;
			return true;
		}
		return false;
	}

	static bool str_to_ul(const std::string& str, unsigned long int& value, int base = 0) noexcept {
		const char* strptr = str.c_str();
		char* endptr;
		unsigned long int result = strtoul(strptr, &endptr, base);
		if (size_t(endptr - strptr) == strlen(strptr)) {
			value = result;
			return true;
		}
		return false;
	}



};

}; // namespace utils

#endif /* UTILS_TYPES_H */

