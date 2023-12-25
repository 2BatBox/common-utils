#pragma once

#include "test_environment.h"
#include <containers/RingArrayBuffer.h>
#include <cstring>

template <size_t Capacity>
class TestRinArrayBuffer {

	using Value_t = ssize_t;
	using SlidingArray_t = RingArrayBuffer<Value_t, Capacity>;
	static constexpr int EmptyValue = -1ll;

	SlidingArray_t _dut;

public:

	TestRinArrayBuffer() noexcept : _dut(EmptyValue) {
		test_fill();
		test_head_move_right();
	}

private:

	void test_fill() noexcept {
		TEST_TRACE;

		_dut.head(0, 0);
		_dut.fill(_dut.capacity(), 1);
		check_filled(0, 1);

		_dut.head(_dut.head_max() / 2u, 0);
		_dut.fill(_dut.capacity(), 1);
		check_filled(_dut.head_max() / 2u, 1);

		_dut.fill(_dut.capacity(), 0);
		check_filled(_dut.head_max() / 2u, 0);

		_dut.head(_dut.head_max(), 0);
		_dut.fill(_dut.capacity(), 1);
		check_filled(_dut.head_max(), 1);
	}

	void test_head_move_right() noexcept {
		TEST_TRACE;
		_test_head_move_right(0, 0);
		_test_head_move_right(1, 0);
		_test_head_move_right(_dut.head_max() / 4u, 0);
		_test_head_move_right(_dut.head_max() / 2u, 0);
		_test_head_move_right(_dut.head_max() - 1u, 0);

		_test_head_move_right(0, 1);
		_test_head_move_right(1, 1);
		_test_head_move_right(_dut.head_max() / 4u, 1);
		_test_head_move_right(_dut.head_max() / 2u, 1);
		_test_head_move_right(_dut.head_max() - 1u, 1);

		_dut.head(0, -1);
		fill_monotonic(1);
		size_t cap = _dut.capacity();
		while(cap--) {
			_dut.head(_dut.head() + 1u, -1);
		}
		check_filled(_dut.capacity(), -1);
	}

	void _test_head_move_right(size_t offset, Value_t val) noexcept {
		const auto new_item_value = val + _dut.capacity();
		_dut.head(offset, 0);
		fill_monotonic(val);
		check_monotonic(val);
		_dut.head(offset + 1u, new_item_value);
		check_monotonic(val + 1);
	}

	void fill_monotonic(Value_t value) noexcept {
		for(size_t idx = _dut.head(); idx < _dut.tail(); ++idx) {
			_dut[idx] = value;
			value++;
		}
	}

	void check_monotonic(Value_t value) noexcept {
		for(size_t idx = _dut.head(); idx < _dut.tail(); ++idx) {
			assert(_dut[idx] == value);
			value++;
		}
	}

	void check_filled(size_t first, Value_t value) noexcept {
		assert(_dut.head() == first);
		for(size_t idx = _dut.head(); idx < _dut.tail(); ++idx) {
			assert(_dut[idx] == value);
		}
	}

	void dump() const noexcept {
		printf("RingArrayBuffer");
		printf(" capacity=%zu", _dut.capacity());
		printf(" max_head_index=%zu", _dut.head_max());
		printf(" head_index=%zu", _dut.head());
		printf(" tail_index=%zu", _dut.tail());
		printf("\n");
		for(size_t idx = _dut.head(); idx < _dut.tail(); ++idx) {
			printf("[%zu] : %ld\n", idx, _dut[idx]);
		}
	}

};
