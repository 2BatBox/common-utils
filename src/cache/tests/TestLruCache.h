#ifndef CACHE_TESTS_TESTLRUCACHE_H
#define CACHE_TESTS_TESTLRUCACHE_H

#include "../LruCache.h"

#include <assert.h>
#include <iostream>

namespace cache {

class TestLruCache {

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

	using Node_t = LruCacheNode<Key_t, Value_t>;
	using LruCache_t = LruCache<Node_t>;

	LruCache_t m_map;
	const size_t m_capacity;

public:

	TestLruCache(unsigned capacity, float load_factor) noexcept
	: m_map(capacity, load_factor), m_capacity(capacity) {
		if (not m_map.allocate())
			throw std::logic_error("Cannot allocate LruCache instance");
	}

	TestLruCache(const TestLruCache&) = delete;
	TestLruCache(TestLruCache&&) = delete;

	TestLruCache operator=(const TestLruCache&) = delete;
	TestLruCache operator=(TestLruCache&&) = delete;

	~TestLruCache() { }

	void test() noexcept {
		printf("<TestLruCache>...\n");
		printf("sizeof(Node_t)=%zu\n", sizeof (Node_t));
		printf("sizeof(LruCache_t::Bucket_t)=%zu\n", sizeof (LruCache_t::Bucket_t));
		printf("capacity=%zu\n", m_capacity);
		printf("buckets=%zu\n", m_map.m_map.buckets());
		printf("memory used %zu Kb\n", m_map.storage_bytes() / (1024));

		unsigned step = 1;
		test_push_pop_sequential(step++);
		test_push_remove_sequential(step++);
		test_push_pop_odd_even(step++);
		test_push_remove_odd_even(step++);
		test_update(step++);
		test_recycle(step++);
		test_clear(step++);
	}

	void test_push_pop_sequential(unsigned step) {
		printf("-> test_push_pop_sequential()\n");
		assert(m_map.size() == 0);

		for (size_t i = 0; i < m_capacity * 2; i++) {
			if (i < m_capacity) {
				push_back_new(i * step, i + step);
				push_back_recycle(i * step, i + step);
			} else {
				push_back_recycle(i * step, i + step);
			}
		}

		for (size_t i = 0; i < m_capacity * 2; i++) {
			if (i < m_capacity) {
				miss_one(i * step);
			} else {
				find_one(i * step, i + step);
				peek_front_one(i * step, i + step);
				pop_front_one(i * step, i + step);
				miss_one(i * step);
			}
		}

		assert(m_map.size() == 0);
		test_sanity();
	}

	void test_push_remove_sequential(unsigned step) {
		printf("-> test_push_remove_sequential()\n");
		assert(m_map.size() == 0);

		for (size_t i = 0; i < m_capacity * 2; i++) {
			if (i < m_capacity) {
				push_back_new(i * step, i + step);
				push_back_recycle(i * step, i + step);
			} else {
				push_back_recycle(i * step, i + step);
			}
		}

		for (size_t i = 0; i < m_capacity * 2; i++) {
			if (i < m_capacity) {
				miss_one(i * step);
			} else {
				find_one(i * step, i + step);
				peek_front_one(i * step, i + step);
				remove_one(i * step, i + step);
				miss_one(i * step);
			}
		}

		assert(m_map.size() == 0);
		test_sanity();
	}

	void test_push_pop_odd_even(unsigned step) {
		printf("-> test_push_pop_odd_even()\n");
		assert(m_map.size() == 0);

		for (size_t i = 0; i < m_capacity * 2; i++) {
			if (i % 2 != 0) {
				push_back_new(i * step, i + step);
				push_back_recycle(i * step, i + step);
			}
		}

		for (size_t i = 0; i < m_capacity * 2; i++) {
			if (i % 2 == 0) {
				push_back_recycle(i * step, i + step);
			}
		}

		for (size_t i = 0; i < m_capacity * 2; i++) {
			if (i % 2 == 0) {
				find_one(i * step, i + step);
				peek_front_one(i * step, i + step);
				pop_front_one(i * step, i + step);
				miss_one(i * step);
			} else {
				miss_one(i * step);
			}
		}

		assert(m_map.size() == 0);
		test_sanity();
	}

	void test_push_remove_odd_even(unsigned step) {
		printf("-> test_push_remove_odd_even()\n");
		assert(m_map.size() == 0);

		for (size_t i = 0; i < m_capacity * 2; i++) {
			if (i % 2 != 0) {
				push_back_new(i * step, i + step);
				push_back_recycle(i * step, i + step);
			}
		}

		for (size_t i = 0; i < m_capacity * 2; i++) {
			if (i % 2 == 0) {
				push_back_recycle(i * step, i + step);
			}
		}

		for (size_t i = 0; i < m_capacity * 2; i++) {
			if (i % 2 == 0) {
				find_one(i * step, i + step);
				remove_one(i * step, i + step);
				miss_one(i * step);
			} else {
				miss_one(i * step);
			}
		}

		assert(m_map.size() == 0);
		test_sanity();
	}

	void test_update(unsigned step) noexcept {
		printf("-> test_update()\n");

		for (Key_t i = 0; i < m_capacity; i++) {
			push_back_new(i * step, i + step);
			push_back_recycle(i * step, i + step);
		}

		for (Key_t i = 0; i < m_capacity; i++) {
			if (i % 2 == 0) {
				update_one(i * step, i + step);
			}
		}

		for (Key_t i = 0; i < m_capacity; i++) {
			if (i % 2 != 0) {
				push_back_recycle(i * step, i + step + 1);
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
		assert(m_map.size() == 0);
		test_sanity();
	}

	void test_recycle(unsigned step) noexcept {
		printf("-> test_recycle()\n");

		for (Key_t i = 0; i < m_capacity * 2; i++) {
			if (i < m_capacity) {
				push_back_new(i * step, i + step);
				push_back_recycle(i * step, i + step);
			} else {
				push_back_recycle(i * step, i + step);
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

		assert(m_map.size() == 0);
		test_sanity();
	}

	void test_clear(unsigned step) {
		printf("-> test_clear()\n");
		for (size_t i = 0; i < m_capacity; i++) {
			push_back_new(i * step, i + step);
		}
		m_map.clear();

		for (size_t i = 0; i < m_capacity; i++) {
			miss_one(i * step);
		}

		assert(m_map.size() == 0);
		test_sanity();
	}

	void dump() {
		std::cout << "map has " << m_map.m_map.size() << " elements \n";
		for (size_t bucket = 0; bucket < m_map.m_map.buckets(); ++bucket) {
			std::cout << "B[" << bucket << "] ";
			for (auto it = m_map.m_map.cbegin(bucket); it != m_map.m_map.cend(); ++it) {
				std::cout << "[" << &(*it) << "] " << (*it).im_key << ":" << (*it).value.value << " -> ";
			}
			std::cout << "\n";
		}
		std::cout << "cached list has " << m_map.m_list_cached.size() << " elements \n";
		for (auto it = m_map.m_list_cached.cbegin(); it != m_map.m_list_cached.cend(); ++it) {
			std::cout << (*it).value.value << ", ";
		}
		std::cout << "\n";
		std::cout << "freed list has " << m_map.m_list_freed.size() << " elements \n";
		for (auto it = m_map.m_list_freed.cbegin(); it != m_map.m_list_freed.cend(); ++it) {
			std::cout << (*it).value.value << ", ";
		}
		std::cout << "\n";
	}

private:

	void push_back_new(const Key_t& key, const Value_t& value) noexcept {
		bool recycled = true;
		auto it = m_map.find(key);
		assert(it == m_map.end());
		it = m_map.push_back(key, recycled);
		assert(it != m_map.end());
		assert(not recycled);
		it->value = value;
		assert(it->im_key == key);
		assert(it->im_linked);
	}

	void push_back_recycle(const Key_t& key, const Value_t& value) noexcept {
		bool recycled = false;
		auto it = m_map.push_back(key, recycled);
		assert(it != m_map.end());
		assert(recycled);
		it->value = value;
		assert(it->im_key == key);
		assert(it->im_linked);
	}

	void peek_front_one(const Key_t& key, const Value_t& value) noexcept {
		auto it = m_map.peek_front();
		assert(it != m_map.end());
		it->value = value;
		assert(it->im_key == key);
		assert(it->im_linked);
	}

	void pop_front_one(const Key_t& key, const Value_t& value) noexcept {
		auto it = m_map.pop_front();
		assert(it != m_map.end());
		it->value = value;
		assert(it->im_key == key);
		assert(not it->im_linked);
	}

	void update_one(const Key_t& key, const Value_t& value) noexcept {
		auto it = m_map.find(key);
		assert(it != m_map.end());
		it->value = value;
		assert(it->im_key == key);
		assert(it->im_linked);
		m_map.update(it);
	}

	void find_one(const Key_t& key, const Value_t& value) noexcept {
		auto it = m_map.find(key);
		assert(it != m_map.end());
		assert(it->value == value);
		assert(it->im_key == key);
		assert(it->il_linked);
		assert(it->im_linked);
	}

	void miss_one(const Key_t& key) noexcept {
		auto it = m_map.find(key);
		assert(it == m_map.end());
	}

	void remove_one(const Key_t& key, const Value_t& value) noexcept {
		auto it = m_map.find(key);
		assert(it != m_map.end());
		assert(it->value == value);
		assert(it->im_key == key);
		assert(it->il_linked);
		assert(it->im_linked);
		m_map.remove(it);
		assert(it->il_linked);
		assert(not it->im_linked);
		it = m_map.find(key);
		assert(it == m_map.end());
	}

	void test_sanity() {
		for (Key_t i = 0; i < m_capacity; i++) {
			assert(not m_map.m_storage[i].im_linked);
			assert(m_map.m_storage[i].il_linked);
		}
	}

};

}; // namespace cache

#endif /* CACHE_TESTS_TESTLRUCACHE_H */

