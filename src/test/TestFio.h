#pragma once

#include "test_environment.h"
#include <fio/Reader.h>
#include <fio/Writer.h>

#include <cstring>

class TestFio {

public:

	TestFio() noexcept {
		test_empty_input();
	}

private:

	void test_empty_input() noexcept {
		TEST_TRACE;

		struct Dummy {
			bool b;
			short s;
			long l;

			bool operator==(const Dummy& rv) const {
				return b == rv.b && s == rv.s && l == rv.l;
			}
		};

		const char ch_ref = 'c';
		const short s_ref = 17;
		const int i_ref = 0x0abbccdd;
		const Dummy st_ref {false, 22, 3333333};
		const std::string hello_dick_ref = "hello.dick";

		fio::Writer writer(hello_dick_ref);
		assert(writer.open());
		assert(writer.write(ch_ref, s_ref, i_ref, st_ref, hello_dick_ref));
		writer.close();

		char ch;
		short s;
		int i;
		Dummy st;
		std::string hello_dick;

		fio::Reader reader(hello_dick_ref);
		assert(reader.open());
		assert(reader.read(ch, s, i, st, hello_dick));
		reader.close();

		assert(ch_ref == ch);
		assert(s_ref == s);
		assert(i_ref == i);
		assert(st_ref == st);

//		printf("'%s' %zu\n", hello_dick_ref.c_str(), hello_dick_ref.size());
//		printf("'%s' %zu\n", hello_dick.c_str(), hello_dick.size());

		assert(hello_dick_ref == hello_dick);
	}

};
