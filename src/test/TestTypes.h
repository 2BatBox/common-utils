#pragma once

#include "test_environment.h"
#include <utils/Types.h>

class TestTypes {

	template<typename T>
	void test_parse_signed() noexcept {
		T value;

		// not a number
		assert(not utils::Types::parse_signed("", value));
		assert(not utils::Types::parse_signed("#", value));
		assert(not utils::Types::parse_signed("0#", value));
		assert(not utils::Types::parse_signed("#0", value));

		// oct
		assert(utils::Types::parse_signed("00", value) && value == 0);
		assert(utils::Types::parse_signed("-00", value) && value == 0);
		assert(utils::Types::parse_signed("01", value) && value == 1);
		assert(utils::Types::parse_signed("-01", value) && value == -1);
		assert(utils::Types::parse_signed("0177", value) && value == 127);
		assert(utils::Types::parse_signed("-0200", value) && value == -128);
		assert(utils::Types::parse_signed("0000000000000", value) && value == 0);

		// dec
		assert(utils::Types::parse_signed("0", value) && value == 0);
		assert(utils::Types::parse_signed("-0", value) && value == 0);
		assert(utils::Types::parse_signed("1", value) && value == 1);
		assert(utils::Types::parse_signed("-1", value) && value == -1);
		assert(utils::Types::parse_signed("127", value) && value == 127);
		assert(utils::Types::parse_signed("-128", value) && value == -128);

		// hex
		assert(utils::Types::parse_signed("0x0", value) && value == 0);
		assert(utils::Types::parse_signed("-0x0", value) && value == 0);
		assert(utils::Types::parse_signed("0x1", value) && value == 1);
		assert(utils::Types::parse_signed("-0x1", value) && value == -1);
		assert(utils::Types::parse_signed("0x7F", value) && value == 127);
		assert(utils::Types::parse_signed("-0x80", value) && value == -128);
		assert(utils::Types::parse_signed("0x0000000000000", value) && value == 0x0);

		// limits
		constexpr auto min_val = std::numeric_limits<T>::lowest();
		constexpr auto max_val = std::numeric_limits<T>::max();

		const auto min_str = std::to_string(min_val);
		const auto max_str = std::to_string(max_val);
		assert(utils::Types::parse_signed(min_str.c_str(), value) && value == min_val);
		assert(utils::Types::parse_signed(max_str.c_str(), value) && value == max_val);
	}

	template<typename T>
	void test_parse_unsigned() noexcept {
		T value;

		// not a number
		assert(not utils::Types::parse_unsigned("", value));
		assert(not utils::Types::parse_unsigned("#", value));
		assert(not utils::Types::parse_unsigned("0#", value));
		assert(not utils::Types::parse_unsigned("#0", value));

		// oct
		assert(not utils::Types::parse_unsigned("-00", value));
		assert(not utils::Types::parse_unsigned("-01", value));
		assert(not utils::Types::parse_unsigned("-0200", value));
		assert(not utils::Types::parse_unsigned("-0377", value));
		assert(utils::Types::parse_unsigned("00", value) && value == 0);
		assert(utils::Types::parse_unsigned("01", value) && value == 1);
		assert(utils::Types::parse_unsigned("0177", value) && value == 127);
		assert(utils::Types::parse_unsigned("0377", value) && value == 255);
		assert(utils::Types::parse_unsigned("0000000000000", value) && value == 0);

		// dec
		assert(not utils::Types::parse_unsigned("-0", value));
		assert(not utils::Types::parse_unsigned("-1", value));
		assert(not utils::Types::parse_unsigned("-127", value));
		assert(not utils::Types::parse_unsigned("-128", value));
		assert(not utils::Types::parse_unsigned("-255", value));
		assert(utils::Types::parse_unsigned("0", value) && value == 0);
		assert(utils::Types::parse_unsigned("1", value) && value == 1);
		assert(utils::Types::parse_unsigned("127", value) && value == 127);
		assert(utils::Types::parse_unsigned("255", value) && value == 255);

		// hex
		assert(not utils::Types::parse_unsigned("-0x00", value));
		assert(not utils::Types::parse_unsigned("-0x01", value));
		assert(not utils::Types::parse_unsigned("-0x7F", value));
		assert(not utils::Types::parse_unsigned("-0xFF", value));
		assert(utils::Types::parse_unsigned("0x0", value) && value == 0);
		assert(utils::Types::parse_unsigned("0x1", value) && value == 1);
		assert(utils::Types::parse_unsigned("0x7F", value) && value == 127);
		assert(utils::Types::parse_unsigned("0xFF", value) && value == 255);
		assert(utils::Types::parse_unsigned("0x0000000000000", value) && value == 0);

		// limits
		constexpr auto min_val = std::numeric_limits<T>::lowest();
		constexpr auto max_val = std::numeric_limits<T>::max();

		const auto min_str = std::to_string(min_val);
		const auto max_str = std::to_string(max_val);
		assert(utils::Types::parse_unsigned(min_str.c_str(), value) && value == min_val);
		assert(utils::Types::parse_unsigned(max_str.c_str(), value) && value == max_val);
	}

	template<typename T>
	bool test_float_cmp(const T x, const long double y) noexcept {
		static_assert(std::is_floating_point<T>::value);
		return (x < y + std::numeric_limits<T>::epsilon()) && (x > y - std::numeric_limits<T>::epsilon());
	}

	template<typename T>
	void test_parse_float() noexcept {
		T value;

		// not a number
		assert(not utils::Types::parse_float("", value));
		assert(not utils::Types::parse_float("#", value));
		assert(not utils::Types::parse_float("0#", value));
		assert(not utils::Types::parse_float("#0", value));

		assert(utils::Types::parse_float("0", value) && test_float_cmp(value, 0.0l));
		assert(utils::Types::parse_float("0.", value) && test_float_cmp(value, 0.0l));
		assert(utils::Types::parse_float(".0", value) && test_float_cmp(value, 0.0l));

		assert(utils::Types::parse_float("-0", value) && test_float_cmp(value, 0.0l));
		assert(utils::Types::parse_float("-0.", value) && test_float_cmp(value, 0.0l));
		assert(utils::Types::parse_float("-.0", value) && test_float_cmp(value, 0.0l));

		assert(utils::Types::parse_float("1", value) && test_float_cmp(value, 1.0l));
		assert(utils::Types::parse_float("1.", value) && test_float_cmp(value, 1.0l));
		assert(utils::Types::parse_float(".1", value) && test_float_cmp(value, 0.1l));

		assert(utils::Types::parse_float("-1", value) && test_float_cmp(value, -1.0l));
		assert(utils::Types::parse_float("-1.", value) && test_float_cmp(value, -1.0l));
		assert(utils::Types::parse_float("-.1", value) && test_float_cmp(value, -0.1l));
	}

	void test_signed() noexcept {
		TRACE_CALL;
		test_parse_signed<int8_t>();
		test_parse_signed<int16_t>();
		test_parse_signed<int32_t>();
		test_parse_signed<int64_t>();
		test_parse_signed<signed char>();
		test_parse_signed<signed short>();
		test_parse_signed<signed int>();
		test_parse_signed<signed long int>();
		test_parse_signed<signed long long int>();
	}

	void test_unsigned() noexcept {
		TRACE_CALL;
		test_parse_unsigned<uint8_t>();
		test_parse_unsigned<uint16_t>();
		test_parse_unsigned<uint32_t>();
		test_parse_unsigned<uint64_t>();
		test_parse_unsigned<unsigned char>();
		test_parse_unsigned<unsigned short>();
		test_parse_unsigned<unsigned int>();
		test_parse_unsigned<unsigned long int>();
		test_parse_unsigned<unsigned long long int>();
	}

	void test_float() noexcept {
		TRACE_CALL;
		test_parse_float<float>();
		test_parse_float<double>();
		test_parse_float<long double>();
	}

public:

	explicit TestTypes() noexcept {
		test_signed();
		test_unsigned();
		test_float();
	}
};
