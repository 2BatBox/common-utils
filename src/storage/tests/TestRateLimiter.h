#ifndef STORAGE_TESTS_TESTRATELIMITER_H
#define STORAGE_TESTS_TESTRATELIMITER_H

#include "../RateLimiter.h"

#include <assert.h>
#include <iostream>
#include <thread>

namespace storage {

class TestRateLimiter {

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

	using Key_t = unsigned;

	using Node_t = RateLimiterNode<Key_t>;
	using RateLimiter_t = RateLimiter<Node_t>;

	RateLimiter_t m_limiter;
	const size_t m_capacity;

public:

	TestRateLimiter(unsigned capacity, float load_factor) noexcept
		: m_limiter(capacity, load_factor)
		, m_capacity(capacity) {
		assert(m_limiter.allocate() == 0);
	}

	TestRateLimiter(const TestRateLimiter&) = delete;
	TestRateLimiter(TestRateLimiter&&) = delete;

	TestRateLimiter operator=(const TestRateLimiter&) = delete;
	TestRateLimiter operator=(TestRateLimiter&&) = delete;

	~TestRateLimiter() {}

	void test() noexcept {
		printf("<TestRateLimiter>...\n");
		printf("capacity_addr=%zu\n", m_capacity);
		printf("sizeof(NodeAddr_t)=%zu\n", sizeof(Node_t));
		printf("storage_bytes=%.2f Kb\n", m_limiter.storage_bytes() / (float) 1024.0);
		m_limiter.set_period(rte_get_tsc_hz() * 10/*10 sec*/);

		unsigned step = 1;
		test_check(step++);
		test_check_cycles(step++);
		test_remove(step++);
		test_clear(step++);
	}

	void test_check(unsigned step) noexcept {
		m_limiter.set_period(rte_get_tsc_hz() * 10/*10 sec*/);
		printf("-> test_check_addr(step=%u)\n", step);
		assert(m_limiter.size() == 0);

		// test with one item
		size_t init = 0;
		assert(m_limiter.check(init));
		for(size_t i = init; i < m_capacity * 2; i++) {
			assert(not m_limiter.check(init));
		}
		m_limiter.reset();
		assert(m_limiter.size() == 0);

		// test with full capacity_addr
		for(size_t i = 0; i < m_capacity; i++) {
			assert(m_limiter.check(i));
		}
		m_limiter.reset();
		assert(m_limiter.size() == 0);

		// test with double capacity_addr
		for(size_t i = 0; i < m_capacity * 2; i++) {
			assert(m_limiter.check(i));
			assert(not m_limiter.check(i));
		}
		m_limiter.reset();
		assert(m_limiter.size() == 0);
	}

	void test_check_cycles(unsigned step) noexcept {
		printf("-> test_check_cycles(step=%u)\n", step);
		m_limiter.set_period(rte_get_tsc_hz() / 10 /*100 msec*/);
		assert(m_limiter.size() == 0);

		for(size_t i = 0; i < m_capacity; i++) {
			assert(m_limiter.check(i));
			assert(not m_limiter.check(i));
		}

		wait_cycles(rte_get_tsc_hz() / 10);
		for(size_t i = 0; i < m_capacity; i++) {
			assert(m_limiter.check(i));
			assert(not m_limiter.check(i));
		}

		m_limiter.reset();
		assert(m_limiter.size() == 0);
	}

	void test_remove(unsigned step) noexcept {
		m_limiter.set_period(rte_get_tsc_hz() * 10/*10 sec*/);
		printf("-> test_remove(step=%u)\n", step);
		assert(m_limiter.size() == 0);

		for(size_t i = 0; i < m_capacity; i++) {
			assert(m_limiter.remove(i) == m_limiter.end());
			assert(m_limiter.remove(i) == m_limiter.end());
		}

		// test with full capacity_addr
		for(size_t i = 0; i < m_capacity; i++) {
			assert(m_limiter.check(i));
			assert(m_limiter.remove(i) != m_limiter.end());
			assert(m_limiter.remove(i) == m_limiter.end());
		}
		assert(m_limiter.size() == 0);

		// test with double capacity_addr
		for(size_t i = 0; i < m_capacity * 2; i++) {
			assert(m_limiter.check(i));
			assert(m_limiter.remove(i) != m_limiter.end());
			assert(m_limiter.remove(i) == m_limiter.end());
		}
		assert(m_limiter.size() == 0);

		// test with double capacity_addr
		for(size_t i = 0; i < m_capacity * 2; i++) {
			assert(m_limiter.check(i));
		}
		for(size_t i = 0; i < m_capacity * 2; i++) {
			if(i < m_capacity) {
				assert(m_limiter.remove(i) == m_limiter.end());
			} else {
				assert(m_limiter.remove(i) != m_limiter.end());
			}
		}
		assert(m_limiter.size() == 0);

	}

	void test_clear(unsigned step) noexcept {
		printf("-> test_clear(step=%u)\n", step);

		assert(m_limiter.size() == 0);
		m_limiter.reset();

		assert(m_limiter.size() == 0);
		for(size_t i = 0; i < m_capacity; i++) {
			m_limiter.check(i);
		}
		m_limiter.reset();
		assert(m_limiter.size() == 0);
	}

private:

	static void wait_cycles(uint64_t cycles) noexcept {
		uint64_t init = rte_rdtsc();
		while(rte_rdtsc() - init < cycles);
	}

};

}; // namespace storage

#endif /* STORAGE_TESTS_TESTRATELIMITER_H */

