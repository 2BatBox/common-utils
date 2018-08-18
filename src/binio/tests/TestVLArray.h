#ifndef BINIO_TESTS_STATIC_ARRAY_H
#define BINIO_TESTS_STATIC_ARRAY_H

#include <assert.h>
#include <memory>
#include <iostream>
#include <cstdio>

#include "../VLArray.h"

namespace binio {

class TestVLArray {
	using Array8 = VLArray<uint8_t, 128>;
	using Array32 = VLArray<uint32_t, 128>;

	static void test_init_state() noexcept {
		Array8 array_8;
		assert(array_8.length() == 0);
		for (unsigned i = 0; i < array_8.capacity(); i++) {
			assert(array_8[i] == 0);
		}
	}

	static void test_comparison() noexcept {
		Array8 array_first;
		Array8 array_second;
		for (unsigned i = 0; i < array_first.capacity(); i++) {
			array_first[i] = i;
			*(array_second + i) = i;
		}
		array_first.assign(array_first.capacity());
		array_second.assign(array_second.capacity());

		assert(array_first.length() == array_first.capacity());
		assert(array_second.length() == array_second.capacity());
		assert(array_first == array_second);

		array_first[array_first.length() - 1] = 0;
		assert(array_first != array_second);
	}
	
	static void test_assign() noexcept {
		Array8 array_8;
		array_8.assign(0);
		array_8.assign(array_8.capacity() - 1);
		array_8.assign(array_8.capacity());
		try {
			array_8.assign(array_8.capacity() + 1);
			assert(false);
		} catch (const std::out_of_range& e) {
		}
	}

public:

	static void test() {
		test_init_state();
		test_comparison();
		test_assign();
	}

};

}; // namespace binio

#endif /* BINIO_TESTS_STATIC_ARRAY_H */