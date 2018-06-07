#ifndef CACHE_TIME_CACHE_TEST_H
#define CACHE_TIME_CACHE_TEST_H

#include <assert.h>
#include <cstdio>

#include "../TimerQueue.h"

namespace cache {

class TimerQueueTest {
	static constexpr unsigned TIMEOUT_SEC = 3;
	static constexpr unsigned KEY_VALUE_RATIO = 4;

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

	typedef TimerQueueData<Key_t, Value_t> TimerQueueData_t;
	typedef TimerQueue<TimerQueueData_t> TimerQueue_t;

	TimerQueue_t queue;
	const size_t capacity;

public:

	TimerQueueTest(unsigned capacity, float load_factor)
	: queue(capacity, load_factor), capacity(capacity) {
		if (not queue.allocate())
			throw std::logic_error("Cannot allocate TimerQueue instance");
	}

	TimerQueueTest(const TimerQueueTest&) = delete;
	TimerQueueTest(TimerQueueTest&&) = delete;

	TimerQueueTest operator=(const TimerQueueTest&) = delete;
	TimerQueueTest operator=(TimerQueueTest&&) = delete;

	~TimerQueueTest() { }

	void test() {
		printf("<TimerQueueTest>...\n");
		printf("sizeof(Data_t)=%zu\n", sizeof (TimerQueueData_t));
		printf("sizeof(Bucket_t)=%zu\n", sizeof (TimerQueue_t::Bucket_t));
		printf("capacity=%zu\n", capacity);
		printf("buckets=%zu\n", queue.map.buckets());
		printf("memory used %zu Kb\n", queue.storage_bytes() / (1024));
		test_put();
		test_get();
		test_remove();
		test_timeout();
	}

	void test_put() {
		assert(queue.size() == 0);
		fill();
		compare();
		clear();
	}

	void test_get() {
		assert(queue.size() == 0);
		Key_t half = capacity / 2;
		for (Key_t i = 0; i < half; i++) {
			assert(queue.push_back(i, default_value(i)));
		}
		assert(queue.size() == half);
		Value_t value;
		for (Key_t i = 0; i < capacity; i++) {
			auto it = queue.find(i);
			if (i < half) {
				assert(it != queue.end());
				assert(*it == default_value(i));
			} else {
				assert(it == queue.end());
			}
		}
		clear();
	}

	void test_remove() {
		assert(queue.size() == 0);
		fill();
		Key_t half = capacity / 2;
		for (Key_t i = half; i < capacity; i++) {
			assert(queue.remove(i) != queue.end());
			assert(queue.remove(i) == queue.end());
		}
		for (Key_t i = 0; i < half; i++) {
			assert(queue.remove(i) != queue.end());
			assert(queue.remove(i) == queue.end());
		}
		assert(queue.size() == 0);
		test_sanity();
	}

	void test_timeout() {
		assert(queue.size() == 0);

		// timeout all
		fill();
		Key_t key;
		Value_t value;
		for (Key_t i = 0; i < capacity; i++) {
			assert(queue.pop_front(key, value, -1 /*timeout all*/));
			assert(key == i);
			assert(value == default_value(i));
		}

		assert(queue.size() == 0);
		test_sanity();

		// test timeout
		Key_t key_exp = 1;
		Value_t value_exp = 1;
		std::time_t before = std::time(nullptr);
		assert(queue.push_back(key_exp, value_exp));
		assert(queue.size() == 1);
		while (not queue.pop_front(key, value, TIMEOUT_SEC/*timeout all*/));
		std::time_t after = std::time(nullptr);

		assert(after - before >= TIMEOUT_SEC);
		assert(key == key_exp);
		assert(value == value_exp);

		assert(queue.size() == 0);
		test_sanity();
	}

	void dump() {
		std::cout << "map has " << queue.map.size() << " elements \n";
		for (size_t bucket = 0; bucket < queue.map.buckets(); ++bucket) {
			std::cout << "B[" << bucket << "] ";
			for (auto it = queue.map.cbegin(bucket); it != queue.map.cend(); ++it) {
				std::cout << (*it).value.value << " (" << (*it).im_key << "), ";
			}
			std::cout << "\n";
		}
		std::cout << "cached list has " << queue.list_cached.size() << " elements \n";
		for (auto it = queue.list_cached.cbegin(); it != queue.list_cached.cend(); ++it) {
			std::cout << (*it).value.value << " ";
		}
		std::cout << "\n";
		std::cout << "freed list has " << queue.list_freed.size() << " elements \n";
		for (auto it = queue.list_freed.cbegin(); it != queue.list_freed.cend(); ++it) {
			std::cout << (*it).value.value << ", ";
		}
		std::cout << "\n";
	}

private:

	void test_sanity() {
		for (Key_t i = 0; i < capacity; i++) {
			assert(not queue.storage[i].im_linked);
			assert(queue.storage[i].il_linked);
		}
	}

	void fill() {
		for (Key_t i = 0; i < capacity; i++) {
			assert(queue.push_back(i, i));
			assert(queue.push_back(i, default_value(i)));
		}
		assert(queue.size() == capacity);
	}

	void compare() {
		Value_t value;
		for (Key_t i = 0; i < capacity; i++) {
			auto it = queue.find(i);
			assert(it != queue.end());
			assert(*it == default_value(i));
		}
	}

	void clear() {
		queue.reset();
		assert(queue.size() == 0);
		test_sanity();
	}

	Value_t default_value(Key_t key) {
		Value_t value = key;
		return value.value * KEY_VALUE_RATIO;
	}
};

}; // namespace cache

#endif /* CACHE_TIME_CACHE_TEST_H */

