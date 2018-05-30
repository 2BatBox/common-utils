#ifndef BINIO_TESTS_TEST_BYTE_BUFFER_H
#define BINIO_TESTS_TEST_BYTE_BUFFER_H

#include <assert.h>
#include <memory>
#include <iostream>
#include <cstdio>

#include "../ByteBuffer.h"

namespace binio {

class TestByteBuffer {
	using Readable = ByteConstBuffer;
	using Writable = ByteBuffer;

	using Readable8 = BasicBuffer<const uint8_t>;
	using Writable8 = BasicBuffer<uint8_t>;
	using Readable16 = BasicBuffer<const uint16_t>;
	using Writable16 = BasicBuffer<uint16_t>;
	using Readable32 = BasicBuffer<const uint32_t>;
	using Writable32 = BasicBuffer<uint32_t>;
	using Readable64 = BasicBuffer<const uint64_t>;
	using Writable64 = BasicBuffer<uint64_t>;

	struct DataSet {
		char c;
		short s;
		int i;
		long l;

		DataSet() noexcept : c(0), s(0), i(0), l(0) { }

		DataSet(int value) noexcept : c((char)value), s((short)value), i(value), l(value << 16) { }

		DataSet(char c, short s, int i, long l) noexcept : c(c), s(s), i(i), l(l) { }

		bool operator==(const DataSet& ds) noexcept {
			return c == ds.c && s == ds.s && i == ds.i && l == ds.l;
		}
	};

	static void test_length() noexcept {
		const void* const_void_ptr = nullptr;
		void* void_ptr = nullptr;

		const uint32_t* const_u32_ptr = nullptr;
		uint32_t* u32_ptr = nullptr;

		const DataSet* c_struct_ptr = nullptr;
		DataSet* struct_ptr = nullptr;

		Readable readable;
		Writable writable;

		readable = as_const_buffer(const_void_ptr, 1);
		assert(readable.length() == 1);
		readable = as_const_buffer(const_u32_ptr, 1);
		assert(readable.length() == sizeof (*const_u32_ptr));
		readable = as_const_buffer(c_struct_ptr, 1);
		assert(readable.length() == sizeof (*c_struct_ptr));

		writable = as_buffer(void_ptr, 1);
		assert(writable.length() == 1);
		writable = as_buffer(u32_ptr, 1);
		assert(writable.length() == sizeof (*u32_ptr));
		writable = as_buffer(struct_ptr, 1);
		assert(writable.length() == sizeof (*struct_ptr));

		Readable32 readable32(const_u32_ptr, 1);
		Writable32 writable32(u32_ptr, 1);

		readable = as_const_buffer(readable32);
		assert(readable.length() == sizeof (*const_u32_ptr));

		writable = as_buffer(writable32);
		assert(readable.length() == sizeof (*u32_ptr));
	}

	static void dummy_readable(Readable) { }

	static void dummy_readable_ref(Readable&) { }

	static void dummy_readable_cref(const Readable&) { }

	static void dummy_writable(Writable) { }

	static void dummy_writable_ref(Writable&) { }

	static void dummy_writable_cref(const Writable&) { }

	static void test_type_converting() noexcept {
		Readable readable;
		Writable writable;

		Readable32 readable32;
		Writable32 writable32;

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
		readable32 = writable32;
		//readable = readable32; implicit type converting, not allowed
		//writable = writable32; implicit type converting, not allowed
		readable = as_const_buffer(readable32); // explicit type converting, allowed
		writable = as_buffer(writable32); // explicit type converting, allowed

		// data comparison
		if (readable == readable32) {
		}
		if (readable == writable32) {
		}
	}

	static void test_comparison() noexcept {
		unsigned buf_size = 64 - 1;
		DataSet array_struc_first[buf_size];
		DataSet array_struc_second[buf_size];
		for (unsigned i = 0; i < buf_size; i++) {
			array_struc_first[i] = DataSet(i);
			array_struc_second[i] = DataSet(i);
		}

		Readable readable_null;
		Readable writable_null;

		Readable readable_first = as_const_buffer(array_struc_first, buf_size);
		Readable readable_second = as_const_buffer(array_struc_second, buf_size);

		Writable writable_first = as_buffer(array_struc_first, buf_size);
		Writable writable_second = as_buffer(array_struc_second, buf_size);

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

public:

	static void test() {
		test_length();
		test_type_converting();
		test_comparison();
	}

};

}; // namespace binio

#endif /* BINIO_TESTS_TEST_BYTE_BUFFER_H */