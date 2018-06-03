#ifndef BINIO_TESTS_TEST_PACKET_H
#define BINIO_TESTS_TEST_PACKET_H

#include <assert.h>
#include <memory>
#include <iostream>
#include <cstdio>

#include "../packet/PacketReader.h"
#include "../packet/PacketWriter.h"

namespace binio {

class TestPacket {
	using Reader = PacketReader<unsigned>;
	using Writer = PacketWriter<unsigned>;

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
		char raw_buffer[raw_buffer_size];

		Reader buf(as_mem_const_area(raw_buffer, raw_buffer_size));

		assert(buf.available() == raw_buffer_size);
		for (unsigned i = 0; i < raw_buffer_size; i++) {
			assert(buf.offset() == i);
			assert(buf.available() == raw_buffer_size - i);
			assert(buf.available(raw_buffer_size - i));
			assert(buf.padding() == 0);
			assert(buf.size() == raw_buffer_size);
			buf.head_move(1);
		}

		for (unsigned i = 0; i < raw_buffer_size; i++) {
			assert(buf.offset() == raw_buffer_size - i);
			assert(buf.available() == i);
			assert(buf.available(i));
			assert(buf.padding() == 0);
			assert(buf.size() == raw_buffer_size);
			buf.head_move_back(1);
		}
	}

	static void test_distances_tail() noexcept {
		unsigned raw_buffer_size = 8;
		char raw_buffer[raw_buffer_size];

		Reader buf(as_mem_const_area(raw_buffer, raw_buffer_size));

		assert(buf.available() == raw_buffer_size);
		for (unsigned i = 0; i < raw_buffer_size; i++) {
			assert(buf.offset() == 0);
			assert(buf.available() == raw_buffer_size - i);
			assert(buf.available(raw_buffer_size - i));
			assert(buf.padding() == i);
			assert(buf.size() == raw_buffer_size);
			buf.tail_move_back(1);
		}

		for (unsigned i = 0; i < raw_buffer_size; i++) {
			assert(buf.offset() == 0);
			assert(buf.available() == i);
			assert(buf.available(i));
			assert(buf.padding() == raw_buffer_size - i);
			assert(buf.size() == raw_buffer_size);
			buf.tail_move(1);
		}

	}

	static void test_distances_read() noexcept {
		using DataType = unsigned long long;
		unsigned raw_buffer_size = 32;
		DataType raw_buffer[raw_buffer_size];
		Reader buf(as_mem_const_area(raw_buffer, raw_buffer_size));

		DataType value0;
		DataType value1;

		assert(buf.available() == raw_buffer_size * sizeof (DataType));

		for (unsigned i = 0; i < raw_buffer_size; i++) {
			assert(buf.offset() == i * sizeof (DataType));
			assert(buf.available() == (raw_buffer_size - i) * sizeof (DataType));
			assert(buf.available((raw_buffer_size - i) * sizeof (DataType)));
			assert(buf.padding() == 0);
			assert(buf.size() == raw_buffer_size * sizeof (DataType));
			buf.read(value0);
		}
		buf.reset();

		for (unsigned i = 0; i < raw_buffer_size; i += 2) {
			assert(buf.offset() == i * sizeof (DataType));
			assert(buf.available() == (raw_buffer_size - i) * sizeof (DataType));
			assert(buf.available((raw_buffer_size - i) * sizeof (DataType)));
			assert(buf.padding() == 0);
			assert(buf.size() == raw_buffer_size * sizeof (DataType));
			buf.read(value0, value1);
		}

	}

	static void test_distances_write() noexcept {
		using DataType = unsigned long long;
		unsigned raw_buffer_size = 32;
		DataType raw_buffer[raw_buffer_size];

		Writer buf(as_mem_area(raw_buffer, raw_buffer_size));

		DataType value0;
		DataType value1;

		assert(buf.available() == raw_buffer_size * sizeof (DataType));

		for (unsigned i = 0; i < raw_buffer_size; i++) {
			assert(buf.offset() == i * sizeof (DataType));
			assert(buf.available() == (raw_buffer_size - i) * sizeof (DataType));
			assert(buf.available((raw_buffer_size - i) * sizeof (DataType)));
			assert(buf.padding() == 0);
			assert(buf.size() == raw_buffer_size * sizeof (DataType));
			buf.write(value0);
		}

		buf.reset();

		for (unsigned i = 0; i < raw_buffer_size; i += 2) {
			assert(buf.offset() == i * sizeof (DataType));
			assert(buf.available() == (raw_buffer_size - i) * sizeof (DataType));
			assert(buf.available((raw_buffer_size - i) * sizeof (DataType)));
			assert(buf.padding() == 0);
			assert(buf.size() == raw_buffer_size * sizeof (DataType));
			buf.write(value0, value1);
		}

	}

	static void test_distances_as_mem_area() noexcept {
		unsigned raw_buffer_size = 8;
		char raw_buffer[raw_buffer_size];
		Reader buf(as_mem_const_area(raw_buffer, raw_buffer_size));

		buf.reset();

		for (unsigned i = 0; i < raw_buffer_size; i++) {
			assert(buf.offset() == 0);
			assert(buf.available() == raw_buffer_size - i);
			assert(buf.available(raw_buffer_size - i));
			assert(buf.padding() == 0);
			assert(buf.size() == raw_buffer_size - i);
			buf.head_move(1);
			buf = Reader(buf.available_mem_area());
		}

		buf = Reader(as_mem_const_area(raw_buffer, raw_buffer_size));
		for (unsigned i = 0; i < raw_buffer_size; i++) {
			assert(buf.offset() == 0);
			assert(buf.available() == raw_buffer_size - i);
			assert(buf.available(raw_buffer_size - i));
			assert(buf.padding() == 0);
			assert(buf.size() == raw_buffer_size - i);
			buf.tail_move_back(1);
			buf = Reader(buf.available_mem_area());
		}

	}

	static void test_distances_reset() noexcept {
		unsigned raw_buffer_size = 8;
		char raw_buffer[raw_buffer_size];
		Reader buf(as_mem_const_area(raw_buffer, raw_buffer_size));

		assert(buf.offset() == 0);
		assert(buf.available() == raw_buffer_size);
		assert(buf.padding() == 0);

		buf.head_move(1);
		buf.tail_move_back(1);

		assert(buf.offset() == 1);
		assert(buf.available() == raw_buffer_size - 2);
		assert(buf.padding() == 1);

		buf.reset();

		assert(buf.offset() == 0);
		assert(buf.available() == raw_buffer_size);
		assert(buf.padding() == 0);
	}

	static void test_read_write(DataSet& set) noexcept {
		DataSet copy;
		unsigned raw_buffer_size = 2;
		DataSet raw_buffer[raw_buffer_size];

		Writer buf(as_mem_area(raw_buffer, raw_buffer_size));
		assert(buf.available() == sizeof (DataSet) * raw_buffer_size);

		// writing
		buf.write(set);
		buf.write(set.c, set.s, set.i, set.l);
		buf.reset();

		// reading
		buf.read(copy);
		assert(copy == set);
		buf.read(copy.c, copy.s, copy.i, copy.l);
		assert(copy == set);
	}

	static void test_assign(DataSet& set) noexcept {
		DataSet* copy;
		const char* c_cptr = nullptr;
		const short* s_cptr = nullptr;
		int* i_ptr = nullptr;
		long* l_ptr = nullptr;

		unsigned raw_buffer_size = sizeof (DataSet) * 2;
		char raw_buffer[raw_buffer_size];
		Writer buf(as_mem_area(raw_buffer, raw_buffer_size));

		// writing
		buf.write(set);
		buf.write(set.c, set.s, set.i, set.l);
		buf.reset();

		// reading
		buf.assign(copy);
		assert(*copy == set);

		buf.assign(c_cptr);
		buf.assign(s_cptr);
		buf.assign(i_ptr);
		buf.assign(l_ptr);

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

		Writer buf(as_mem_area(raw_buffer, buf_size));
		assert(buf.size() == sizeof (RawType) * buf_size);

		for (int i = 0; i < buf_size; i += 4) {
			buf.write_memory(raw_input + i, 4);
		}

		buf.reset();
		for (int i = 0; i < buf_size; i += 4) {
			buf.read_memory(raw_output + i, 4);
		}

		for (int i = 0; i < buf_size; i++) {
			assert(raw_input[i] == raw_output[i]);
		}
	}

public:

	static void test() {
		test_distances_head();
		test_distances_tail();
		test_distances_read();
		test_distances_write();
		test_distances_as_mem_area();
		test_distances_reset();
		test_read_write_assign();
		test_read_write_memory();
	}

};

}; // namespace binio

#endif /* BINIO_TESTS_TEST_PACKET_H */