#ifndef INTRUSIVEPOOL_TESTS_TESTDEQUEPOOL_h
#define INTRUSIVEPOOL_TESTS_TESTDEQUEPOOL_h

#include "../DequePool.h"

#include <assert.h>
#include <iostream>

namespace intrusive {

class TestDequePool {

	template<typename T>
	struct StructValue {
		T value;

		StructValue() : value(0) {};

		StructValue(T x) : value(x) {};

		bool operator==(const StructValue& st_val) const {
			return value == st_val.value;
		}

		bool operator==(const T& val) const {
			return value == val;
		}
	};

	using Value_t = StructValue<long long unsigned>;

	using Node_t = DequePoolNode<Value_t>;
	using LinkedPool_t = DequePool<Node_t>;

	LinkedPool_t m_pool;
	const size_t m_capacity;

public:

	TestDequePool(unsigned capacity) noexcept
		: m_pool(capacity), m_capacity(capacity) {
		assert(m_pool.allocate() == 0);
	}

	TestDequePool(const TestDequePool&) = delete;
	TestDequePool(TestDequePool&&) = delete;

	TestDequePool operator=(const TestDequePool&) = delete;
	TestDequePool operator=(TestDequePool&&) = delete;

	~TestDequePool() {}

	void test() noexcept {
		float mem_used = m_pool.capacity() * sizeof(Node_t);
		printf("<TestDequePool>...\n");
		printf("sizeof(Node_t)=%zu\n", sizeof(Node_t));
		printf("capacity=%zu\n", m_capacity);
		printf("memory used %.2f Kb\n", mem_used / 1024);

		unsigned step = 1;
		test_push_back_pop_front(step++);

		test_push_front_pop_back(step++);
		test_push_back_pop_back(step++);
		test_push_front_pop_front(step++);
		test_push_remove(step++);
		test_reset(step++);
		test_iterators(step++);
	}

	void test_push_back_pop_front(unsigned step) {
		printf("-> test_push_back_pop_front()\n");
		assert(m_pool.size() == 0);

		// test with one item
		for(size_t i = 0; i < m_capacity; i++) {
			push_back_one(i * step);
			peek_front_one(i * step);
			pop_front_one(i * step);
		}

		assert(m_pool.size() == 0);
		test_sanity();

		// test with full capacity
		for(size_t i = 0; i < m_capacity; i++) {
			push_back_one(i * step);
		}
		oversize_one();

		for(size_t i = 0; i < m_capacity; i++) {
			peek_front_one(i * step);
			pop_front_one(i * step);
		}

		assert(m_pool.size() == 0);
		test_sanity();
	}

	void test_push_front_pop_back(unsigned step) {
		printf("-> test_push_front_pop_back()\n");
		assert(m_pool.size() == 0);

		// test with one item
		for(size_t i = 0; i < m_capacity; i++) {
			push_front_one(i * step);
			peek_back_one(i * step);
			pop_back_one(i * step);
		}

		assert(m_pool.size() == 0);
		test_sanity();

		// test with full capacity
		for(size_t i = 0; i < m_capacity; i++) {
			push_front_one(i * step);
		}
		oversize_one();

		for(size_t i = 0; i < m_capacity; i++) {
			peek_back_one(i * step);
			pop_back_one(i * step);
		}

		assert(m_pool.size() == 0);
		test_sanity();
	}

	void test_push_back_pop_back(unsigned step) {
		printf("-> test_push_back_pop_back()\n");
		assert(m_pool.size() == 0);

		// test with one item
		for(size_t i = 0; i < m_capacity; i++) {
			push_back_one(i * step);
			peek_back_one(i * step);
			pop_back_one(i * step);
		}

		assert(m_pool.size() == 0);
		test_sanity();

		// test with full capacity
		for(size_t i = 0; i < m_capacity; i++) {
			push_back_one(i * step);
		}
		oversize_one();

		for(int i = m_capacity - 1; i >= 0; i--) {
			peek_back_one(i * step);
			pop_back_one(i * step);
		}

		assert(m_pool.size() == 0);
		test_sanity();
	}

	void test_push_front_pop_front(unsigned step) {
		printf("-> test_push_front_pop_front()\n");
		assert(m_pool.size() == 0);

		// test with one item
		for(size_t i = 0; i < m_capacity; i++) {
			push_front_one(i * step);
			peek_front_one(i * step);
			pop_front_one(i * step);
		}

		assert(m_pool.size() == 0);
		test_sanity();

		// test with full capacity
		for(size_t i = 0; i < m_capacity; i++) {
			push_front_one(i * step);
		}
		oversize_one();

		for(int i = m_capacity - 1; i >= 0; i--) {
			peek_front_one(i * step);
			pop_front_one(i * step);
		}

		assert(m_pool.size() == 0);
		test_sanity();
	}

	void test_push_remove(unsigned step) {
		printf("-> test_push_back_remove()\n");
		assert(m_pool.size() == 0);

		// test with one item
		for(size_t i = 0; i < m_capacity; i++) {
			push_back_one(i * step);
			remove_front_one(i * step);
			push_back_one(i * step);
			remove_back_one(i * step);
		}

		assert(m_pool.size() == 0);
		test_sanity();

		// test with full capacity
		for(size_t i = 0; i < m_capacity; i++) {
			push_back_one(i * step);
		}
		oversize_one();

		for(size_t i = 0; i < m_capacity; i++) {
			remove_front_one(i * step);
		}

		for(size_t i = 0; i < m_capacity; i++) {
			push_front_one(i * step);
		}
		oversize_one();

		for(size_t i = 0; i < m_capacity; i++) {
			remove_back_one(i * step);
		}

		assert(m_pool.size() == 0);
		test_sanity();
	}

	void test_reset(unsigned step) {
		printf("-> test_reset()\n");
		for(size_t i = 0; i < m_capacity; i++) {
			push_back_one(i * step);
		}
		m_pool.reset();

		assert(m_pool.size() == 0);
		test_sanity();
	}

	void test_iterators(unsigned step) {
		printf("-> test_iterators()\n");
		assert(m_pool.size() == 0);

		for(size_t i = 0; i < m_capacity; i++) {
			push_back_one(i * step);
		}
		oversize_one();

		size_t index = 0;
		for(auto& elem : m_pool) {
			assert(elem.value == index * step);
			index++;
		}

		index = 0;
		for(auto it = m_pool.begin(); it != m_pool.end(); ++it) {
			assert(it->value == index * step);
			index++;
		}

		index = 0;
		for(auto it = m_pool.begin(); it != m_pool.end(); it++) {
			assert(it->value == index * step);
			index++;
		}

		index = 0;
		for(auto it = m_pool.cbegin(); it != m_pool.cend(); ++it) {
			assert(it->value == index * step);
			index++;
		}

		index = 0;
		for(auto it = m_pool.cbegin(); it != m_pool.cend(); it++) {
			assert(it->value == index * step);
			index++;
		}

		index = m_capacity - 1;
		for(auto it = m_pool.rbegin(); it != m_pool.rend(); ++it) {
			assert(it->value == index * step);
			index--;
		}

		index = m_capacity - 1;
		for(auto it = m_pool.rbegin(); it != m_pool.rend(); it++) {
			assert(it->value == index * step);
			index--;
		}

		index = m_capacity - 1;
		for(auto it = m_pool.crbegin(); it != m_pool.crend(); ++it) {
			assert(it->value == index * step);
			index--;
		}

		index = m_capacity - 1;
		for(auto it = m_pool.crbegin(); it != m_pool.crend(); it++) {
			assert(it->value == index * step);
			index--;
		}

		for(size_t i = 0; i < m_capacity; i++) {
			peek_front_one(i * step);
			pop_front_one(i * step);
		}

		assert(m_pool.size() == 0);
		test_sanity();
	}

	void dump() {
		std::cout << "cached list has " << m_pool.m_list_cached.size() << " elements \n";
		for(auto it = m_pool.m_list_cached.cbegin(); it != m_pool.m_list_cached.cend(); ++it) {
			std::cout << (*it).value.value << ", ";
		}
		std::cout << "\n";
		std::cout << "freed list has " << m_pool.m_list_freed.size() << " elements \n";
		for(auto it = m_pool.m_list_freed.cbegin(); it != m_pool.m_list_freed.cend(); ++it) {
			std::cout << (*it).value.value << ", ";
		}
		std::cout << "\n";
	}

private:

	void push_back_one(const Value_t& value) noexcept {
		auto it = m_pool.push_back();
		assert(it != m_pool.end());
		it->value = value;
		assert(it->il_linked);
	}

	void push_front_one(const Value_t& value) noexcept {
		auto it = m_pool.push_front();
		assert(it != m_pool.end());
		it->value = value;
		assert(it->il_linked);
	}

	void peek_front_one(const Value_t& value) noexcept {
		auto it = m_pool.begin();
		assert(it != m_pool.end());
		it->value = value;
		assert(it->il_linked);
	}

	void peek_back_one(const Value_t& value) noexcept {
		auto it = m_pool.rbegin();
		assert(it != m_pool.rend());
		it->value = value;
		assert(it->il_linked);
	}

	void pop_front_one(const Value_t& value) noexcept {
		auto it = m_pool.pop_front();
		assert(it != m_pool.end());
		it->value = value;
		assert(it->il_linked);
	}

	void pop_back_one(const Value_t& value) noexcept {
		auto it = m_pool.pop_back();
		assert(it != m_pool.end());
		it->value = value;
		assert(it->il_linked);
	}

	void remove_front_one(const Value_t& value) noexcept {
		auto it = m_pool.begin();
		assert(it != m_pool.end());
		it->value = value;
		assert(it->il_linked);
		m_pool.remove(it);
		assert(it->il_linked);
	}

	void remove_back_one(const Value_t& value) noexcept {
		auto it = m_pool.rbegin();
		assert(it != m_pool.rend());
		it->value = value;
		assert(it->il_linked);
		m_pool.remove(it);
		assert(it->il_linked);
	}

	void oversize_one() noexcept {
		auto it = m_pool.push_front();
		assert(it == m_pool.end());
		it = m_pool.push_back();
		assert(it == m_pool.end());
	}

	void test_sanity() {
		for(unsigned i = 0; i < m_capacity; i++) {
			assert(m_pool.m_storage[i].il_linked);
		}
	}

};

}; // namespace intrusive

#endif /* INTRUSIVEPOOL_TESTS_TESTDEQUEPOOL_h */

