#pragma once

#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <type_traits>
#include <cstdio>
#include <cerrno>

namespace cli {

class Integer {
public:

	template<typename T>
	static size_t parse_offset(const char* arg, T& value, const unsigned base = 10) noexcept {
		static_assert(std::is_integral<T>::value, "cli::Integer::parse_offset");
		if(std::is_signed<T>::value) {
			return parse_signed(arg, value, base);
		} else {
			return parse_unsigned(arg, value, base);
		}
	}

	template<typename T>
	static bool parse(const char* arg, T& value, const unsigned base = 10) noexcept {
		const auto len = strlen(arg);
		return len && parse_offset(arg, value, base) == len;
	}

private:

	template<typename T>
	static size_t parse_unsigned(const char* arg, T& value, const unsigned base = 10) noexcept {
		constexpr T max = T(~0ull);
		size_t offset = 0;
		if(arg && base) {
			char* endptr;
			const auto raw_value = std::strtoull(arg, &endptr, base);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
			if(raw_value <= max && errno == 0) {
#pragma GCC diagnostic pop
				value = raw_value;
				offset = endptr - arg;
			}
		}
		return offset;
	}

	template<typename T>
	static size_t parse_signed(const char* arg, T& value, const unsigned base = 10) noexcept {
		constexpr T max = ~(~0ull << ((sizeof(T) << 3ull) - 1ull));
		size_t offset = 0;
		if(arg && base) {
			char* endptr;
			const auto raw_value = std::strtoll(arg, &endptr, base);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
			if(raw_value <= max && errno == 0) {
#pragma GCC diagnostic pop
				value = raw_value;
				offset = endptr - arg;
			}
		}
		return offset;
	}
};

}; // namespace cli
