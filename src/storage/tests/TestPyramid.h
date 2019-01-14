#ifndef STORAGE_TESTS_TESTPYRAMID_H
#define STORAGE_TESTS_TESTPYRAMID_H

#include "../Pyramid.h"

#include <cstdio>
#include <assert.h>
#include <iostream>
#include <thread>
#include <algorithm>

namespace storage {

class TestPyramid {
	using Type_t = unsigned;
	using Pyramid_t = Pyramid<Type_t>;

	Pyramid_t m_pyramid;
	const size_t m_capacity;
	Type_t* m_storage;

public:

	TestPyramid(size_t capacity) noexcept
		: m_pyramid(nullptr, capacity)
		, m_capacity(capacity) {
		m_storage = new Type_t[capacity];
		assert(m_storage);
		m_pyramid.m_head = m_storage;
	}

	TestPyramid(const TestPyramid&) = delete;
	TestPyramid(TestPyramid&&) = delete;

	TestPyramid operator=(const TestPyramid&) = delete;
	TestPyramid operator=(TestPyramid&&) = delete;

	~TestPyramid() {
		delete[] m_storage;
	}

	void test() noexcept {
		printf("<TestPyramid>...\n");
		printf("capacity=%zu\n", m_pyramid.capacity());

		unsigned step = 1;
//		test_iterators(step++);
//		test_height(step++);
//		test_build_peek_pop(step++);
//		test_insert_peek_pop(step++);

//		test_std_sort(step++);
		test_pyramid_sort_build(step++);

//		test_indexing_capacity(step++);
//		test_common_indexing(step++);
//		test_layers(step++);

//		dump();
	}

private:

	void test_iterators(unsigned step) noexcept {
		printf("-> test_iterators(step=%u)\n", step);
		for(size_t i = 0; i < m_capacity; ++i) {
			m_storage[i] = i;
		}
		m_pyramid.m_size = m_capacity;

		size_t index = 0;
		for(auto const& t: m_pyramid) {
			assert(t == index);
			index++;
		}

		index = 0;
		for(auto it = m_pyramid.begin(); it != m_pyramid.end(); ++it) {
			assert(*it == index);
			index++;
		}

		index = 0;
		for(Pyramid_t::Height_t h = 0; h < m_pyramid.height(); h++) {
			for(auto it = m_pyramid.begin(h); it != m_pyramid.end(h); ++it) {
				assert(*it == index);
				index++;
			}
		}
		m_pyramid.m_size = 0;
	}

	void test_height(unsigned step) noexcept {
		printf("-> test_height(step=%u)\n", step);
		m_pyramid.m_size = 0;
		assert(m_pyramid.height() == 0);
		m_pyramid.m_size = 1;
		assert(m_pyramid.height() == 1);
		m_pyramid.m_size = 2;
		assert(m_pyramid.height() == 2);
		m_pyramid.m_size = 3;
		assert(m_pyramid.height() == 2);
		m_pyramid.m_size = 4;
		assert(m_pyramid.height() == 3);
		m_pyramid.m_size = 5;
		assert(m_pyramid.height() == 3);
		m_pyramid.m_size = 6;
		assert(m_pyramid.height() == 3);
		m_pyramid.m_size = 7;
		assert(m_pyramid.height() == 3);
		m_pyramid.m_size = 8;
		assert(m_pyramid.height() == 4);
		m_pyramid.m_size = 0;
	}

	void test_build_peek_pop(unsigned step) noexcept {
		printf("-> test_build_peek_pop(step=%u)\n", step);
		assert(m_pyramid.size() == 0);
		for(size_t i = 0; i < m_capacity; ++i) {
			m_storage[i] = m_capacity - i - 1;
		}
		auto result = m_pyramid.build(m_capacity);
		assert(result == m_capacity);
		validate_pyramid();
	}

	void test_insert_peek_pop(unsigned step) noexcept {
		printf("-> test_insert_peek_pop(step=%u)\n", step);
		assert(m_pyramid.size() == 0);
		for(size_t i = 0; i < m_capacity; ++i) {
			m_pyramid.insert(m_capacity - i - 1);
		}
		validate_pyramid();
	}

	void validate_pyramid() noexcept {
		const Type_t* cptr;
		Type_t* ptr;
		for(size_t i = 0; i < m_capacity; ++i) {
			cptr = m_pyramid.peek();
			ptr = m_pyramid.peek();
			assert(cptr);
			assert(ptr);
			assert(*cptr == i);
			assert(*ptr == i);
		}
		assert(m_pyramid.size() == 0);
		assert(m_pyramid.peek() == nullptr);
	}

	void test_std_sort(unsigned step) noexcept {
		printf("-> test_std_sort(step=%u)\n", step);
		size_t check_summ = randomize_storage();
		std::sort(m_storage, m_storage + m_capacity);
		validate_storage(check_summ);
		printf("check_summ=0x%zx\n", check_summ);
	}

	void test_pyramid_sort_build(unsigned step) noexcept {
		printf("-> test_pyramid_sort_build(step=%u)\n", step);
		size_t check_summ = randomize_storage();

		auto result = m_pyramid.build(m_capacity);
		assert(result == m_capacity);

		for(size_t i = 0; i < m_capacity; ++i) {
			m_pyramid.pop_swap();
		}

		validate_storage(check_summ);
		printf("check_summ=0x%zx\n", check_summ);
	}

	size_t randomize_storage(size_t seed = 0) noexcept {
		size_t summ = 0;
		srand(seed);
		for(size_t i = 0; i < m_capacity; ++i) {
			m_storage[i] = rand();
			summ += m_storage[i];
		}
		return summ;
	}

	void validate_storage(size_t check_summ) noexcept {
		size_t summ = 0;
		Type_t min = 0;
		for(size_t i = 0; i < m_capacity; ++i) {
			assert(min <= m_storage[i]);
			min = m_storage[i];
			summ += m_storage[i];
		}
		assert(summ == check_summ);
	}

	void test_indexing_capacity(unsigned step) noexcept {
		printf("-> test_indexing_capacity(step=%u)\n", step);
		assert(capacity_max(0) == 0);
		assert(capacity_max(1) == 1);
		assert(capacity_max(2) == 3);
		assert(capacity_max(3) == 7);
		assert(capacity_max(4) == 15);
		assert(capacity_max((sizeof(size_t) * 8) - 1));
		assert(capacity_max((sizeof(size_t) * 8)));
		assert(capacity_max((sizeof(size_t) * 8) + 1) == 0);

		assert(capacity_min(0) == 0);
		assert(capacity_min(1) == 1);
		assert(capacity_min(2) == 2);
		assert(capacity_min(3) == 4);
		assert(capacity_min(4) == 8);
		assert(capacity_min((sizeof(size_t) * 8) - 1));
		assert(capacity_min((sizeof(size_t) * 8)));
		assert(capacity_min((sizeof(size_t) * 8) + 1) == 0);
	}

	void test_common_indexing(unsigned step) noexcept {
		printf("-> test_common_indexing(step=%u)\n", step);
		assert(Pyramid_t::left(0) == 1);
		assert(Pyramid_t::right(0) == 2);

		assert(Pyramid_t::left(1) == 3);
		assert(Pyramid_t::right(1) == 4);
		assert(Pyramid_t::parent(1) == 0);
		assert(Pyramid_t::parent(2) == 0);
		assert(Pyramid_t::parent(0xFFFFFFFFFFFFFFFF) == 0x7fffffffffffffff);

		assert(Pyramid_t::layer_offset(0) == 0);
		assert(Pyramid_t::layer_offset(1) == 1);
		assert(Pyramid_t::layer_offset(2) == 3);
		assert(Pyramid_t::layer_offset(3) == 7);
		assert(Pyramid_t::layer_offset(4) == 15);
		assert(Pyramid_t::layer_offset(sizeof(size_t) * 8) - 1);

//		assert(first_leaf(0) == 0);
//		assert(first_leaf(1) == 0);
//		assert(first_leaf(2) == 1);
//		assert(first_leaf(3) == 1);
//		assert(first_leaf(4) == 2);
//		assert(first_leaf(5) == 2);
//		assert(first_leaf(0xFFFFFFFFFFFFFFFF) == 0x7fffffffffffffff);
//
//		assert(leaves(0) == 0);
//		assert(leaves(1) == 1);
//		assert(leaves(2) == 1);
//		assert(leaves(3) == 2);
//		assert(leaves(4) == 2);
//		assert(leaves(5) == 3);
//		assert(leaves(0xFFFFFFFFFFFFFFFF - 1) == 0x7fffffffffffffff);
	}

	void test_layers(unsigned step) noexcept {
		printf("-> test_layers(step=%u)\n", step);
		assert(layer_capacity(0) == 1);
		assert(layer_capacity(1) == 2);
		assert(layer_capacity(2) == 4);
		assert(layer_capacity(3) == 8);
		assert(layer_capacity(4) == 16);
		assert(layer_capacity(sizeof(size_t) * 8) - 1);

	}

	void dump(size_t mark = ~size_t(0)) const noexcept {

		printf("size=%zu", m_pyramid.size());
		printf(" capacity=%zu", m_pyramid.capacity());
		printf(" leaves=%zu", m_pyramid.leaves());
		printf("\n");

		size_t index = 0;
		for(Pyramid_t::Height_t h = 0; h < m_pyramid.height(); h++) {
			printf("[%u] : ", h);
			for(auto it = m_pyramid.begin(h); it != m_pyramid.end(h); ++it) {
				if(mark == index) {
					printf("[%u] ", *it);
				} else {
					printf("%-3u  ", *it);
				}
				index++;
			}
			printf("\n");
		}

		for(auto const t : m_pyramid) {
			printf("%u ", t);
		}
		printf("\n");
	}

	// common indexing methods

	static inline size_t capacity_max(Pyramid_t::Height_t height) noexcept {
		size_t result = 0;
		const Pyramid_t::Height_t arch_bits = sizeof(size_t) * 8;
		if(height > 0 && height < arch_bits) {
			result = size_t(1) << size_t(height);
			result = result - size_t(1);
		} else if(height == arch_bits) {
			result = ~result;
		}
		return result;
	}

	static inline size_t capacity_min(Pyramid_t::Height_t height) noexcept {
		size_t result = 0;
		if(height > 0 && height <= sizeof(size_t) * 8) {
			result = size_t(1) << (height - 1);
		}
		return result;
	}


	static inline size_t layer_capacity(Pyramid_t::Height_t layer) noexcept {
		size_t result = 0;
		if(layer < sizeof(size_t) * 8) {
			result = size_t(1) << size_t(layer);
		}
		return result;
	}

};

}; // namespace storage

#endif /* STORAGE_TESTS_TESTPYRAMID_H */

