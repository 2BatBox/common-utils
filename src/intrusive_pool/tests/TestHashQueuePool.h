#ifndef INTRUSIVEPOOL_TESTS_TESTHASHQUEUEPOOL_H
#define INTRUSIVEPOOL_TESTS_TESTHASHQUEUEPOOL_H

#include "../HashQueuePool.h"

#include <assert.h>
#include <iostream>

namespace intrusive {

class TestHashQueuePool {

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

	using Node_t = HashQueuePoolNode<Key_t, Value_t>;
	using Pool_t = HashQueuePool<Node_t>;

	Pool_t m_pool;
	const size_t m_capacity;

public:

	TestHashQueuePool(unsigned capacity, float load_factor) noexcept
	: m_pool(capacity, load_factor), m_capacity(capacity) {
		assert(m_pool.allocate() == 0);
	}

	TestHashQueuePool(const TestHashQueuePool&) = delete;
	TestHashQueuePool(TestHashQueuePool&&) = delete;

	TestHashQueuePool operator=(const TestHashQueuePool&) = delete;
	TestHashQueuePool operator=(TestHashQueuePool&&) = delete;

	~TestHashQueuePool() { }

	void test() noexcept {
		float mem_used = m_pool.m_map.buckets() * sizeof (Pool_t::Bucket_t);
		mem_used += m_pool.capacity() * sizeof (Node_t);
		printf("<TestHashQueuePool>...\n");
		printf("sizeof(Node_t)=%zu\n", sizeof (Node_t));
		printf("sizeof(Pool_t::Bucket_t)=%zu\n", sizeof (Pool_t::Bucket_t));
		printf("capacity=%zu\n", m_capacity);
		printf("buckets=%zu\n", m_pool.m_map.buckets());
		printf("memory used %.2f Kb\n", mem_used / 1024);

		unsigned step = 1;
		test_push_pop(step++);
		test_push_pop_same_key(step++);
		test_push_remove(step++);
		test_push_remove_same_key(step++);

		test_clear(step++);
	}

	void test_push_pop(unsigned step) {
		printf("-> test_push_pop()\n");
		assert(m_pool.size() == 0);

		// test with one item
		for (size_t i = 0; i < m_capacity * 2; i++) {
			push_back(i * step, i + step);
			find_one(i * step, i + step);
			peek_front_one(i * step, i + step);
			pop_front_one(i * step, i + step);
			miss_one(i * step);
		}

		assert(m_pool.size() == 0);
		test_sanity();

		// test with full capacity
		for (size_t i = 0; i < m_capacity * 2; i++) {
			if (i < m_capacity) {
				push_back(i * step, i + step);
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

	void test_push_pop_same_key(unsigned step) {
		printf("-> test_push_pop_same_key()\n");
		assert(m_pool.size() == 0);

		// test with one item
		for (size_t i = 0; i < m_capacity * 2; i++) {
			push_back(step, i + step);
			find_one(step, i + step);
			peek_front_one(step, i + step);
			pop_front_one(step, i + step);
			miss_one(step);
		}

		assert(m_pool.size() == 0);
		test_sanity();

		// test with full capacity
		for (size_t i = 0; i < m_capacity * 2; i++) {
			if (i < m_capacity) {
				push_back(step, i + step);
			} else {
				push_back_oversize(step);
			}
		}

		for (size_t i = 0; i < m_capacity; i++) {
			find_multi(step, i + step);
			peek_front_one(step, i + step);
			pop_front_one(step, i + step);
		}

		miss_one(step);

		assert(m_pool.size() == 0);
		test_sanity();
	}

	void test_push_remove(unsigned step) {
		printf("-> test_push_remove()\n");
		assert(m_pool.size() == 0);

		// test with one item
		for (size_t i = 0; i < m_capacity * 2; i++) {
			push_back(i * step, i + step);
			find_one(i * step, i + step);
			peek_front_one(i * step, i + step);
			remove_one(i * step, i + step);
			miss_one(i * step);
		}

		assert(m_pool.size() == 0);
		test_sanity();

		// test with full capacity
		for (size_t i = 0; i < m_capacity * 2; i++) {
			if (i < m_capacity) {
				push_back(i * step, i + step);
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

	void test_push_remove_same_key(unsigned step) {
		printf("-> test_push_remove_same_key()\n");
		assert(m_pool.size() == 0);

		// test with one item
		for (size_t i = 0; i < m_capacity * 2; i++) {
			push_back(step, i + step);
			find_one(step, i + step);
			peek_front_one(step, i + step);
			remove_one(step, i + step);
			miss_one(step);
		}

		assert(m_pool.size() == 0);
		test_sanity();

		// test with full capacity
		for (size_t i = 0; i < m_capacity * 2; i++) {
			if (i < m_capacity) {
				push_back(step, i + step);
			} else {
				push_back_oversize(i * step);
			}
		}

		for (size_t i = 0; i < m_capacity; i++) {
			find_multi(step, i + step);
			peek_front_one(step, i + step);
			remove_multi(step, i + step);
		}

		miss_one(step);

		assert(m_pool.size() == 0);
		test_sanity();
	}

	void test_clear(unsigned step) {
		printf("-> test_clear()\n");
		for (size_t i = 0; i < m_capacity; i++) {
			push_back(i * step, i + step);
		}
		m_pool.reset();

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

	void push_back(const Key_t& key, const Value_t& value) noexcept {
		auto it = m_pool.push_back(key);
		assert(it != m_pool.end());
		it->value = value;
		assert(it->im_key == key);
		assert(it->im_linked);
	}

	void push_back_oversize(const Key_t& key) noexcept {
		auto it = m_pool.push_back(key);
		assert(it == m_pool.end());
	}

	void peek_front_one(const Key_t& key, const Value_t& value) noexcept {
		auto it = m_pool.peek_front();
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

	void find_multi(const Key_t& key, const Value_t& value) noexcept {
		auto it = m_pool.find(key);
		assert(it != m_pool.end());
		for (; it != m_pool.end(); it.next(key)) {
			assert(it->im_key == key);
			assert(it->im_linked);
			if (it->value == value) {
				assert(it->il_linked);
				assert(it->im_linked);
				return;
			}
		}
		assert(false);

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

	void remove_multi(const Key_t& key, const Value_t& value) noexcept {
		auto it = m_pool.find(key);
		assert(it != m_pool.end());
		for (; it != m_pool.end(); it.next(key)) {
			assert(it->im_key == key);
			assert(it->im_linked);
			if (it->value == value) {
				assert(it->il_linked);
				assert(it->im_linked);
				m_pool.remove(it);
				assert(it->il_linked);
				assert(not it->im_linked);
				return;
			}
		}
		assert(false);
	}

	void test_sanity() {
		for (Key_t i = 0; i < m_capacity; i++) {
			assert(not m_pool.m_storage[i].im_linked);
			assert(m_pool.m_storage[i].il_linked);
		}
	}

};

}; // namespace intrusive

#endif /* INTRUSIVEPOOL_TESTS_TESTHASHQUEUEPOOL_H */

