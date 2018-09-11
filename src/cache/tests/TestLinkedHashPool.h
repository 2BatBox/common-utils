#ifndef CACHE_TESTS_TESTLRUCACHE_H
#define CACHE_TESTS_TESTLRUCACHE_H

#include "../LinkedHashPool.h"

#include <assert.h>
#include <iostream>

namespace cache {

class TestLinkedHashPool {

	template <typename T>
	struct StructValue {
		T value;

		StructValue() : value(0) { };

		StructValue(T x) : value(x) { };

		bool operator==(const StructValue& st_val) const {
			return value == st_val.value;
		}

		bool operator==(const T& val) const {
			return value == val;
		}
	};

	using Key_t = unsigned;
	using Value_t = StructValue<long long unsigned>;

	using Node_t = LinkedHashPoolNode<Key_t, Value_t>;
	using LruCache_t = LinkedHashPool<Node_t>;

	LruCache_t m_pool;
	const size_t m_capacity;

public:

	TestLinkedHashPool(unsigned capacity, float load_factor) noexcept
	: m_pool(capacity, load_factor), m_capacity(capacity) {
		assert(m_pool.allocate() == 0);
	}

	TestLinkedHashPool(const TestLinkedHashPool&) = delete;
	TestLinkedHashPool(TestLinkedHashPool&&) = delete;

	TestLinkedHashPool operator=(const TestLinkedHashPool&) = delete;
	TestLinkedHashPool operator=(TestLinkedHashPool&&) = delete;

	~TestLinkedHashPool() { }

	void test() noexcept {
		float mem_used = m_pool.m_map.buckets() * sizeof (LruCache_t::Bucket_t);
		mem_used += m_pool.capacity() * sizeof (Node_t);
		printf("<TestLinkedHashPool>...\n");
		printf("sizeof(Node_t)=%zu\n", sizeof (Node_t));
		printf("sizeof(LruCache_t::Bucket_t)=%zu\n", sizeof (LruCache_t::Bucket_t));
		printf("capacity=%zu\n", m_capacity);
		printf("buckets=%zu\n", m_pool.m_map.buckets());
		printf("memory used %.2f Kb\n", mem_used / 1024);

		unsigned step = 1;
		test_iterators(step++);
		test_push_pop_sequential(step++);
		test_push_remove_sequential(step++);
		test_push_pop_odd_even(step++);
		test_push_remove_odd_even(step++);
		test_move_back(step++);
		test_clear(step++);
	}

	void test_iterators(unsigned step) {
		printf("-> test_iterators()\n");
		assert(m_pool.size() == 0);

		for (size_t i = 0; i < m_capacity; i++) {
			push_back_new(i * step, i + step);
		}

		size_t index = 0;
		for (auto& elem : m_pool) {
			assert(elem.im_key == index * step);
			assert(elem.value == index + step);
			index++;
		}

		index = 0;
		for (auto it = m_pool.begin(); it != m_pool.end(); ++it) {
			assert(it->im_key == index * step);
			assert(it->value == index + step);
			index++;
		}

		index = 0;
		for (auto it = m_pool.begin(); it != m_pool.end(); it++) {
			assert(it->im_key == index * step);
			assert(it->value == index + step);
			index++;
		}

		index = 0;
		for (auto it = m_pool.cbegin(); it != m_pool.cend(); ++it) {
			assert(it->im_key == index * step);
			assert(it->value == index + step);
			index++;
		}

		index = 0;
		for (auto it = m_pool.cbegin(); it != m_pool.cend(); it++) {
			assert(it->im_key == index * step);
			assert(it->value == index + step);
			index++;
		}

		index = m_capacity - 1;
		for (auto it = m_pool.rbegin(); it != m_pool.rend(); ++it) {
			assert(it->im_key == index * step);
			assert(it->value == index + step);
			index--;
		}

		index = m_capacity - 1;
		for (auto it = m_pool.rbegin(); it != m_pool.rend(); it++) {
			assert(it->im_key == index * step);
			assert(it->value == index + step);
			index--;
		}

		index = m_capacity - 1;
		for (auto it = m_pool.crbegin(); it != m_pool.crend(); ++it) {
			assert(it->im_key == index * step);
			assert(it->value == index + step);
			index--;
		}

		index = m_capacity - 1;
		for (auto it = m_pool.crbegin(); it != m_pool.crend(); it++) {
			assert(it->im_key == index * step);
			assert(it->value == index + step);
			index--;
		}

		for (size_t i = 0; i < m_capacity; i++) {
			find_one(i * step, i + step);
			peek_front_one(i * step, i + step);
			pop_front_one(i * step, i + step);
			miss_one(i * step);
		}

		assert(m_pool.size() == 0);
		test_sanity();
	}

	void test_push_pop_sequential(unsigned step) {
		printf("-> test_push_pop_sequential()\n");
		assert(m_pool.size() == 0);

		for (size_t i = 0; i < m_capacity * 2; i++) {
			if (i < m_capacity) {
				push_back_new(i * step, i + step);
				push_back_reused(i * step, i + step);
			} else {
				push_back_oversize(i * step);
			}
		}

		for (size_t i = 0; i < m_capacity; i++) {
			find_one(i * step, i + step);
			peek_front_one(i * step, i + step);
			pop_front_one(i * step, i + step);
			miss_one(i * step);
		}

		assert(m_pool.size() == 0);
		test_sanity();
	}

	void test_push_remove_sequential(unsigned step) {
		printf("-> test_push_remove_sequential()\n");
		assert(m_pool.size() == 0);

		for (size_t i = 0; i < m_capacity * 2; i++) {
			if (i < m_capacity) {
				push_back_new(i * step, i + step);
				push_back_reused(i * step, i + step);
			} else {
				push_back_oversize(i * step);
			}
		}

		for (size_t i = 0; i < m_capacity; i++) {
			find_one(i * step, i + step);
			peek_front_one(i * step, i + step);
			remove_one(i * step, i + step);
			miss_one(i * step);
		}

		assert(m_pool.size() == 0);
		test_sanity();
	}

	void test_push_pop_odd_even(unsigned step) {
		printf("-> test_push_pop_odd_even()\n");
		assert(m_pool.size() == 0);

		for (size_t i = 0; i < m_capacity; i++) {
			if (i % 2 != 0) {
				push_back_new(i * step, i + step);
				push_back_reused(i * step, i + step);
			}
		}

		for (size_t i = 0; i < m_capacity; i++) {
			if (i % 2 != 0) {
				find_one(i * step, i + step);
				peek_front_one(i * step, i + step);
				pop_front_one(i * step, i + step);
				miss_one(i * step);
			} else {
				miss_one(i * step);
			}
		}

		assert(m_pool.size() == 0);
		test_sanity();
	}

	void test_push_remove_odd_even(unsigned step) {
		printf("-> test_push_remove_odd_even()\n");
		assert(m_pool.size() == 0);

		for (size_t i = 0; i < m_capacity; i++) {
			if (i % 2 != 0) {
				push_back_new(i * step, i + step);
				push_back_reused(i * step, i + step);
			}
		}

		for (size_t i = 0; i < m_capacity; i++) {
			if (i % 2 != 0) {
				find_one(i * step, i + step);
				remove_one(i * step, i + step);
				miss_one(i * step);
			} else {
				miss_one(i * step);
			}
		}

		assert(m_pool.size() == 0);
		test_sanity();
	}

	void test_move_back(unsigned step) noexcept {
		printf("-> test_move_back()\n");

		for (Key_t i = 0; i < m_capacity; i++) {
			push_back_new(i * step, i + step);
			push_back_reused(i * step, i + step);
		}

		for (Key_t i = 0; i < m_capacity; i++) {
			if (i % 2 == 0) {
				move_back_one(i * step, i + step);
			}
		}

		for (Key_t i = 0; i < m_capacity; i++) {
			if (i % 2 != 0) {
				push_back_reused(i * step, i + step + 1);
			}
		}

		for (Key_t i = 0; i < m_capacity; i++) {
			if (i % 2 == 0) {
				find_one(i * step, i + step);
				remove_one(i * step, i + step);
				miss_one(i * step);
			} else {
				find_one(i * step, i + step + 1);
				remove_one(i * step, i + step + 1);
				miss_one(i * step);
			}
		}
		assert(m_pool.size() == 0);
		test_sanity();
	}

	void test_recycle(unsigned step) noexcept {
		printf("-> test_recycle()\n");

		for (Key_t i = 0; i < m_capacity * 2; i++) {
			if (i < m_capacity) {
				push_back_new(i * step, i + step);
				push_back_reused(i * step, i + step);
			} else {
				push_back_reused(i * step, i + step);
			}
		}

		for (Key_t i = 0; i < m_capacity * 2; i++) {
			if (i < m_capacity) {
				miss_one(i * step);
			} else {
				remove_one(i * step, i + step);
				miss_one(i * step);
			}
		}

		assert(m_pool.size() == 0);
		test_sanity();
	}

	void test_clear(unsigned step) {
		printf("-> test_clear()\n");
		for (size_t i = 0; i < m_capacity; i++) {
			push_back_new(i * step, i + step);
		}
		m_pool.clear();

		for (size_t i = 0; i < m_capacity; i++) {
			miss_one(i * step);
		}

		assert(m_pool.size() == 0);
		test_sanity();
	}

	void dump() {
		std::cout << "map has " << m_pool.m_map.size() << " elements \n";
		for (size_t bucket = 0; bucket < m_pool.m_map.buckets(); ++bucket) {
			std::cout << "B[" << bucket << "] ";
			for (auto it = m_pool.m_map.cbegin(bucket); it != m_pool.m_map.cend(); ++it) {
				std::cout << "[" << &(*it) << "] " << (*it).im_key << ":" << (*it).value.value << " -> ";
			}
			std::cout << "\n";
		}
		std::cout << "cached list has " << m_pool.m_list_cached.size() << " elements \n";
		for (auto it = m_pool.m_list_cached.cbegin(); it != m_pool.m_list_cached.cend(); ++it) {
			std::cout << (*it).value.value << ", ";
		}
		std::cout << "\n";
		std::cout << "freed list has " << m_pool.m_list_freed.size() << " elements \n";
		for (auto it = m_pool.m_list_freed.cbegin(); it != m_pool.m_list_freed.cend(); ++it) {
			std::cout << (*it).value.value << ", ";
		}
		std::cout << "\n";
	}

private:

	void push_back_new(const Key_t& key, const Value_t& value) noexcept {
		bool reused = true;
		auto it = m_pool.find(key);
		assert(it == m_pool.end());
		auto rit = m_pool.push_back(key, reused);
		assert(rit != m_pool.end());
		assert(not reused);
		rit->value = value;
		assert(rit->im_key == key);
		assert(rit->im_linked);
	}

	void push_back_reused(const Key_t& key, const Value_t& value) noexcept {
		bool reused = false;
		auto it = m_pool.push_back(key, reused);
		assert(it != m_pool.end());
		assert(reused);
		it->value = value;
		assert(it->im_key == key);
		assert(it->im_linked);
	}

	void push_back_oversize(const Key_t& key) noexcept {
		bool recycled = false;
		auto it = m_pool.push_back(key, recycled);
		assert(it == m_pool.end());
	}

	void peek_front_one(const Key_t& key, const Value_t& value) noexcept {
		auto it = m_pool.begin();
		assert(it != m_pool.end());
		it->value = value;
		assert(it->im_key == key);
		assert(it->im_linked);
	}

	void pop_front_one(const Key_t& key, const Value_t& value) noexcept {
		auto it = m_pool.pop_front();
		assert(it != m_pool.end());
		it->value = value;
		assert(it->im_key == key);
		assert(not it->im_linked);
	}

	void move_back_one(const Key_t& key, const Value_t& value) noexcept {
		auto it = m_pool.find(key);
		assert(it != m_pool.end());
		it->value = value;
		assert(it->im_key == key);
		assert(it->im_linked);
		m_pool.move_back(it);
	}

	void find_one(const Key_t& key, const Value_t& value) noexcept {
		auto it = m_pool.find(key);
		assert(it != m_pool.end());
		assert(it->value == value);
		assert(it->im_key == key);
		assert(it->il_linked);
		assert(it->im_linked);
	}

	void miss_one(const Key_t& key) noexcept {
		auto it = m_pool.find(key);
		assert(it == m_pool.end());
	}

	void remove_one(const Key_t& key, const Value_t& value) noexcept {
		auto it = m_pool.find(key);
		assert(it != m_pool.end());
		assert(it->value == value);
		assert(it->im_key == key);
		assert(it->il_linked);
		m_pool.remove(it);
		assert(it->il_linked);
		assert(not it->im_linked);
		it = m_pool.find(key);
		assert(it == m_pool.end());
	}

	void test_sanity() {
		for (Key_t i = 0; i < m_capacity; i++) {
			assert(not m_pool.m_storage[i].im_linked);
			assert(m_pool.m_storage[i].il_linked);
		}
	}

};

}; // namespace cache

#endif /* CACHE_TESTS_TESTLRUCACHE_H */

