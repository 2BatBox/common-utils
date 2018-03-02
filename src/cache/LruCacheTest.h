#ifndef CACHE_LRU_CACHE_TEST_H
#define CACHE_LRU_CACHE_TEST_H

#include "LruCache.h"

#include <assert.h>
#include <iostream>

namespace cache {

class LruCacheTest {

	template <typename T>
	struct StructValue {
		T value;

		StructValue(): value(0) { };

		StructValue(T x): value(x) { };

		bool operator==(const StructValue& st_val) const {
			return value == st_val.value;
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
		test_get_refresh();
		test_remove();
		test_cycle();
		test_update_value();
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
			assert(cache.put(i, i));
		}
		assert(cache.size() == half);
		Value_t value;
		for (Key_t i = 0; i < capacity; i++) {
			if (i < half) {
				assert(cache.get(i, value));
				assert(value == i);
			} else {
				assert(not cache.get(i, value));
			}
		}
		clear();
	}

	void test_get_refresh() {
		assert(cache.size() == 0);
		Key_t max_key = ~static_cast<Key_t>(0);
		Value_t value;
		cache.put(max_key, value);
		for (Key_t i = 1; i < capacity * 2; i++) {
			assert(cache.put(i, i));
			assert(cache.get_refresh(max_key, value));
			assert(value.value = 0xbcde);
		}
		assert(cache.size() == capacity);
		clear();
	}

	void test_remove() {
		assert(cache.size() == 0);
		fill();
		Key_t half = capacity / 2;
		for (Key_t i = half; i < capacity; i++) {
			assert(cache.remove(i));
			assert(not cache.remove(i));
		}
		for (Key_t i = 0; i < half; i++) {
			assert(cache.remove(i));
			assert(not cache.remove(i));
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
			if (i < capacity) {
				assert(not cache.get(i, value));
			} else {
				assert(cache.get(i, value));
				assert(value == i);
			}
		}
		clear();
	}

	void test_update_value() {
		assert(cache.size() == 0);
		for (Key_t i = 0; i < capacity; i++) {
			cache.put(i, i);
			cache.put(i, i * 2);
		}
		assert(cache.size() == capacity);
		Value_t value;
		for (Key_t i = 0; i < capacity; i++) {
			assert(cache.get(i, value));
			assert(value == i * 2);
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
			assert(not cache.put(i, i));
		}
		assert(cache.size() == capacity);
	}

	void compare() {
		Value_t value;
		for (Key_t i = 0; i < capacity; i++) {
			assert(cache.get(i, value));
			assert(value == i);
		}
	}

	void clear() {
		cache.reset();
		assert(cache.size() == 0);
		test_sanity();
	}

};

}; // namespace cache

#endif /* CACHE_LRU_CACHE_TEST_H */

