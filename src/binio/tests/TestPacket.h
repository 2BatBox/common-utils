#ifndef BINIO_TESTS_TEST_PACKET_H
#define BINIO_TESTS_TEST_PACKET_H

#include <assert.h>
#include <memory>
#include <iostream>
#include <cstdio>

#include "../packet/PacketReader.h"
#include "../packet/PacketWriter.h"
#include "../packet/PacketSafeReader.h"
#include "../packet/PacketSafeWriter.h"

namespace binio {

class TestPacket {
	using Reader = PacketReader;
	using Writer = PacketWriter;

	static constexpr unsigned BUFFER_SIZE = 64;

	struct DataSet {
		char c;
		short s;
		int i;
		long l;

		DataSet() noexcept : c(0), s(0), i(0), l(0) {}

		DataSet(char c, short s, int i, long l) noexcept : c(c), s(s), i(i), l(l) {}

		bool operator==(const DataSet& ds) const noexcept {
			return c == ds.c && s == ds.s && i == ds.i && l == ds.l;
		}
	};

	struct DataSetPacket {
		char c;
		short s;
		int i;
		long l;

		DataSetPacket() noexcept : c(0), s(0), i(0), l(0) {}

		DataSetPacket(char c, short s, int i, long l) noexcept : c(c), s(s), i(i), l(l) {}

		bool operator==(const DataSetPacket& ds) noexcept {
			return c == ds.c && s == ds.s && i == ds.i && l == ds.l;
		}
	} __attribute__ ((packed));

	template<typename T, typename R>
	static void test_distances_head() noexcept {
		printf("-> test_distances_head()\n");

		T raw_buffer[BUFFER_SIZE];
		R buf(as_const_area(raw_buffer, BUFFER_SIZE));
		unsigned bn_bytes = BUFFER_SIZE * sizeof(T);

		assert(buf.available() == bn_bytes);
		for(unsigned i = 0; i < bn_bytes; i++) {
			assert(buf.offset() == i);
			assert(buf.available() == bn_bytes - i);
			assert(buf.available(bn_bytes - i));
			assert(buf.padding() == 0);
			assert(buf.size() == bn_bytes);
			buf.head_move(1);
		}

		for(unsigned i = 0; i < bn_bytes; i++) {
			assert(buf.offset() == bn_bytes - i);
			assert(buf.available() == i);
			assert(buf.available(i));
			assert(buf.padding() == 0);
			assert(buf.size() == bn_bytes);
			buf.head_move_back(1);
		}
	}

	template<typename T, typename R>
	static void test_distances_tail() noexcept {
		printf("-> test_distances_tail()\n");

		T raw_buffer[BUFFER_SIZE];
		R buf(as_const_area(raw_buffer, BUFFER_SIZE));
		unsigned bn_bytes = BUFFER_SIZE * sizeof(T);

		assert(buf.available() == bn_bytes);
		for(unsigned i = 0; i < bn_bytes; i++) {
			assert(buf.offset() == 0);
			assert(buf.available() == bn_bytes - i);
			assert(buf.available(bn_bytes - i));
			assert(buf.padding() == i);
			assert(buf.size() == bn_bytes);
			buf.tail_move_back(1);
		}

		for(unsigned i = 0; i < bn_bytes; i++) {
			assert(buf.offset() == 0);
			assert(buf.available() == i);
			assert(buf.available(i));
			assert(buf.padding() == bn_bytes - i);
			assert(buf.size() == bn_bytes);
			buf.tail_move(1);
		}

	}

	template<typename T, typename R>
	static void test_distances_read() noexcept {
		printf("-> test_distances_read()\n");

		T raw_buffer[BUFFER_SIZE];
		R buf(as_const_area(raw_buffer, BUFFER_SIZE));
		unsigned bn_bytes = BUFFER_SIZE * sizeof(T);

		T value0;
		T value1;

		assert(buf.available() == bn_bytes);

		for(unsigned i = 0; i < BUFFER_SIZE; i++) {
			assert(buf.offset() == i * sizeof(T));
			assert(buf.available() == bn_bytes - (i * sizeof(T)));
			assert(buf.available(bn_bytes - (i * sizeof(T))));
			assert(buf.padding() == 0);
			assert(buf.size() == bn_bytes);
			buf.read(value0);
		}
		buf.reset();

		for(unsigned i = 0; i < BUFFER_SIZE; i += 2) {
			assert(buf.offset() == i * sizeof(T));
			assert(buf.available() == (bn_bytes - (i * sizeof(T))));
			assert(buf.available(bn_bytes - (i * sizeof(T))));
			assert(buf.padding() == 0);
			assert(buf.size() == bn_bytes);
			buf.read(value0, value1);
		}
	}

	template<typename T, typename W>
	static void test_distances_write() noexcept {
		printf("-> test_distances_write()\n");

		T raw_buffer[BUFFER_SIZE];
		W buf(as_area(raw_buffer, BUFFER_SIZE));
		unsigned bn_bytes = BUFFER_SIZE * sizeof(T);

		T value0;
		T value1;

		assert(buf.available() == bn_bytes);

		for(unsigned i = 0; i < BUFFER_SIZE; i++) {
			assert(buf.offset() == i * sizeof(T));
			assert(buf.available() == bn_bytes - (i * sizeof(T)));
			assert(buf.available(bn_bytes - (i * sizeof(T))));
			assert(buf.padding() == 0);
			assert(buf.size() == bn_bytes);
			buf.write(value0);
		}
		buf.reset();

		for(unsigned i = 0; i < BUFFER_SIZE; i += 2) {
			assert(buf.offset() == i * sizeof(T));
			assert(buf.available() == (bn_bytes - (i * sizeof(T))));
			assert(buf.available(bn_bytes - (i * sizeof(T))));
			assert(buf.padding() == 0);
			assert(buf.size() == bn_bytes);
			buf.write(value0, value1);
		}
	}

	template<typename T, typename W>
	static void test_distances_as_area() noexcept {
		printf("-> test_distances_as_area()\n");

		T raw_buffer[BUFFER_SIZE];
		W buf(as_area(raw_buffer, BUFFER_SIZE));
		unsigned bn_bytes = BUFFER_SIZE * sizeof(T);

		buf.reset();

		for(unsigned i = 0; i < bn_bytes; i++) {
			assert(buf.offset() == 0);
			assert(buf.available() == bn_bytes - i);
			assert(buf.available(bn_bytes - i));
			assert(buf.padding() == 0);
			assert(buf.size() == bn_bytes - i);
			buf.head_move(1);
			buf = W(buf.available_area());
		}

		buf = W(as_area(raw_buffer, BUFFER_SIZE));
		for(unsigned i = 0; i < bn_bytes; i++) {
			assert(buf.offset() == 0);
			assert(buf.available() == bn_bytes - i);
			assert(buf.available(bn_bytes - i));
			assert(buf.padding() == 0);
			assert(buf.size() == bn_bytes - i);
			buf.tail_move_back(1);
			buf = W(buf.available_area());
		}
	}

	template<typename T, typename W>
	static void test_distances_reset() noexcept {
		printf("-> test_distances_reset()\n");

		T raw_buffer[BUFFER_SIZE];
		W buf(as_area(raw_buffer, BUFFER_SIZE));
		unsigned bn_bytes = BUFFER_SIZE * sizeof(T);

		assert(buf.offset() == 0);
		assert(buf.available() == bn_bytes);
		assert(buf.padding() == 0);

		buf.head_move(1);
		buf.tail_move_back(1);

		assert(buf.offset() == 1);
		assert(buf.available() == bn_bytes - 2);
		assert(buf.padding() == 1);

		buf.reset();

		assert(buf.offset() == 0);
		assert(buf.available() == bn_bytes);
		assert(buf.padding() == 0);
	}

	template<typename R, typename W>
	static void test_read_write_assign() noexcept {
		printf("-> test_read_write_assign()\n");
		DataSet input_false(0x00, 0x00, 0x00, 0x00);
		DataSet input_true(~0, ~0, ~0, ~0);
		DataSet input_inc(0x01, 0x02, 0x03, 0x04);
		DataSet input_dec(~0 - 1, ~0 - 2, ~0 - 3, ~0 - 4);

		test_read_write<R, W>(input_false);
		test_read_write<R, W>(input_true);
		test_read_write<R, W>(input_inc);
		test_read_write<R, W>(input_dec);

		test_assign<R, W>(input_false);
		test_assign<R, W>(input_true);
		test_assign<R, W>(input_inc);
		test_assign<R, W>(input_dec);
	}

	template<typename R, typename W>
	static void test_read_write_memory() noexcept {
		printf("-> test_read_write_memory()\n");

		//		size_t raw_buffer[BUFFER_SIZE];
		//		W buf(as_area(raw_buffer, BUFFER_SIZE));
		//		unsigned bn_bytes = BUFFER_SIZE * sizeof (size_t);
		//
		//		int raw_input[BUFFER_SIZE];
		//		int raw_output[BUFFER_SIZE];
		//		for (int i = 0; i < BUFFER_SIZE; i++) {
		//			raw_input[i] = i;
		//		}
		//
		//		Writer writer(as_area(raw_buffer, buf_size));
		//		assert(writer.size_addr() == sizeof (RawType) * buf_size);
		//
		//		for (int i = 0; i < buf_size; i += 4) {
		//			writer.write_area(as_const_area(raw_input + i, 4));
		//		}
		//
		//		writer.reset();
		//		for (int i = 0; i < buf_size; i += 4) {
		//			writer.read_area(as_area(raw_output + i, 4));
		//		}
		//
		//		for (int i = 0; i < buf_size; i++) {
		//			assert(raw_input[i] == raw_output[i]);
		//		}
	}

	template<typename R, typename W>
	static void test_read_write(DataSet& set) noexcept {
		DataSet copy;
		unsigned raw_buffer_size = 2;
		DataSet raw_buffer[raw_buffer_size];

		W writer(as_area(raw_buffer, raw_buffer_size));
		assert(writer.available() == sizeof(DataSet) * raw_buffer_size);

		// writing
		writer.write(set);
		writer.write(set.c, set.s, set.i, set.l);
		writer.reset();

		R reader(as_const_area(raw_buffer, raw_buffer_size));
		// reading
		reader.read(copy);
		assert(copy == set);
		reader.read(copy.c, copy.s, copy.i, copy.l);
		assert(copy == set);
	}

	template<typename R, typename W>
	static void test_assign(DataSet& set) noexcept {
		const DataSet* copy = nullptr;
		const char* c_cptr = nullptr;
		const short* s_cptr = nullptr;
		const int* i_ptr = nullptr;
		const long* l_ptr = nullptr;

		unsigned raw_buffer_size = sizeof(DataSet) * 2;
		char raw_buffer[raw_buffer_size];
		W writer(as_area(raw_buffer, raw_buffer_size));

		// writing
		writer.write(set);
		writer.write(set.c, set.s, set.i, set.l);
		writer.reset();

		// reading
		R reader(as_const_area(raw_buffer, raw_buffer_size));
		reader.assign(copy);
		assert(*copy == set);

		reader.assign(c_cptr);
		reader.assign(s_cptr);
		reader.assign(i_ptr);
		reader.assign(l_ptr);

		assert(*c_cptr == set.c);
		assert(*s_cptr == set.s);
		assert(*i_ptr == set.i);
		assert(*l_ptr == set.l);
	}

	template<typename T, typename R>
	static void test_reader() {
		printf("-> test_reader('%s', '%s')\n", typeid(T).name(), typeid(R).name());
		test_distances_head<T, R>();
		test_distances_tail<T, R>();
		test_distances_read<T, R>();
		test_distances_as_area<T, R>();
		test_distances_reset<T, R>();
	}

	template<typename T, typename W>
	static void test_writer() {
		printf("-> test_writer('%s', '%s')\n", typeid(T).name(), typeid(W).name());
		test_distances_write<T, W>();
		test_distances_as_area<T, W>();
		test_distances_reset<T, W>();
	}

	template<typename R, typename W>
	static void test_reader_writer() {
		printf("-> test_reader_writer('%s', '%s')\n", typeid(R).name(), typeid(W).name());
		test_read_write_assign<R, W>();
		test_read_write_memory<R, W>();
	}

	template<typename T>
	static void test_type() {
		test_reader<T, PacketReader>();
		test_reader<T, PacketSafeReader>();
		test_writer<T, PacketWriter>();
		test_writer<T, PacketSafeWriter>();
	}

	template<typename T>
	static void read_write_endian(Reader& reader, Writer& writer, T input) noexcept {
		printf("-> read_write_endian('%s')\n", typeid(T).name());
		T output;
		T bit_mask;

		writer.write_little_endian(input);
		reader.read_little_endian(output);
		assert(output == input);
		writer.write_big_endian(input);
		reader.read_big_endian(output);
		assert(output == input);

		for(unsigned i = 0; i < sizeof(T) + 1; ++i) {
			bit_mask = ~T(0);
			if(i < sizeof(T)){
				bit_mask <<= (i * 8);
				bit_mask = ~T(bit_mask);
			}
			writer.write_little_endian(input, i);
			reader.read_little_endian(output, i);
			if(i) {
				assert(output == (input & bit_mask));
			}
			writer.write_big_endian(input, i);
			reader.read_big_endian(output, i);
			if(i) {
				assert(output == (input & bit_mask));
			}
		}
	}

	static void test_endian() {
		printf("-> test_endian()\n");
		const unsigned buffer_size = 1024;
		char raw_buffer[buffer_size];
		for(unsigned i = 0; i < buffer_size; ++i) {
			raw_buffer[i] = i;
		}
		Reader reader(as_const_area(raw_buffer, buffer_size));
		Writer writer(as_area(raw_buffer, buffer_size));

		read_write_endian<uint8_t>(reader, writer, 0x01);
		read_write_endian<int8_t>(reader, writer, 0x01);
		read_write_endian<uint16_t>(reader, writer, 0x0102);
		read_write_endian<int16_t>(reader, writer, 0x0102);
		read_write_endian<uint32_t>(reader, writer, 0x01020304);
		read_write_endian<int32_t>(reader, writer, 0x01020304);
		read_write_endian<uint64_t>(reader, writer, 0x0102030405060708);
		read_write_endian<int64_t>(reader, writer, 0x0102030405060708);
	}


public:

	static void test() {
		printf("TestPacket is running...\n");
		test_type<bool>();
		test_type<unsigned char>();
		test_type<signed char>();
		test_type<unsigned long long>();
		test_type<signed long long>();
		test_type<float>();
		test_type<double>();
		test_type<DataSet>();
		test_type<DataSetPacket>();

		test_reader_writer<PacketReader, PacketWriter>();
		test_reader_writer<PacketSafeReader, PacketWriter>();
		test_reader_writer<PacketReader, PacketSafeWriter>();
		test_reader_writer<PacketSafeReader, PacketSafeWriter>();

		test_endian();

		printf("*** done\n");
	}

};

}; // namespace binio

#endif /* BINIO_TESTS_TEST_PACKET_H */