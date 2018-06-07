#ifndef CACHE_TESTS_TEST_LRU_CACHE_H
#define CACHE_TESTS_TEST_LRU_CACHE_H

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

	using Node = LruCacheNode<Key_t, Value_t>;
	using LruCache_t = LruCache<Node>;

	LruCache_t cache;
	const size_t capacity;

public:

	TestLruCache(unsigned capacity, float load_factor) noexcept
	: cache(capacity, load_factor), capacity(capacity) {
		if (not cache.allocate())
			throw std::logic_error("Cannot allocate LruCache instance");
	}

	TestLruCache(const TestLruCache&) = delete;
	TestLruCache(TestLruCache&&) = delete;

	TestLruCache operator=(const TestLruCache&) = delete;
	TestLruCache operator=(TestLruCache&&) = delete;

	~TestLruCache() { }

	void test() noexcept {
		printf("<LruCacheTest>...\n");
		printf("sizeof(Data_t)=%zu\n", sizeof (Node));
		printf("sizeof(Bucket_t)=%zu\n", sizeof (LruCache_t::Bucket_t));
		printf("capacity=%zu\n", capacity);
		printf("buckets=%zu\n", cache.m_map.buckets());
		printf("memory used %zu Kb\n", cache.storage_bytes() / (1024));
		test_put_find_remove();
		test_lru();
		test_update();
		test_clear();
	}

	void test_put_find_remove() {
		assert(cache.size() == 0);
		unsigned step = 0;

		// forward
		step++;
		for (size_t i = 0; i < capacity; i++) {
			put_once(i * step, i + step);
		}
		for (size_t i = 0; i < capacity; i++) {
			find_once(i * step, i + step);
			find_once_const(i * step, i + step);
		}
		for (size_t i = 0; i < capacity; i++) {
			remove_once(i * step, i + step);
		}
		assert(cache.size() == 0);
		test_sanity();

		// odd
		step++;
		for (size_t i = 0; i < capacity; i++) {
			if (i % 2 != 0) {
				put_once(i * step, i + step);
			}
		}
		for (size_t i = 0; i < capacity; i++) {
			if (i % 2 != 0) {
				find_once(i * step, i + step);
				find_once_const(i * step, i + step);
			} else {
				miss_once(i * step);
			}
		}
		for (size_t i = 0; i < capacity; i++) {
			if (i % 2 != 0) {
				remove_once(i * step, i + step);
			}
		}
		assert(cache.size() == 0);
		test_sanity();

		// even
		step++;
		for (size_t i = 0; i < capacity; i++) {
			if (i % 2 == 0) {
				put_once(i * step, i + step);
			}
		}
		for (size_t i = 0; i < capacity; i++) {
			if (i % 2 == 0) {
				find_once(i * step, i + step);
				find_once_const(i * step, i + step);
			} else {
				miss_once(i * step);
			}
		}
		for (size_t i = 0; i < capacity; i++) {
			if (i % 2 == 0) {
				remove_once(i * step, i + step);
			}
		}
		assert(cache.size() == 0);
		test_sanity();
	}

	void test_lru() noexcept {
		// odd / even
		for (Key_t index = 0; index < capacity * 2; index++) {
			put_once(index, index);
		}

		for (Key_t index = 0; index < capacity * 2; index++) {
			if (index < capacity) {
				miss_once(index);
			} else {
				find_once(index, index);
				find_once_const(index, index);
				remove_once(index, index);
			}
		}
		assert(cache.size() == 0);
		test_sanity();
	}

	void test_update() noexcept {
		for (Key_t index = 0; index < capacity; index++) {
			put_once(index, index);
		}

		for (Key_t index = 0; index < capacity; index++) {
			if (index % 2 == 0) {
				auto it = cache.find(index);
				assert(it != cache.end());
				cache.update(it);
			}
		}

		for (Key_t index = 0; index < capacity; index++) {
			if (index % 2 != 0) {
				auto it = cache.put(index);
				assert(it != cache.end());
				it->value = (index + 1);
			}
		}

		for (Key_t index = 0; index < capacity; index++) {
			if (index % 2 == 0) {
				find_once(index, index);
				find_once_const(index, index);
				remove_once(index, index);
			} else {
				find_once(index, index + 1);
				find_once_const(index, index + 1);
				remove_once(index, index + 1);
			}
		}
		assert(cache.size() == 0);
		test_sanity();
	}

	void test_clear() {
		for (size_t i = 0; i < capacity; i++) {
			put_once(i, i);
		}
		cache.clear();
		assert(cache.size() == 0);
		test_sanity();
	}

	void dump() {
		std::cout << "map has " << cache.m_map.size() << " elements \n";
		for (size_t bucket = 0; bucket < cache.m_map.buckets(); ++bucket) {
			std::cout << "B[" << bucket << "] ";
			for (auto it = cache.m_map.cbegin(bucket); it != cache.m_map.cend(); ++it) {
				std::cout << "[" << &(*it) << "] " << (*it).im_key << ":" << (*it).value.value << " -> ";
			}
			std::cout << "\n";
		}
		std::cout << "cached list has " << cache.m_list_cached.size() << " elements \n";
		for (auto it = cache.m_list_cached.cbegin(); it != cache.m_list_cached.cend(); ++it) {
			std::cout << (*it).value.value << ", ";
		}
		std::cout << "\n";
		std::cout << "freed list has " << cache.m_list_freed.size() << " elements \n";
		for (auto it = cache.m_list_freed.cbegin(); it != cache.m_list_freed.cend(); ++it) {
			std::cout << (*it).value.value << ", ";
		}
		std::cout << "\n";
	}

private:

	void put_once(const Key_t& key, const Value_t& value) noexcept {
		auto it = cache.find(key);
		assert(it == cache.end());
		it = cache.put(key);
		assert(it != cache.end());
		it->value = value;
		assert(it->im_key == key);
		assert(it->im_linked);
	}

	void update_once(const Key_t& key, const Value_t& value) noexcept {
		auto it = cache.find(key);
		assert(it != cache.end());
		it->value = value;
		assert(it->im_key == key);
		assert(it->im_linked);
		cache.update(it);
	}

	void find_once(const Key_t& key, const Value_t& value) noexcept {
		auto it = cache.find(key);
		assert(it != cache.end());
		assert(it->value == value);
		assert(it->im_key == key);
		assert(it->il_linked);
		assert(it->im_linked);
	}

	void find_once_const(const Key_t& key, const Value_t& value) const noexcept {
		auto it = cache.find(key);
		assert(it != cache.cend());
		assert(it->value == value);
		assert(it->im_key == key);
		assert(it->il_linked);
		assert(it->im_linked);
	}

	void miss_once(const Key_t& key) noexcept {
		auto it = cache.find(key);
		assert(it == cache.end());
	}

	void remove_once(const Key_t& key, const Value_t& value) noexcept {
		auto it = cache.find(key);
		assert(it != cache.end());
		assert(it->value == value);
		assert(it->im_key == key);
		assert(it->il_linked);
		assert(it->im_linked);
		cache.remove(it);
		assert(it->il_linked);
		assert(not it->im_linked);
		it = cache.find(key);
		assert(it == cache.end());
	}

	void test_sanity() {
		for (Key_t i = 0; i < capacity; i++) {
			assert(not cache.m_storage[i].im_linked);
			assert(cache.m_storage[i].il_linked);
		}
	}

	void fill() {
		for (Key_t i = 0; i < capacity; i++) {
			put_once(i, i);
		}
		assert(cache.size() == capacity);
	}

	void clear() {
		cache.clear();
		assert(cache.size() == 0);
		test_sanity();
	}

};

}; // namespace cache

#endif /* CACHE_TESTS_TEST_LRU_CACHE_H */

