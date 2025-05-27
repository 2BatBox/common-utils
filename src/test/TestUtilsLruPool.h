#pragma once

#include "test_environment.h"

#include <lib/utils/LruPool.h>

#include <iostream>

class TestUtilsLruPool {

	using Key_t = long long unsigned;
	using Value_t = long long unsigned;

	using Pool_t = utils::LruPool<Key_t, Value_t>;

	const size_t _capacity;
	Pool_t _pool;

public:

	TestUtilsLruPool(unsigned capacity, float load_factor) :
		_capacity(capacity),
		_pool(capacity, load_factor)
	{
		unsigned step = 1;
		test_acquire_find_release_single(step++);
		test_acquire_find_release_bulk(step++);
		test_clear(step++);
	}

	TestUtilsLruPool(const TestUtilsLruPool&) = delete;
	TestUtilsLruPool(TestUtilsLruPool&&) = delete;

	TestUtilsLruPool operator=(const TestUtilsLruPool&) = delete;
	TestUtilsLruPool operator=(TestUtilsLruPool&&) = delete;

	~TestUtilsLruPool() {}

	void test_acquire_find_release_single(unsigned step) {
		TEST_TRACE;
		ASSERT(_pool.size() == 0);

		for(size_t i = 0; i < _capacity * 2; i++) {
			const Key_t k = i + step;
			const Value_t v = k + 1000u;

			auto item = _pool.acquire(k);
			ASSERT(item != nullptr);
			item->value() = v;

			ASSERT(item->is_acquired());
			ASSERT(item->key() == k);
			ASSERT(item->value() == v);

			auto it = _pool.find(k);
			ASSERT(it != _pool.end());
			ASSERT(item == &(*it));

			_pool.release(item);
			ASSERT(_pool.size() == 0);
		}
		
	}

	void test_acquire_find_release_bulk(unsigned step) {
		TEST_TRACE;
		ASSERT(_pool.size() == 0);

		for(size_t i = 0; i < _capacity * 2; i++) {
			const Key_t k = i + step;
			const Value_t v = k + 1000u;

			auto item = _pool.acquire(k);
			ASSERT(item != nullptr);
			item->value() = v;

			ASSERT(item->is_acquired());
			ASSERT(item->key() == k);
			ASSERT(item->value() == v);
		}

		ASSERT(_pool.size() == _pool.capacity());

		for(size_t i = 0; i < _capacity * 2; i++) {
			const Key_t k = i + step;
			const Value_t v = k + 1000u;

			auto it = _pool.find(k);
			if(i < _capacity) {
				ASSERT(it == _pool.end());
			} else {
				ASSERT(it != _pool.end());
				ASSERT(it->is_acquired());
				ASSERT(it->key() == k);
				ASSERT(it->value() == v);
			}
		}

		ASSERT(_pool.size() == _pool.capacity());

		for(size_t i = 0; i < _capacity * 2; i++) {
			const Key_t k = i + step;
			const Value_t v = k + 1000u;

			auto it = _pool.find(k);
			if(i < _capacity) {
				ASSERT(it == _pool.end());
			} else {
				ASSERT(it != _pool.end());
				ASSERT(it->is_acquired());
				ASSERT(it->key() == k);
				ASSERT(it->value() == v);
				_pool.release(it.get());
			}
		}

		ASSERT(_pool.size() == 0);
	}

	void test_clear(unsigned step) {
		TEST_TRACE;
		ASSERT(_pool.size() == 0);

		for(size_t i = 0; i < _capacity * 2; i++) {
			const Key_t k = i + step;
			const Value_t v = k + 1000u;

			auto item = _pool.acquire(k);
			ASSERT(item != nullptr);
			item->value() = v;

			ASSERT(item->is_acquired());
			ASSERT(item->key() == k);
			ASSERT(item->value() == v);
		}

		_pool.reset();

		for(size_t i = 0; i < _capacity * 2; i++) {
			const Key_t k = i + step;
			auto it = _pool.find(k);
			ASSERT(it == _pool.end());
		}

		ASSERT(_pool.size() == 0);
	}

	void dump() {
		const auto bkt = _pool._map.buckets();
		std::cout << "map has " << bkt << " buckets \n";
		for(size_t bucket = 0; bucket < bkt; ++bucket) {
			std::cout << "B[" << bucket << "] ";
			for(auto it = _pool._map.cbegin(bucket); it != _pool._map.cend(); ++it) {
				std::cout << it->key() << ":" << it->value() << " -> ";
			}
			std::cout << "\n";
		}
		std::cout << "used list has " << _pool._list_used.size() << " elements \n";
		for(auto it = _pool._list_used.cbegin(); it != _pool._list_used.cend(); ++it) {
			std::cout << it->key() << ":" << it->value() << ", ";
		}
		std::cout << "\n";
		std::cout << "freed list has " << _pool._list_freed.size() << " elements \n";
		for(auto it = _pool._list_freed.cbegin(); it != _pool._list_freed.cend(); ++it) {
			std::cout << it->value() << ", ";
		}
		std::cout << "\n";
	}

};
