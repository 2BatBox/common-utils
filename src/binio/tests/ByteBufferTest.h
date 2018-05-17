#ifndef BINIO_SAFE_AREA_TEST_H
#define BINIO_SAFE_AREA_TEST_H

#include <assert.h>
#include <memory>
#include <iostream>
#include <cstdio>

#include "../ByteBuffer.h"

namespace binio {

class ByteBufferTest {

	struct DataSet {
		char c;
		short s;
		int i;
		long l;

		DataSet() noexcept : c(0), s(0), i(0), l(0) { }

		DataSet(char c, short s, int i, long l) noexcept : c(c), s(s), i(i), l(l) { }

		bool operator==(const DataSet& ds) noexcept {
			return c == ds.c && s == ds.s && i == ds.i && l == ds.l;
		}
	};

	static void test_distances_head() noexcept {
		unsigned raw_buffer_size = 8;
		unsigned char raw_buffer[raw_buffer_size];
		ReadableByteBuffer buf(raw_buffer, raw_buffer_size);

		assert(buf.reset());
		assert(buf.bounds());
		for (unsigned i = 0; i < raw_buffer_size; i++) {
			assert(buf.offset() == i);
			assert(buf.available() == raw_buffer_size - i);
			assert(buf.available(raw_buffer_size - i));
			assert(buf.padding() == 0);
			assert(buf.size() == raw_buffer_size);
			assert(buf.head_move(1));
		}

		for (unsigned i = 0; i < raw_buffer_size; i++) {
			assert(buf.offset() == raw_buffer_size - i);
			assert(buf.available() == i);
			assert(buf.available(i));
			assert(buf.padding() == 0);
			assert(buf.size() == raw_buffer_size);
			assert(buf.head_move_back(1));
		}
		assert(buf.bounds());
	}

	static void test_distances_tail() noexcept {
		unsigned raw_buffer_size = 8;
		unsigned char raw_buffer[raw_buffer_size];
		ReadableByteBuffer buf(raw_buffer, raw_buffer_size);

		assert(buf.reset());
		assert(buf.bounds());
		for (unsigned i = 0; i < raw_buffer_size; i++) {
			assert(buf.offset() == 0);
			assert(buf.available() == raw_buffer_size - i);
			assert(buf.available(raw_buffer_size - i));
			assert(buf.padding() == i);
			assert(buf.size() == raw_buffer_size);
			assert(buf.tail_move_back(1));
		}

		for (unsigned i = 0; i < raw_buffer_size; i++) {
			assert(buf.offset() == 0);
			assert(buf.available() == i);
			assert(buf.available(i));
			assert(buf.padding() == raw_buffer_size - i);
			assert(buf.size() == raw_buffer_size);
			assert(buf.tail_move(1));
		}
		assert(buf.bounds());
	}

	static void test_distances_read() noexcept {
		using DataType = unsigned long long;
		unsigned raw_buffer_size = 32;
		DataType raw_buffer[raw_buffer_size];
		ReadableByteBuffer buf(raw_buffer, raw_buffer_size * sizeof (DataType));

		DataType value0;
		DataType value1;

		assert(buf.reset());
		assert(buf.bounds());
		for (unsigned i = 0; i < raw_buffer_size; i++) {
			assert(buf.offset() == i * sizeof (DataType));
			assert(buf.available() == (raw_buffer_size - i) * sizeof (DataType));
			assert(buf.available((raw_buffer_size - i) * sizeof (DataType)));
			assert(buf.padding() == 0);
			assert(buf.size() == raw_buffer_size * sizeof (DataType));
			assert(buf.read(value0));
		}

		assert(buf.reset());
		assert(buf.bounds());
		for (unsigned i = 0; i < raw_buffer_size; i += 2) {
			assert(buf.offset() == i * sizeof (DataType));
			assert(buf.available() == (raw_buffer_size - i) * sizeof (DataType));
			assert(buf.available((raw_buffer_size - i) * sizeof (DataType)));
			assert(buf.padding() == 0);
			assert(buf.size() == raw_buffer_size * sizeof (DataType));
			assert(buf.read(value0, value1));
		}
		assert(buf.bounds());
	}

	static void test_distances_write() noexcept {
		using DataType = unsigned long long;
		unsigned raw_buffer_size = 32;
		DataType raw_buffer[raw_buffer_size];
		WritableByteBuffer buf(raw_buffer, raw_buffer_size * sizeof (DataType));

		DataType value0;
		DataType value1;

		assert(buf.reset());
		assert(buf.bounds());
		for (unsigned i = 0; i < raw_buffer_size; i++) {
			assert(buf.offset() == i * sizeof (DataType));
			assert(buf.available() == (raw_buffer_size - i) * sizeof (DataType));
			assert(buf.available((raw_buffer_size - i) * sizeof (DataType)));
			assert(buf.padding() == 0);
			assert(buf.size() == raw_buffer_size * sizeof (DataType));
			assert(buf.write(value0));
		}

		assert(buf.reset());
		assert(buf.bounds());
		for (unsigned i = 0; i < raw_buffer_size; i += 2) {
			assert(buf.offset() == i * sizeof (DataType));
			assert(buf.available() == (raw_buffer_size - i) * sizeof (DataType));
			assert(buf.available((raw_buffer_size - i) * sizeof (DataType)));
			assert(buf.padding() == 0);
			assert(buf.size() == raw_buffer_size * sizeof (DataType));
			assert(buf.write(value0, value1));
		}
		assert(buf.bounds());
	}

	static void test_distances_range() noexcept {
		unsigned raw_buffer_size = 8;
		unsigned char raw_buffer[raw_buffer_size];
		ReadableByteBuffer buf(raw_buffer, raw_buffer_size);

		assert(buf.reset());
		assert(buf.bounds());
		for (unsigned i = 0; i < raw_buffer_size; i++) {
			assert(buf.offset() == 0);
			assert(buf.available() == raw_buffer_size - i);
			assert(buf.available(raw_buffer_size - i));
			assert(buf.padding() == 0);
			assert(buf.size() == raw_buffer_size - i);
			assert(buf.head_move(1));
			buf = ReadableByteBuffer(buf.available_array());
		}

		buf = ReadableByteBuffer(raw_buffer, raw_buffer_size);
		for (unsigned i = 0; i < raw_buffer_size; i++) {
			assert(buf.offset() == 0);
			assert(buf.available() == raw_buffer_size - i);
			assert(buf.available(raw_buffer_size - i));
			assert(buf.padding() == 0);
			assert(buf.size() == raw_buffer_size - i);
			assert(buf.tail_move_back(1));
			buf = ReadableByteBuffer(buf.available_array());
		}
		assert(buf.bounds());
	}

	static void test_distances_reset() noexcept {
		unsigned raw_buffer_size = 8;
		unsigned char raw_buffer[raw_buffer_size];
		ReadableByteBuffer buf(raw_buffer, raw_buffer_size);

		assert(buf.offset() == 0);
		assert(buf.available() == raw_buffer_size);
		assert(buf.padding() == 0);

		assert(buf.head_move(1));
		assert(buf.tail_move_back(1));

		assert(buf.offset() == 1);
		assert(buf.available() == raw_buffer_size - 2);
		assert(buf.padding() == 1);

		assert(buf.reset());

		assert(buf.offset() == 0);
		assert(buf.available() == raw_buffer_size);
		assert(buf.padding() == 0);
	}

	static void test_read_write(DataSet& set) noexcept {
		DataSet copy;

		unsigned raw_buffer_size = sizeof (DataSet) * 2;
		char raw_buffer[raw_buffer_size];
		WritableByteBuffer buffer(raw_buffer, raw_buffer_size);

		// writing
		assert(buffer.bounds());
		assert(buffer.write(set));
		assert(buffer.write(set.c, set.s, set.i, set.l));
		assert(buffer.bounds());
		assert(buffer.reset());

		// reading
		assert(buffer.bounds());
		assert(buffer.read(copy));
		assert(copy == set);

		assert(buffer.read(copy.c, copy.s, copy.i, copy.l));
		assert(copy == set);
		assert(buffer.bounds());
	}

	static void test_assign(DataSet& set) noexcept {
		DataSet* copy;
		const char* c_cptr = nullptr;
		const short* s_cptr = nullptr;
		int* i_ptr = nullptr;
		long* l_ptr = nullptr;

		unsigned raw_buffer_size = sizeof (DataSet) * 2;
		char raw_buffer[raw_buffer_size];
		WritableByteBuffer buffer(raw_buffer, raw_buffer_size);

		// writing
		assert(buffer.bounds());
		assert(buffer.write(set));
		assert(buffer.write(set.c, set.s, set.i, set.l));
		assert(buffer.bounds());
		assert(buffer.reset());

		// reading
		assert(buffer.bounds());
		assert(buffer.assign(copy));
		assert(*copy == set);

		assert(buffer.assign(c_cptr));
		assert(buffer.assign(s_cptr));
		assert(buffer.assign(i_ptr));
		assert(buffer.assign(l_ptr));
		assert(buffer.bounds());
		assert(*c_cptr == set.c);
		assert(*s_cptr == set.s);
		assert(*i_ptr == set.i);
		assert(*l_ptr == set.l);
	}

	static void test_read_write_assign() noexcept {
		DataSet input_false(0x00, 0x00, 0x00, 0x00);
		DataSet input_true(~0, ~0, ~0, ~0);
		DataSet input_inc(0x01, 0x02, 0x03, 0x04);
		DataSet input_dec(~0 - 1, ~0 - 2, ~0 - 3, ~0 - 4);

		test_read_write(input_false);
		test_read_write(input_true);
		test_read_write(input_inc);
		test_read_write(input_dec);

		test_assign(input_false);
		test_assign(input_true);
		test_assign(input_inc);
		test_assign(input_dec);
	}

	static void test_read_write_memory() noexcept {
		using RawType = size_t;
		constexpr int buf_size = 32;
		RawType raw_buffer[buf_size];
		int raw_input[buf_size];
		int raw_output[buf_size];
		for (int i = 0; i < buf_size; i++) {
			raw_input[i] = i;
		}

		WritableByteBuffer buffer(raw_buffer, buf_size * sizeof (RawType));
		assert(buffer.size() == sizeof (RawType) * buf_size);
		assert(buffer.bounds());
		for (int i = 0; i < buf_size; i += 4) {
			assert(buffer.write_memory(raw_input + i, 4));
		}
		assert(buffer.bounds());
		assert(buffer.reset());
		for (int i = 0; i < buf_size; i += 4) {
			assert(buffer.read_memory(raw_output + i, 4));
		}
		assert(buffer.bounds());
		for (int i = 0; i < buf_size; i++) {
			assert(raw_input[i] == raw_output[i]);
		}
	}

	static void test_raii() noexcept {
		DataSet set_first(~0 - 1, ~0 - 2, ~0 - 3, ~0 - 4);
		DataSet set_second(1, 2, 3, 4);

		DataSet copy;

		unsigned raw_buffer_size = sizeof (DataSet) * 2;
		unsigned char raw_buffer[raw_buffer_size];

		WritableByteBuffer buffer(raw_buffer, raw_buffer_size);
		assert(buffer.write(set_first));
		assert(buffer.bounds());

		WritableByteBuffer buffer2(buffer);
		assert(buffer.write(set_second));
		assert(buffer.bounds());

		ReadableByteBuffer const_buffer(buffer2);
		assert(const_buffer.reset());
		assert(const_buffer.bounds());
		assert(const_buffer.read(copy));
		assert(copy == set_first);
		assert(const_buffer.read(copy));
		assert(copy == set_second);
		assert(const_buffer.bounds());

		copy = DataSet();
		ReadableByteBuffer const_buffer2;
		const_buffer2 = buffer2;

		assert(const_buffer2.reset());
		assert(const_buffer2.bounds());
		assert(const_buffer2.read(copy));
		assert(copy == set_first);
		assert(const_buffer2.read(copy));
		assert(copy == set_second);
		assert(const_buffer2.bounds());
	}

	static void test_input_data() noexcept {
		int i;
		int* iptr = &i;
		int* iptr_null = nullptr;

		ReadableByteBuffer ra;
		assert(not ra.bounds());

		ra = ReadableByteBuffer(iptr_null, 1);
		assert(not ra.bounds());

		ra = ReadableByteBuffer(make_const_byte_array(iptr_null, 1));
		assert(not ra.bounds());

		ra = ReadableByteBuffer(iptr, 1);
		assert(ra.bounds());

		ra = ReadableByteBuffer(make_const_byte_array(iptr, 1));
		assert(ra.bounds());

		WritableByteBuffer wa;
		assert(not wa.bounds());

		wa = WritableByteBuffer(iptr_null, 1);
		assert(not wa.bounds());

		wa = WritableByteBuffer(make_byte_array(iptr_null, 1));
		assert(not wa.bounds());

		wa = WritableByteBuffer(iptr, 1);
		assert(wa.bounds());

		wa = WritableByteBuffer(make_byte_array(iptr, 1));
		assert(wa.bounds());
	}

public:

	static void test() {
		test_distances_head();
		test_distances_tail();
		test_distances_read();
		test_distances_write();
		test_distances_range();
		test_distances_reset();
		test_read_write_assign();
		test_read_write_memory();
		test_raii();
		test_input_data();
	}

};

}; // namespace binio

#endif /* BINIO_SAFE_AREA_TEST_H */