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

	static void test_read_write(DataSet& set) {
		DataSet copy;

		unsigned raw_buffer_size = sizeof (DataSet) * 2;
		char raw_buffer[raw_buffer_size];
		RawBuffer buffer(raw_buffer, raw_buffer_size);

		// writing
		assert(buffer.bounds());
		assert(buffer.write(set));
		assert(buffer.write(set.c, set.s, set.i, set.l));
		assert(buffer.bounds());
		assert(buffer.rewind());

		// reading
		assert(buffer.bounds());
		assert(buffer.read(copy));
		assert(copy == set);

		assert(buffer.read(copy.c, copy.s, copy.i, copy.l));
		assert(copy == set);
		assert(buffer.bounds());
	}

	static void test_assign(DataSet& set) {
		DataSet* copy;
		const char* c_cptr = nullptr;
		const short* s_cptr = nullptr;
		int* i_ptr = nullptr;
		long* l_ptr = nullptr;

		unsigned raw_buffer_size = sizeof (DataSet) * 2;
		char raw_buffer[raw_buffer_size];
		RawBuffer buffer(raw_buffer, raw_buffer_size);

		// writing
		assert(buffer.bounds());
		assert(buffer.write(set));
		assert(buffer.write(set.c, set.s, set.i, set.l));
		assert(buffer.bounds());
		assert(buffer.rewind());

		// reading
		assert(buffer.bounds());
		assert(buffer.assign(copy));
		assert(*copy == set);

		assert(buffer.assign(c_cptr, s_cptr, i_ptr, l_ptr));
		assert(buffer.bounds());
		assert(*c_cptr == set.c);
		assert(*s_cptr == set.s);
		assert(*i_ptr == set.i);
		assert(*l_ptr == set.l);
	}

	static void test_read_write_assign() {
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

	static void test_bounds() {
		char raw_buffer[2];
		char w1 = 1;
		char w2 = 128;
		char w3 = 255;

		char r1 = 0;
		char r2 = 0;
		char r3 = 0;

		RawBuffer buffer(raw_buffer, 2);

		assert(buffer.bounds());
		assert(buffer.write(w1));
		assert(buffer.write(w2));
		assert(buffer.bounds());
		assert(not buffer.write(w3));
		assert(not buffer.bounds());

		RawBuffer buffer2(raw_buffer, 2);
		assert(buffer2.bounds());
		assert(buffer2.write(w1, w2));
		assert(buffer2.bounds());
		assert(not buffer2.write(w3));
		assert(not buffer2.bounds());

		ConstRawBuffer buffer3(raw_buffer, 2);
		assert(buffer3.read(r1));
		assert(buffer3.read(r2));
		assert(buffer3.bounds());
		assert(not buffer3.read(r3));
		assert(not buffer3.bounds());
		assert(w1 == r1);
		assert(w2 == r2);

		ConstRawBuffer buffer4(raw_buffer, 2);
		assert(buffer4.read(r1, r2));
		assert(buffer4.bounds());
		assert(not buffer4.read(r3));
		assert(not buffer4.bounds());
		assert(w1 == r1);
		assert(w2 == r2);
	}

	static void test_skip_rewind() {
		unsigned raw_buffer_size = 3;
		unsigned char raw_buffer[raw_buffer_size];
		char w1 = 1;
		char w2 = 128;
		char w3 = 255;

		char r1 = 0;
		char r2 = 0;
		char r3 = 0;

		RawBuffer buffer(raw_buffer, raw_buffer_size);
		assert(buffer.write(w1));
		assert(buffer.skip(1));
		assert(buffer.rewind(2));
		assert(buffer.skip(2));
		assert(buffer.write(w3));
		assert(buffer.rewind());
		assert(buffer.bounds());

		assert(buffer.skip(1));
		assert(buffer.write(w2));
		assert(buffer.rewind(2));
		assert(buffer.skip(2));
		assert(buffer.write(w3));
		assert(buffer.rewind());
		assert(buffer.bounds());

		assert(buffer.read(r1));
		assert(buffer.skip(1));
		assert(buffer.rewind(2));
		assert(buffer.skip(2));
		assert(buffer.read(r3));
		assert(buffer.rewind());
		assert(buffer.bounds());

		assert(buffer.skip(1));
		assert(buffer.rewind(1));
		assert(buffer.skip(1));
		assert(buffer.read(r2));
		assert(buffer.skip(1));

		assert(buffer.bounds());

		assert(w1 == r1);
		assert(w2 == r2);
		assert(w3 == r3);
	}

	static void test_raii() {
		DataSet set_first(~0 - 1, ~0 - 2, ~0 - 3, ~0 - 4);
		DataSet set_second(1, 2, 3, 4);

		DataSet copy;

		unsigned raw_buffer_size = sizeof (DataSet) * 2;
		unsigned char raw_buffer[raw_buffer_size];

		RawBuffer buffer(raw_buffer, raw_buffer_size);
		assert(buffer.write(set_first));
		assert(buffer.bounds());

		RawBuffer buffer2(buffer);
		assert(buffer.write(set_second));
		assert(buffer.bounds());

		ConstRawBuffer const_buffer(buffer2);
		assert(const_buffer.rewind());
		assert(const_buffer.bounds());
		assert(const_buffer.read(copy));
		assert(copy == set_first);
		assert(const_buffer.read(copy));
		assert(copy == set_second);
		assert(const_buffer.bounds());

		copy = DataSet();
		ConstRawBuffer const_buffer2;
		const_buffer2 = buffer2;
		
		assert(const_buffer2.rewind());
		assert(const_buffer2.bounds());
		assert(const_buffer2.read(copy));
		assert(copy == set_first);
		assert(const_buffer2.read(copy));
		assert(copy == set_second);
		assert(const_buffer2.bounds());
	}

public:

	static void test() {
		test_read_write_assign();
		test_bounds();
		test_skip_rewind();
		test_raii();
		//test_array(); TODO:
	}

};

}; // namespace binio

#endif /* BINIO_RAW_BUFFER_TEST_H */