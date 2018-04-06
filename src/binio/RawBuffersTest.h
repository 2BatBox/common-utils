#ifndef BINIO_RAW_BUFFER_TEST_H
#define BINIO_RAW_BUFFER_TEST_H

#include <assert.h>

#include "RawBuffer.h"

namespace binio {

class RawBuffersTest {

	struct DataSet {
		char c;
		short s;
		int i;
		long l;

		DataSet(): c(0), s(0), i(0), l(0) { }

		DataSet(char c, short s, int i, long l): c(c), s(s), i(i), l(l) { }

		bool operator==(const DataSet& ds) {
			return c == ds.c && s == ds.s && i == ds.i && l == ds.l;
		}
	};

	static void test_struct_rw(DataSet& set) {
		DataSet copy;

		char raw_buffer[sizeof (DataSet)];
		RawBuffer buffer = make_buffer(raw_buffer, sizeof (DataSet));

		assert(buffer.bounds());
		assert(buffer.write(set));
		assert(buffer.bounds());
		buffer.reset();
		assert(buffer.bounds());
		assert(buffer.read(copy));
		assert(buffer.bounds());
		assert(copy == set);
	}

	static void test_struct_assign(DataSet& set) {
		DataSet* set_ptr;

		char raw_buffer[sizeof (DataSet)];
		RawBuffer buffer = make_buffer(raw_buffer, sizeof (DataSet));

		assert(buffer.bounds());
		assert(buffer.write(set));
		assert(buffer.bounds());
		buffer.reset();
		assert(buffer.bounds());
		assert(buffer.assign(set_ptr));
		assert(buffer.bounds());
		assert(set == *set_ptr);
	}

	static void test_fields_rw(DataSet& set) {
		DataSet copy;

		char raw_buffer[sizeof (DataSet)];
		RawBuffer buffer = make_buffer(raw_buffer, sizeof (DataSet));

		assert(buffer.bounds());
		assert(buffer.write(set.c, set.s, set.i, set.l));
		assert(buffer.bounds());
		buffer.reset();
		assert(buffer.bounds());
		assert(buffer.read(copy.c, copy.s, copy.i, copy.l));
		assert(buffer.bounds());
		assert(copy == set);
	}

	static void test_fields_assign(DataSet& set) {
		DataSet copy;
		const char* c_cptr = nullptr;
		const short* s_cptr = nullptr;
		int* i_ptr = nullptr;
		long* l_ptr = nullptr;

		char raw_buffer[sizeof (DataSet)];
		RawBuffer buffer = make_buffer(raw_buffer, sizeof (DataSet));

		assert(buffer.bounds());
		assert(buffer.write(set.c, set.s, set.i, set.l));
		assert(buffer.bounds());
		buffer.reset();
		assert(buffer.bounds());
		assert(buffer.assign(c_cptr, s_cptr, i_ptr, l_ptr));

		copy.c = *c_cptr;
		copy.s = *s_cptr;
		copy.i = *i_ptr;
		copy.l = *l_ptr;

		assert(buffer.bounds());
		assert(copy == set);
	}

	static void test_smash() {
		char raw_buffer[2];
		char w1 = 1;
		char w2 = 128;
		char w3 = 255;

		char r1 = 0;
		char r2 = 0;
		char r3 = 0;

		RawBuffer buffer = make_buffer(raw_buffer, 2);

		assert(buffer.bounds());
		assert(buffer.write(w1));
		assert(buffer.write(w2));
		assert(buffer.bounds());
		assert(not buffer.write(w3));
		assert(not buffer.bounds());

		buffer.reset();
		assert(buffer.bounds());
		assert(buffer.write(w1, w2));
		assert(buffer.bounds());
		assert(not buffer.write(w3));
		assert(not buffer.bounds());

		buffer.reset();
		assert(buffer.read(r1));
		assert(buffer.read(r2));
		assert(buffer.bounds());
		assert(not buffer.read(r3));
		assert(not buffer.bounds());
		assert(w1 == r1);
		assert(w2 == r2);

		buffer.reset();
		assert(buffer.read(r1, r2));
		assert(buffer.bounds());
		assert(not buffer.read(r3));
		assert(not buffer.bounds());
		assert(w1 == r1);
		assert(w2 == r2);
	}

	static void test_skip() {
		unsigned char raw_buffer[2];
		char w1 = 1;
		char w2 = 128;
		char w3 = 255;

		char r1 = 0;
		char r2 = 0;
		char r3 = 0;

		RawBuffer buffer = make_buffer(raw_buffer, 2);
		assert(buffer.write(w1));
		assert(buffer.skip(1));
		assert(buffer.bounds());
		assert(not buffer.write(w3));
		assert(not buffer.bounds());

		buffer.reset();
		assert(buffer.skip(1));
		assert(buffer.write(w2));
		assert(buffer.bounds());
		assert(not buffer.write(w3));
		assert(not buffer.bounds());

		buffer.reset();
		assert(buffer.read(r1));
		assert(buffer.skip(1));
		assert(buffer.bounds());
		assert(not buffer.read(r3));
		assert(not buffer.bounds());

		buffer.reset();
		assert(buffer.skip(1));
		assert(buffer.read(r2));
		assert(buffer.bounds());
		assert(not buffer.read(r3));
		assert(not buffer.bounds());

		assert(w1 == r1);
		assert(w2 == r2);
	}

	static void test_raii_copy_ctr() {
		DataSet set_first(~0 - 1, ~0 - 2, ~0 - 3, ~0 - 4);
		DataSet set_second(1, 2, 3, 4);

		DataSet copy;

		char raw_buffer[sizeof (DataSet) * 2];
		RawBuffer buffer = make_buffer(raw_buffer, sizeof (DataSet) * 2);

		assert(buffer.bounds());
		assert(buffer.write(set_first));
		assert(buffer.bounds());

		RawBuffer buffer2(buffer);
		assert(buffer.bounds());
		assert(buffer.write(set_second));
		assert(buffer.bounds());

		ConstRawBuffer const_buffer(buffer2);
		const_buffer.reset();
		assert(const_buffer.bounds());
		assert(const_buffer.read(copy));
		assert(const_buffer.bounds());
		assert(copy == set_first);

		ConstRawBuffer const_buffer2(const_buffer);
		assert(const_buffer2.bounds());
		assert(const_buffer2.read(copy));
		assert(const_buffer2.bounds());
		assert(copy == set_second);
	}

	static void test_raii_copy_opt() {
		DataSet set_first(~0 - 1, ~0 - 2, ~0 - 3, ~0 - 4);
		DataSet set_second(1, 2, 3, 4);

		DataSet copy;

		char raw_buffer[sizeof (DataSet) * 2];
		RawBuffer buffer = make_buffer(raw_buffer, sizeof (DataSet) * 2);

		assert(buffer.bounds());
		assert(buffer.write(set_first));
		assert(buffer.bounds());

		RawBuffer buffer2;
		buffer2 = buffer;
		assert(buffer.bounds());
		assert(buffer.write(set_second));
		assert(buffer.bounds());

		ConstRawBuffer const_buffer;
		const_buffer = buffer2;

		const_buffer.reset();
		assert(const_buffer.bounds());
		assert(const_buffer.read(copy));
		assert(const_buffer.bounds());
		assert(copy == set_first);

		ConstRawBuffer const_buffer2;
		const_buffer2 = const_buffer;

		assert(const_buffer2.bounds());
		assert(const_buffer2.read(copy));
		assert(const_buffer2.bounds());
		assert(copy == set_second);
	}

	static void test_structs() {
		DataSet input_false(0x00, 0x00, 0x00, 0x00);
		DataSet input_true(~0, ~0, ~0, ~0);
		DataSet input_inc(0x01, 0x02, 0x03, 0x04);
		DataSet input_dec(~0 - 1, ~0 - 2, ~0 - 3, ~0 - 4);

		test_struct_rw(input_false);
		test_struct_rw(input_true);
		test_struct_rw(input_inc);
		test_struct_rw(input_dec);

		test_struct_assign(input_false);
		test_struct_assign(input_true);
		test_struct_assign(input_inc);
		test_struct_assign(input_dec);

		test_fields_rw(input_false);
		test_fields_rw(input_true);
		test_fields_rw(input_inc);
		test_fields_rw(input_dec);

		test_fields_assign(input_false);
		test_fields_assign(input_true);
		test_fields_assign(input_inc);
		test_fields_assign(input_dec);
	}

public:

	static void test() {
		test_structs();
		test_smash();
		test_skip();
		test_raii_copy_ctr();
		test_raii_copy_opt();
	}

};

}; // namespace binio

#endif /* BINIO_RAW_BUFFER_TEST_H */