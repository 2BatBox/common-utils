#pragma once

#include <cstdlib>
#include <cerrno>
#include <cstring>

namespace cli {

class Float {
public:

	template<typename T>
	static size_t parse_offset(const char* arg, T& value) noexcept {
		static_assert(std::is_floating_point<T>::value, "cli::Float::parse_offset");
		size_t offset = 0;
		if(arg) {
			char* endptr;
			const auto raw_value = std::strtod(arg, &endptr);
			if(errno == 0) {
				value = raw_value;
				offset = endptr - arg;
			}
		}
		return offset;
	}

	template<typename T>
	static bool parse(const char* arg, T& value) noexcept {
		const auto len = strlen(arg);
		const auto read = parse_offset(arg, value);
		return len && read == len;
	}

};

}; // namespace cli
