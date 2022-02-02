#pragma once

#include <string>
#include <cstring>
#include <limits>

namespace utils {

class Types {
public:

	template<typename T>
	inline static bool parse_signed(const char* str, T& value) noexcept {
		using SLLI = signed long long int;
		static_assert(std::is_integral<T>::value);
		static_assert(std::is_signed<T>::value);
		static_assert(sizeof(SLLI) >= sizeof(value));
		constexpr SLLI VMin = std::numeric_limits<T>::lowest();
		constexpr SLLI VMax = std::numeric_limits<T>::max();

		char* end;
		errno = EXIT_SUCCESS;
		const SLLI slli = strtoll(str, &end, 0);
		bool result = (str != end) && (errno == EXIT_SUCCESS) && (*end == '\0') && (slli >= VMin) && (slli <= VMax);
		if(result) {
			value = T(slli);
		}
		return result;
	}

	template<typename T>
	inline static bool parse_unsigned(const char* str, T& value) noexcept {
		using ULLI = unsigned long long int;
		static_assert(std::is_integral<T>::value);
		static_assert(std::is_unsigned<T>::value);
		static_assert(sizeof(ULLI) >= sizeof(value));
		constexpr ULLI VMax = std::numeric_limits<T>::max();

		// 'strtoull()' does accept negative value
		if(strchr(str, '-')) {
			return false;
		}

		char* end;
		errno = EXIT_SUCCESS;
		const ULLI ulli = strtoull(str, &end, 0);
		bool result = (str != end) && (errno == EXIT_SUCCESS) && (*end == '\0') && (ulli <= VMax);
		if(result) {
			value = T(ulli);
		}
		return result;
	}

	template<typename T>
	inline static bool parse_float(const char* str, T& value) noexcept {
		using LD = long double;
		static_assert(std::is_floating_point<T>::value);
		static_assert(sizeof(LD) >= sizeof(value));

		char* end;
		errno = EXIT_SUCCESS;
		const LD ld = strtold(str, &end);
		bool result = (str != end) && (errno == EXIT_SUCCESS) && (*end == '\0');
		if(result) {
			value = T(ld);
		}
		return result;
	}

};

}; // namespace utils

