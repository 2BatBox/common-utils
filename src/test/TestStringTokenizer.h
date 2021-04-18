#pragma once

#include "test_environment.h"
#include <tio/StringTokenizer.h>
#include <cstring>

class TestStringTokenizer {

public:

	TestStringTokenizer() noexcept {
		test_empty_input();
		test_empty_separator_seq();
		test_overflow_accumulator();
		test_separators();
	}

private:

	void test_empty_input() noexcept {
		TRACE_CALL;

		constexpr size_t Capacity = 32;
		const char* input = "";
		const char* separators = " \t\n";

		tio::StringTokenizer<Capacity> stk(input, separators);
		assert(not stk.next());
	}

	void test_empty_separator_seq() noexcept {
		TRACE_CALL;

		constexpr size_t Capacity = 32;
		const char* input = "void* vptr = nullptr;";
		const char* separators = "";

		tio::StringTokenizer<Capacity> stk(input, separators);
		assert(stk.next());
		assert(strcmp(stk.token(), "void* vptr = nullptr;") == 0);
		assert(not stk.next());
	}

	void test_overflow_accumulator() noexcept {
		TRACE_CALL;

		constexpr size_t Capacity = 3; // two chars + zero byte
		const char* input = "a bb ccc dddd";
		const char* separators = " ";

		tio::StringTokenizer<Capacity> stk(input, separators);
		assert(stk.next());
		assert(strcmp(stk.token(), "a") == 0);
		assert(not stk.overflown());

		assert(stk.next());
		assert(strcmp(stk.token(), "bb") == 0);
		assert(not stk.overflown());

		assert(stk.next());
		assert(strcmp(stk.token(), "cc") == 0);
		assert(stk.overflown());

		assert(stk.next());
		assert(strcmp(stk.token(), "c") == 0);
		assert(not stk.overflown());

		assert(stk.next());
		assert(strcmp(stk.token(), "dd") == 0);
		assert(stk.overflown());

		assert(stk.next());
		assert(strcmp(stk.token(), "dd") == 0);
		assert(not stk.overflown());
		assert(not stk.next());
	}

	void test_separators() noexcept {
		TRACE_CALL;

		constexpr size_t Capacity = 32;
		const char* input = " This is\n\ta token sequence.\t \t";
		const char* separators = " \t\n";

		tio::StringTokenizer<Capacity> stk(input, separators);
		assert(stk.next());
		assert(strcmp(stk.token(), "This") == 0);
		assert(stk.next());
		assert(strcmp(stk.token(), "is") == 0);
		assert(stk.next());
		assert(strcmp(stk.token(), "a") == 0);
		assert(stk.next());
		assert(strcmp(stk.token(), "token") == 0);
		assert(stk.next());
		assert(strcmp(stk.token(), "sequence.") == 0);
		assert(not stk.next());
	}

};
