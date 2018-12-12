#ifndef BINIO_TESTS_TEST_MAREA_H
#define BINIO_TESTS_TEST_MAREA_H

#include <assert.h>
#include <memory>
#include <iostream>
#include <cstdio>

#include "../../../src/binio/MArea.h"

namespace binio {

class TestMArea {
	using Readable = MCArea;
	using Writable = MArea;

	struct DataSet {
		char c;
		short s;
		int i;
		long l;

		DataSet() noexcept : c(0), s(0), i(0), l(0) {}

		DataSet(int value) noexcept : c((char) value), s((short) value), i(value), l(value << 16) {}

		DataSet(char c, short s, int i, long l) noexcept : c(c), s(s), i(i), l(l) {}

		bool operator==(const DataSet& ds) noexcept {
			return c == ds.c && s == ds.s && i == ds.i && l == ds.l;
		}
	};

	static void test_length() noexcept {
		printf("-> test_length()\n");
		const void* const_void_ptr = nullptr;
		void* void_ptr = nullptr;

		const uint32_t* const_u32_ptr = nullptr;
		uint32_t* u32_ptr = nullptr;

		const DataSet* c_struct_ptr = nullptr;
		DataSet* struct_ptr = nullptr;

		Readable readable;
		Writable writable;

		readable = as_const_area(const_void_ptr, 1);
		assert(readable.length() == 1);
		readable = as_const_area(const_u32_ptr, 1);
		assert(readable.length() == sizeof(*const_u32_ptr));
		readable = as_const_area(c_struct_ptr, 1);
		assert(readable.length() == sizeof(*c_struct_ptr));

		writable = as_area(void_ptr, 1);
		assert(writable.length() == 1);
		writable = as_area(u32_ptr, 1);
		assert(writable.length() == sizeof(*u32_ptr));
		writable = as_area(struct_ptr, 1);
		assert(writable.length() == sizeof(*struct_ptr));

	}

	static void dummy_readable(Readable) {}

	static void dummy_readable_ref(Readable&) {}

	static void dummy_readable_cref(const Readable&) {}

	static void dummy_writable(Writable) {}

	static void dummy_writable_ref(Writable&) {}

	static void dummy_writable_cref(const Writable&) {}

	static void test_type_converting() noexcept {
		printf("-> test_type_converting()\n");
		Readable readable;
		Writable writable;

		dummy_readable(readable);
		dummy_readable_ref(readable);
		dummy_readable_cref(readable);

		dummy_readable(writable);
		//dummy_readable_ref(writable); no const reference, not allowed
		dummy_readable_cref(writable);

		dummy_writable(writable);
		dummy_writable_ref(writable);
		dummy_writable_cref(writable);

		readable = writable;

		// data comparison
		if(readable == readable && readable == writable && writable == writable) {
		}
	}

	static void test_comparison() noexcept {
		printf("-> test_comparison()\n");
		unsigned buf_size = 64 - 1;
		DataSet array_struc_first[buf_size];
		DataSet array_struc_second[buf_size];
		for(unsigned i = 0; i < buf_size; i++) {
			array_struc_first[i] = DataSet(i);
			array_struc_second[i] = DataSet(i);
		}

		Readable readable_null;
		Readable writable_null;

		Readable readable_first = as_const_area(array_struc_first, buf_size);
		Readable readable_second = as_const_area(array_struc_second, buf_size);

		Writable writable_first = as_area(array_struc_first, buf_size);
		Writable writable_second = as_area(array_struc_second, buf_size);

		assert(readable_first != readable_null);
		assert(readable_second != readable_null);
		assert(writable_first != readable_null);

		assert(readable_first == readable_second);
		assert(writable_first == writable_second);
		assert(readable_first == writable_first);

		array_struc_second[buf_size - 1] = DataSet(0);

		assert(readable_first != readable_second);
		assert(writable_first != writable_second);
	}

	static void test_subarea() noexcept {
		printf("-> test_subarea()\n");
		unsigned buf_size = 16;
		char raw_buffer[buf_size];

		Readable readable = as_area(raw_buffer, buf_size);
		assert(readable == readable.subarea(0));
		assert(readable == readable.subarea(0, buf_size));
		assert(readable.subarea(0) == readable.subarea(0, buf_size));
		assert(readable.subarea(1) == readable.subarea(1, buf_size - 1));
		assert(readable.subarea(buf_size) == readable.subarea(buf_size, 0));
		assert(readable.subarea(buf_size - 1) == readable.subarea(buf_size - 1, 1));

		readable.subarea(0, buf_size);
		try {
			assert(readable.subarea(buf_size + 1));
			assert(false);
		} catch (const std::out_of_range& e) {
		}
		try {
			assert(readable.subarea(0, buf_size + 1));
			assert(false);
		} catch (const std::out_of_range& e) {
		}
	}

public:

	static void test() {
		test_length();
		test_type_converting();
		test_comparison();
		test_subarea();
	}

};

}; // namespace binio

#endif /* BINIO_TESTS_TEST_MAREA_H */