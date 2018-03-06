#ifndef CACHE_LRU_CACHE_TEST_H
#define CACHE_LRU_CACHE_TEST_H

#include "LruCache.h"

#include <assert.h>
#include <iostream>

namespace cache {

class LruCacheTest {
	static constexpr unsigned KEY_VALUE_RATIO = 4;

	template <typename T>
	struct StructValue {
		T value;

		StructValue(): value(0) { };

		StructValue(T x): value(x) { };

		bool operator==(const StructValue& st_val) const {
			return value == st_val.value;
		}

		bool operator==(const T& val) const {
			return value == val;
		}
	};

	typedef unsigned Key_t;
	typedef StructValue<long long unsigned> Value_t;

	typedef LruCacheData<Key_t, Value_t> LruCacheData_t;
	typedef LruCache<LruCacheData_t> LruCache_t;

	LruCache_t cache;
	const size_t capacity;

public:

	LruCacheTest(unsigned capacity, float load_factor)
	: cache(capacity, load_factor), capacity(capacity) {
		if (not cache.allocate())
			throw std::logic_error("Cannot allocate LruCache instance");
	}

	LruCacheTest(const LruCacheTest&) = delete;
	LruCacheTest(LruCacheTest&&) = delete;

	LruCacheTest operator=(const LruCacheTest&) = delete;
	LruCacheTest operator=(LruCacheTest&&) = delete;

	~LruCacheTest() { }

	void test() {
		printf("<LruCacheTest>...\n");
		printf("sizeof(Data_t)=%zu\n", sizeof (LruCacheData_t));
		printf("sizeof(Bucket_t)=%zu\n", sizeof (LruCache_t::Bucket_t));
		printf("capacity=%zu\n", capacity);
		printf("buckets=%zu\n", cache.map.buckets());
		printf("memory used %zu Kb\n", cache.storage_bytes() / (1024));
		test_put();
		test_get();
		test_update();
		test_remove();
		test_cycle();
	}

	void test_put() {
		assert(cache.size() == 0);
		fill();
		compare();
		clear();
	}

	void test_get() {
		assert(cache.size() == 0);
		Key_t half = capacity / 2;
		for (Key_t i = 0; i < half; i++) {
			assert(cache.put(i, default_value(i)));
		}
		assert(cache.size() == half);
		Value_t value;
		for (Key_t i = 0; i < capacity; i++) {
			auto it = cache.find(i);
			if (i < half) {
				assert(it != cache.end());
				assert(*it == default_value(i));
			} else {
				assert(it == cache.end());
			}
		}
		clear();
	}

	void test_update() {
		assert(cache.size() == 0);
		Key_t max_key = ~static_cast<Key_t>(0);
		Value_t max_value(0xABCD);
		Value_t value;
		cache.put(max_key, max_value);
		for (Key_t i = 1; i < capacity * 2; i++) {
			auto it = cache.update(max_key);
			assert(cache.put(i, i));
			assert(it != cache.end());
			assert(*it == max_value);
		}
		auto it = cache.find(max_key);
		assert(it != cache.end());
		assert(*it == max_value);
		assert(cache.size() == capacity);
		clear();
	}

	void test_remove() {
		assert(cache.size() == 0);
		fill();
		Key_t half = capacity / 2;
		for (Key_t i = half; i < capacity; i++) {
			assert(cache.remove(i) != cache.end());
			assert(cache.remove(i) == cache.end());
		}
		for (Key_t i = 0; i < half; i++) {
			assert(cache.remove(i) != cache.end());
			assert(cache.remove(i) == cache.end());
		}
		assert(cache.size() == 0);
		test_sanity();
	}

	void test_cycle() {
		assert(cache.size() == 0);
		for (Key_t i = 0; i < capacity * 2; i++) {
			cache.put(i, i);
		}
		assert(cache.size() == capacity);
		Value_t value;
		for (Key_t i = 0; i < capacity * 2; i++) {
			auto it = cache.find(i);
			if (i < capacity) {
				assert(it == cache.end());
			} else {
				assert(it != cache.end());
				assert(*it == i);
			}
		}
		clear();
	}

	void dump() {
		std::cout << "map has " << cache.map.size() << " elements \n";
		for (size_t bucket = 0; bucket < cache.map.buckets(); ++bucket) {
			std::cout << "B[" << bucket << "] ";
			for (auto it = cache.map.cbegin(bucket); it != cache.map.cend(); ++it) {
				std::cout << (*it).value.value << " (" << (*it).im_key << "), ";
			}
			std::cout << "\n";
		}
		std::cout << "cached list has " << cache.list_cached.size() << " elements \n";
		for (auto it = cache.list_cached.cbegin(); it != cache.list_cached.cend(); ++it) {
			std::cout << (*it).value.value << ", ";
		}
		std::cout << "\n";
		std::cout << "freed list has " << cache.list_freed.size() << " elements \n";
		for (auto it = cache.list_freed.cbegin(); it != cache.list_freed.cend(); ++it) {
			std::cout << (*it).value.value << ", ";
		}
		std::cout << "\n";
	}

private:

	void test_sanity() {
		for (Key_t i = 0; i < capacity; i++) {
			assert(not cache.storage[i].im_linked);
			assert(cache.storage[i].il_linked);
		}
	}

	void fill() {
		for (Key_t i = 0; i < capacity; i++) {
			assert(cache.put(i, i));
			assert(not cache.put(i, default_value(i)));
		}
		assert(cache.size() == capacity);
	}

	void compare() {
		Value_t value;
		for (Key_t i = 0; i < capacity; i++) {
			auto it = cache.find(i);
			assert(it != cache.end());
			assert(*it == default_value(i));
		}
	}

	void clear() {
		cache.reset();
		assert(cache.size() == 0);
		test_sanity();
	}

	Value_t default_value(Key_t key) {
		Value_t value = key;
		return value.value * KEY_VALUE_RATIO;
	}

};

}; // namespace cache

#endif /* CACHE_LRU_CACHE_TEST_H */

