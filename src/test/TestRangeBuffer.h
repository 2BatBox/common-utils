#pragma once

#include "test_environment.h"
#include <containers/RangeBuffer.h>
#include <cstring>

class TestRangeBuffer {

	using Value_t = ssize_t;
	using RangeBuffer_t = RangeBuffer<Value_t>;
	static constexpr int EmptyValue = -1ll;

	RangeBuffer_t _dut;

public:

	TestRangeBuffer(size_t capacity) noexcept : _dut(capacity, EmptyValue) {
		test_resize();
		test_head_move();
	}

private:

	void test_resize() noexcept {
		TEST_TRACE;
		fill_monotonic(0, 0, 0);
		validate_monotonic(0, 0, 0);

		fill_monotonic(1, 0, 0);
		validate_monotonic(1, 0, 0);

		fill_monotonic(1, 1, 0);
		validate_monotonic(1, 1, 0);

		fill_monotonic(_dut.capacity() / 2u, 1, 0);
		validate_monotonic(_dut.capacity() / 2u, 1, 0);

		fill_monotonic(_dut.capacity(), 1, 0);
		validate_monotonic(_dut.capacity(), 1, 0);

		_dut.resize(_dut.capacity() / 2u);
		validate_monotonic(_dut.capacity() / 2u, 1, 0);

		_dut.resize(1u);
		validate_monotonic(1u, 1, 0);

		_dut.resize(0);
		validate_monotonic(0, 1, 0);
	}

	void test_head_move() noexcept {
		TEST_TRACE;
		const auto center = _dut.max_head_index() / 2u;
		fill_monotonic(_dut.capacity(), center, 1);
		validate_monotonic(_dut.capacity(), center, 1);

		_dut.head_index(center - 1u);
		_dut.front() = 0;
		validate_monotonic(_dut.capacity(), center - 1u, 0);

		_dut.head_index(center + 1u);
		validate_monotonic(_dut.capacity() - 2u, center + 1u, 2);
	}

	void fill_monotonic(size_t size, size_t index_first, Value_t value_first) noexcept {
		_dut.head_index(index_first);
		_dut.resize(size);
		for(size_t idx = _dut.head_index(); idx < _dut.tail_index(); ++idx) {
			_dut[idx] = value_first;
			value_first++;
		}
	}

	void validate_monotonic(size_t size, size_t index_first, Value_t value_first) noexcept {
		assert(_dut.size() == size);
		assert(_dut.head_index() == index_first);
		for(size_t idx = _dut.head_index(); idx < _dut.tail_index(); ++idx) {
			assert(_dut[idx] == value_first);
			value_first++;
		}
		assert(_dut[_dut.head_index() - 1u] == EmptyValue);
		assert(_dut[_dut.tail_index() + 1u] == EmptyValue);
	}

	void dump() const noexcept {
		printf("RangeBuffer");
		printf(" capacity=%zu", _dut.capacity());
		printf(" size=%zu", _dut.size());
		printf(" max_head_index=%zu", _dut.max_head_index());
		printf(" head_index=%zu", _dut.head_index());
		printf(" tail_index=%zu", _dut.tail_index());
		printf("\n");
		for(size_t idx = _dut.head_index(); idx < _dut.tail_index(); ++idx) {
			printf("[%zu] : %ld\n", idx, _dut[idx]);
		}
	}

};
