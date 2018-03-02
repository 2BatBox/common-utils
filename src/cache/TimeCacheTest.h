#ifndef CACHE_TIME_CACHE_TEST_H
#define CACHE_TIME_CACHE_TEST_H

#include <assert.h>
#include <cstdio>

#include "TimeCache.h"

namespace cache {

class TimeCacheTest {
	static constexpr unsigned TIMEOUT_SEC = 3;

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

	typedef TimeCacheData<Key_t, Value_t> Data_t;
	typedef TimeCache<Data_t> TimeCache_t;

	TimeCache_t cache;
	const size_t capacity;

public:

	TimeCacheTest(unsigned capacity, float load_factor)
	: cache(capacity, load_factor), capacity(capacity) {
		if (not cache.allocate())
			throw std::logic_error("Cannot allocate TimeCache instance");
	}

	TimeCacheTest(const TimeCacheTest&) = delete;
	TimeCacheTest(TimeCacheTest&&) = delete;

	TimeCacheTest operator=(const TimeCacheTest&) = delete;
	TimeCacheTest operator=(TimeCacheTest&&) = delete;

	~TimeCacheTest() { }

	void test() {
		printf("<TimeCacheTest>...\n");
		printf("sizeof(Data_t)=%zu\n", sizeof (Data_t));
		printf("sizeof(Bucket_t)=%zu\n", sizeof (TimeCache_t::Bucket_t));
		printf("capacity=%zu\n", capacity);
		printf("buckets=%zu\n", cache.map.buckets());
		printf("memory used %zu Kb\n", cache.storage_bytes() / (1024));
		test_put();
		test_get();
		test_get_refresh();
		test_remove();
		test_update_value();
        test_timeout();
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
		for (Key_t i = 1; i < capacity; i++) {
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

	void test_update_value() {
		assert(cache.size() == 0);
		fill();
		compare();
		clear();
	}

	void test_timeout() {
		assert(cache.size() == 0);

		Key_t* key_buffer = new Key_t[capacity];
		Value_t* value_buffer = new Value_t[capacity];
		std::unique_ptr<Key_t> key_buf_ptr(key_buffer);
		std::unique_ptr<Value_t> value_buf_ptr(value_buffer);

		size_t result = 0;

		// timeout all
		fill();
		result = cache.timeout(key_buffer, value_buffer, capacity, 0);
		assert(result == capacity);
		assert(cache.size() == 0);
		test_sanity();

		// timeout all part by part
		fill();
		size_t part_first = capacity / 2;
		size_t part_second = capacity - part_first;
		result = cache.timeout(key_buffer, value_buffer, part_first, 0);
		assert(result == part_first);
		assert(cache.size() == capacity - part_first);

		result = cache.timeout(key_buffer, value_buffer, part_second, 0);
		assert(result == part_second);
		assert(cache.size() == 0);
		test_sanity();

		// test sequences of results
		fill();
		Key_t key = 0;
		Value_t value = 0;
		Key_t key_exp = 0;
		Value_t value_exp = 0;
		while (cache.size()) {
			result = cache.timeout(&key, &value, 1, 0);
			if (result) {
				assert(result == 1);
				assert(key == key_exp && value == value_exp);
				key_exp++;
				value_exp.value = key_exp * 2;
			}
		}
		result = cache.timeout(&key, &value, 1, 0);
		assert(result == 0);
		
		// test timeout
		key_exp = 1;
		value_exp = 1;
		std::time_t before = std::time(nullptr);
		assert(cache.put(key_exp, value_exp));
		assert(cache.size() == 1);
		while((result = cache.timeout(&key, &value, 1, TIMEOUT_SEC)) == 0);
		std::time_t after = std::time(nullptr);
		
		assert(result = 1);
		assert(after - before >= TIMEOUT_SEC);
		assert(key == key_exp);
		assert(value == value_exp);

		assert(cache.size() == 0);
		test_sanity();
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
			std::cout << (*it).value.value << " ";
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
			assert(cache.put(i, i * 2));
		}
		assert(not cache.put(capacity, 0));
		assert(cache.size() == capacity);
	}

	void compare() {
		Value_t value;
		for (Key_t i = 0; i < capacity; i++) {
			assert(cache.get(i, value));
			assert(value == i * 2);
		}
	}

	void clear() {
		cache.reset();
		assert(cache.size() == 0);
		test_sanity();
	}


};

}; // namespace cache

#endif /* CACHE_TIME_CACHE_TEST_H */

