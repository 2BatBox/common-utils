#ifndef STORAGE_TESTS_TESTTIMEDQUEUE_H
#define STORAGE_TESTS_TESTTIMEDQUEUE_H

#include "../TimedQueue.h"

#include <assert.h>
#include <iostream>
#include <thread>

namespace storage {

class TestTimedQueue {

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

	using Node_t = TimedQueueNode<Key_t, Value_t>;
	using Queue_t = TimedQueue<Node_t>;

	Queue_t m_queue;
	const size_t m_capacity;

public:

	TestTimedQueue(unsigned capacity, float load_factor) noexcept
	: m_queue(capacity, load_factor), m_capacity(capacity) {
		assert(m_queue.allocate() == 0);
	}

	TestTimedQueue(const TestTimedQueue&) = delete;
	TestTimedQueue(TestTimedQueue&&) = delete;

	TestTimedQueue operator=(const TestTimedQueue&) = delete;
	TestTimedQueue operator=(TestTimedQueue&&) = delete;

	~TestTimedQueue() { }

	void test() noexcept {
		printf("<TestTimedHashQueue>...\n");
		printf("sizeof(Node_t)=%zu\n", sizeof (Node_t));
		printf("capacity=%zu\n", m_capacity);

		unsigned step = 1;
		test_push_pop(step++);
		test_push_pop_same_key(step++);
		test_push_remove(step++);
		test_push_remove_same_key(step++);
		test_push_remove_all(step++);
		test_push_pop_timeout(step++);

		test_clear(step++);
	}

	void test_push_pop(unsigned step) {
		printf("-> test_push_pop()\n");
		assert(m_queue.size() == 0);

		// test with one item
		for (size_t i = 0; i < m_capacity * 2; i++) {
			push_back(i * step, i + step);
			pop_front_one(i * step, i + step, 0);
		}

		assert(m_queue.size() == 0);

		// test with full capacity
		for (size_t i = 0; i < m_capacity * 2; i++) {
			if (i < m_capacity) {
				push_back(i * step, i + step);
			} else {
				push_back_oversize(i * step);
			}
		}

		for (size_t i = 0; i < m_capacity; i++) {
			pop_front_one(i * step, i + step, 0);
		}

		assert(m_queue.size() == 0);
	}

	void test_push_pop_same_key(unsigned step) {
		printf("-> test_push_pop_same_key()\n");
		assert(m_queue.size() == 0);

		// test with one item
		for (size_t i = 0; i < m_capacity * 2; i++) {
			push_back(step, i + step);
			pop_front_one(step, i + step, 0);
		}

		assert(m_queue.size() == 0);

		// test with full capacity
		for (size_t i = 0; i < m_capacity * 2; i++) {
			if (i < m_capacity) {
				push_back(step, i + step);
			} else {
				push_back_oversize(step);
			}
		}

		for (size_t i = 0; i < m_capacity; i++) {
			pop_front_one(step, i + step, 0);
		}

		assert(m_queue.size() == 0);
	}

	void test_push_remove(unsigned step) {
		printf("-> test_push_remove()\n");
		assert(m_queue.size() == 0);

		// test with one item
		for (size_t i = 0; i < m_capacity * 2; i++) {
			push_back(i * step, i + step);
			remove_one(i * step, i + step);
		}

		assert(m_queue.size() == 0);

		// test with full capacity
		for (size_t i = 0; i < m_capacity * 2; i++) {
			if (i < m_capacity) {
				push_back(i * step, i + step);
			} else {
				push_back_oversize(i * step);
			}
		}

		for (size_t i = 0; i < m_capacity; i++) {
			remove_one(i * step, i + step);
		}

		assert(m_queue.size() == 0);
	}

	void test_push_remove_same_key(unsigned step) {
		printf("-> test_push_remove_same_key()\n");
		assert(m_queue.size() == 0);

		// test with one item
		for (size_t i = 0; i < m_capacity * 2; i++) {
			push_back(step, i + step);
			remove_one(step, i + step);
		}

		assert(m_queue.size() == 0);

		// test with full capacity
		for (size_t i = 0; i < m_capacity * 2; i++) {
			if (i < m_capacity) {
				push_back(step, i + step);
			} else {
				push_back_oversize(i * step);
			}
		}

		for (size_t i = 0; i < m_capacity; i++) {
			remove_one(step, i + step);
		}

		assert(m_queue.size() == 0);
	}

	void test_push_remove_all(unsigned step) {
		printf("-> test_push_remove_all()\n");
		assert(m_queue.size() == 0);

		// test with full capacity
		for (size_t i = 0; i < m_capacity; i++) {
			if (i % 2 == 0) {
				push_back(i * step, i + step);
			} else {
				push_back(m_capacity, i + step);
			}
		}

		m_queue.remove_all(m_capacity);

		for (size_t i = 0; i < m_capacity; i++) {
			if (i % 2 == 0) {
				remove_one(i * step, i + step);
			}
		}

		//        assert(m_queue.size() == 0);
	}

	void test_push_pop_timeout(unsigned step) {
		printf("-> test_push_pop_timeout()\n");
		assert(m_queue.size() == 0);

		const size_t time_slots = std::min(m_capacity, size_t(5));
		unsigned time_sec = 1;

		std::time_t before;
		std::time_t after;

		// test with one item
		for (size_t i = 0; i < time_slots; i++) {
			before = std::time(nullptr);
			push_back(i * step, i + step);
			printf("push_back(%zu)\n", i);

			pop_front_one(i * step, i + step, time_sec);
			printf("pop_front_one(%zu)\n", i);
			after = std::time(nullptr);
			assert(after - before >= time_sec);
		}

		assert(m_queue.size() == 0);

		printf("test with full capacity\n");
		// test with full capacity
		for (size_t i = 0; i < time_slots; i++) {
			push_back(i * step, i + step);
			printf("push_back(%zu)\n", i);
			std::this_thread::sleep_for(std::chrono::seconds(time_sec));
		}

		for (size_t i = 0; i < time_slots; i++) {
			before = std::time(nullptr);
			pop_front_one(i * step, i + step, time_slots * time_sec + time_sec);
			printf("pop_front_one(%zu)\n", i);
			after = std::time(nullptr);
			assert(after - before >= time_sec);
		}

		assert(m_queue.size() == 0);
	}

	void test_clear(unsigned step) {
		printf("-> test_clear()\n");

		assert(m_queue.size() == 0);
		m_queue.reset();

		assert(m_queue.size() == 0);
		for (size_t i = 0; i < m_capacity; i++) {
			push_back(i * step, i + step);
		}
		m_queue.reset();
		assert(m_queue.size() == 0);
	}

private:

	void push_back(const Key_t& key, const Value_t& value) noexcept {
		auto it = m_queue.push_back(key);
		assert(it != m_queue.end());
		it->value = value;
		assert(it->im_key == key);
	}

	void push_back_oversize(const Key_t& key) noexcept {
		auto it = m_queue.push_back(key);
		assert(it == m_queue.end());
	}

	void pop_front_one(const Key_t& key, const Value_t& value, unsigned timeout) noexcept {
		Queue_t::Iterator_t it;
		while (not (it = m_queue.pop_front(timeout)));
		assert(it != m_queue.end());
		it->value = value;
		assert(it->im_key == key);
	}

	void remove_one(const Key_t& key, const Value_t& value) noexcept {
		auto it = m_queue.remove(key);
		assert(it != m_queue.end());
		it->value = value;
		assert(it->im_key == key);
	}

};

}; // namespace storage

#endif /* STORAGE_TESTS_TESTTIMEDQUEUE_H */

