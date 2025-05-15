#pragma once

#include "test_environment.h"

#include <lib/utils/DiceMachine.h>
#include <lib/utils/SlidingArray.h>

#include <cstring>
#include <functional>

template <size_t Capacity>
class TestSlidingArray {

	using Value_t = size_t;
	static constexpr Value_t EmptyValue = SIZE_MAX;
	using Rab_t = utils::SlidingArray<Value_t, Capacity>;
	utils::DiceMachine _dice_machine;

	Rab_t _rab;

public:

	TestSlidingArray() noexcept : _dice_machine(0),  _rab(EmptyValue) {
		ASSERT(Capacity > 0);
		ASSERT(Capacity < SIZE_MAX / 4u);

		test_edge_cases();
		test_example();

		test_moving_forward_increment();
		test_moving_forward_rand();

		test_moving_backward_increment();
		test_moving_backward_rand();

		test_moving_forward_available_increment();
		test_moving_forward_available_rand();

		test_moving_backward_available_increment();
		test_moving_backward_available_rand();

	}

private:

	void test_edge_cases() noexcept {
		TEST_TRACE;

		_rab.initialize(0, 0);
		ASSERT(_rab.head() == 0);
		ASSERT(_rab.head_room() == 0);
		ASSERT(_rab.tail() == 0);
		ASSERT(_rab.tail_room() == Capacity);
		ASSERT(_rab.size() == 0);

		_rab.initialize(0, Capacity);
		ASSERT(_rab.head() == 0);
		ASSERT(_rab.head_room() == 0);
		ASSERT(_rab.tail() == Capacity);
		ASSERT(_rab.tail_room() == 0);
		ASSERT(_rab.size() == Capacity);

		_rab.initialize(0, Capacity * 2u);
		ASSERT(_rab.head() == 0);
		ASSERT(_rab.head_room() == 0);
		ASSERT(_rab.tail() == Capacity);
		ASSERT(_rab.tail_room() == 0);
		ASSERT(_rab.size() == Capacity);

		_rab.initialize(SIZE_MAX, 0);
		ASSERT(_rab.head() == SIZE_MAX);
		ASSERT(_rab.head_room() == Capacity);
		ASSERT(_rab.tail() == SIZE_MAX);
		ASSERT(_rab.tail_room() == 0);
		ASSERT(_rab.size() == 0);

		_rab.initialize(SIZE_MAX, 1u);
		ASSERT(_rab.head() == SIZE_MAX);
		ASSERT(_rab.head_room() == Capacity);
		ASSERT(_rab.tail() == SIZE_MAX);
		ASSERT(_rab.tail_room() == 0);
		ASSERT(_rab.size() == 0);

		_rab.initialize(SIZE_MAX - 1u, 0);
		ASSERT(_rab.head() == SIZE_MAX - 1u);
		ASSERT(_rab.head_room() == Capacity);
		ASSERT(_rab.tail() == SIZE_MAX - 1u);
		ASSERT(_rab.tail_room() == 1);
		ASSERT(_rab.size() == 0);

		_rab.initialize(SIZE_MAX - 1u, 1u);
		ASSERT(_rab.head() == SIZE_MAX - 1u);
		ASSERT(_rab.head_room() == Capacity - 1u);
		ASSERT(_rab.tail() == SIZE_MAX);
		ASSERT(_rab.tail_room() == 0);
		ASSERT(_rab.size() == 1);
	}

	void test_example() noexcept {
		TEST_TRACE;

		constexpr size_t Cap = 10u;
		using RAB_test_t = utils::SlidingArray<Value_t, Cap>;
		RAB_test_t rab(EmptyValue);

		// Case 0 : Moving head forward.
		rab.initialize(34, Cap);
		fill_index(rab);
		test_index(rab);
		ASSERT(count_not_empty(rab) == rab.capacity());

		ASSERT(rab.capacity() == Cap);
		ASSERT(rab.head() == 34);
		ASSERT(rab.tail() == 44);
		ASSERT(rab.size() == 10);
		rab.head_move_forward(4);
		ASSERT(rab.head() == 38);
		ASSERT(rab.tail() == 44);
		ASSERT(rab.size() == 6);
		test_index(rab);
		ASSERT(count_not_empty(rab) == 6);

		// Case 1 : Moving head forward.
		rab.initialize(38, 7);
		fill_index(rab);
		test_index(rab);

		ASSERT(rab.capacity() == Cap);
		ASSERT(rab.size() == 7);
		ASSERT(rab.head() == 38);
		ASSERT(rab.tail() == 45);
		rab.head_move_forward(4);
		ASSERT(rab.head() == 42);
		ASSERT(rab.tail() == 45);
		ASSERT(rab.size() == 3);
		test_index(rab);
		ASSERT(count_not_empty(rab) == 3);

		// Case 2 : Moving head forward.
		rab.initialize(38, 3);
		fill_index(rab);
		test_index(rab);

		ASSERT(rab.capacity() == Cap);
		ASSERT(rab.size() == 3);
		ASSERT(rab.head() == 38);
		ASSERT(rab.tail() == 41);
		rab.head_move_forward(4);
		ASSERT(rab.head() == 42);
		ASSERT(rab.tail() == 42);
		ASSERT(rab.size() == 0);
		test_index(rab);
		ASSERT(count_not_empty(rab) == 0);

		// Case 3 : Moving head backward.
		rab.initialize(78, Cap);
		fill_index(rab);
		test_index(rab);

		ASSERT(rab.capacity() == Cap);
		ASSERT(rab.head() == 78);
		ASSERT(rab.tail() == 88);
		ASSERT(rab.size() == 10);
		rab.head_move_backward(5);
		ASSERT(rab.head() == 73);
		ASSERT(rab.tail() == 83);
		ASSERT(rab.size() == 10);
		test_index(rab);

		// Case 4 : Moving head backward.
		rab.initialize(73, 2);
		fill_index(rab);
		test_index(rab);

		ASSERT(rab.capacity() == Cap);
		ASSERT(rab.head() == 73);
		ASSERT(rab.tail() == 75);
		ASSERT(rab.size() == 2);
		rab.head_move_backward(5);
		ASSERT(rab.head() == 68);
		ASSERT(rab.tail() == 75);
		ASSERT(rab.size() == 7);
		test_index(rab);

		// Case 4 : Moving head backward.
		rab.initialize(73, 6);
		fill_index(rab);
		test_index(rab);

		ASSERT(rab.capacity() == Cap);
		ASSERT(rab.head() == 73);
		ASSERT(rab.tail() == 79);
		ASSERT(rab.size() == 6);
		rab.head_move_backward(60);
		ASSERT(rab.head() == 13);
		ASSERT(rab.tail() == 23);
		ASSERT(rab.size() == 10);
		test_index(rab);

	}

	// -------------------------------------------------
	// Moving test section
	// -------------------------------------------------
	void test_moving_forward_increment() noexcept {
		TEST_TRACE;

		test_moving_forward_increment(0);
		test_moving_forward_increment(1u);
		test_moving_forward_increment(Capacity);
		test_moving_forward_increment(SIZE_MAX - Capacity - 1u);
		test_moving_forward_increment(SIZE_MAX - Capacity);
		test_moving_forward_increment(SIZE_MAX - Capacity / 2u);
		test_moving_forward_increment(SIZE_MAX - 1u);
		test_moving_forward_increment(SIZE_MAX);
	}

	void test_moving_forward_rand() noexcept {
		TEST_TRACE;

		test_moving_forward_rand(0);
		test_moving_forward_rand(1u);
		test_moving_forward_rand(Capacity);
		test_moving_forward_rand(SIZE_MAX - Capacity - 1u);
		test_moving_forward_rand(SIZE_MAX - Capacity);
		test_moving_forward_rand(SIZE_MAX - Capacity / 2u);
		test_moving_forward_rand(SIZE_MAX - 1u);
		test_moving_forward_rand(SIZE_MAX);
	}

	void test_moving_backward_increment() noexcept {
		TEST_TRACE;

		test_moving_backward_increment(0);
		test_moving_backward_increment(1u);
		test_moving_backward_increment(Capacity);
		test_moving_backward_increment(SIZE_MAX - Capacity - 1u);
		test_moving_backward_increment(SIZE_MAX - Capacity);
		test_moving_backward_increment(SIZE_MAX - Capacity / 2u);
		test_moving_backward_increment(SIZE_MAX - 1u);
		test_moving_backward_increment(SIZE_MAX);
	}

	void test_moving_backward_rand() noexcept {
		TEST_TRACE;

		test_moving_backward_rand(0);
		test_moving_backward_rand(1u);
		test_moving_backward_rand(Capacity);
		test_moving_backward_rand(SIZE_MAX - Capacity - 1u);
		test_moving_backward_rand(SIZE_MAX - Capacity);
		test_moving_backward_rand(SIZE_MAX - Capacity / 2u);
		test_moving_backward_rand(SIZE_MAX - 1u);
		test_moving_backward_rand(SIZE_MAX);
	}

	void test_moving_forward_increment(const size_t head_offset) noexcept {
		_rab.initialize(head_offset, 0);
		size_t cnt = Capacity * 7u;
		while(_rab.tail_room() > 0 && cnt > 0) {

			sanity_check(_rab);
			ASSERT(_rab.size() == 0);

			const auto head_before = _rab.head();
			const auto tail_before = _rab.tail();

			_rab.tail_move_forward(1u);
			fill_index(_rab);
			test_index(_rab);
			ASSERT(count_not_empty(_rab) == 1u);

			ASSERT(_rab.head() == head_before);
			ASSERT(_rab.tail() == tail_before + 1u);
			ASSERT(_rab.size() == 1u);

			_rab.head_move_forward(1u);

			cnt--;
		}
	}

	void test_moving_forward_rand(const size_t head_offset) noexcept {
		_rab.initialize(head_offset, 0);
		size_t cnt = Capacity * 7u;
		while(_rab.tail_room() > 0 && cnt > 0) {

			sanity_check(_rab);
			ASSERT(_rab.size() == 0);

			const auto tail_before = _rab.tail();
			auto offset = _dice_machine.lrand48() % (Capacity * 2u);
			offset = std::min(offset, SIZE_MAX - _rab.tail());

			_rab.tail_move_forward(offset);
			fill_index(_rab);
			test_index(_rab);
			ASSERT(count_not_empty(_rab) == _rab.size());

			ASSERT(_rab.head() == _rab.tail() - _rab.size());
			ASSERT(_rab.tail() == tail_before + offset);

			_rab.head_move_forward(offset);

			cnt--;
		}
	}

	void test_moving_backward_increment(const size_t head_offset) noexcept {
		_rab.initialize(head_offset, 0);
		size_t cnt = Capacity * 7u;
		while(_rab.head_room() > 0 && cnt > 0) {

			sanity_check(_rab);
			ASSERT(_rab.size() == 0);

			const auto head_before = _rab.head();
			const auto tail_before = _rab.tail();

			_rab.head_move_backward(1u);
			fill_index(_rab);
			test_index(_rab);
			ASSERT(count_not_empty(_rab) == 1u);

			ASSERT(_rab.head() == head_before - 1u);
			ASSERT(_rab.tail() == tail_before);
			ASSERT(_rab.size() == 1u);

			_rab.tail_move_backward(1u);

			cnt--;
		}
	}

	void test_moving_backward_rand(const size_t head_offset) noexcept {
		_rab.initialize(head_offset, 0);
		size_t cnt = Capacity * 7u;
		while(_rab.head_room() > 0 && cnt > 0) {

			sanity_check(_rab);
			ASSERT(_rab.size() == 0);

			const auto head_before = _rab.head();
			auto offset = _dice_machine.lrand48() % (Capacity * 2u);
			offset = std::min(offset, _rab.head());

			_rab.head_move_backward(offset);
			sanity_check(_rab);

			fill_index(_rab);
			test_index(_rab);
			ASSERT(count_not_empty(_rab) == _rab.size());

			ASSERT(_rab.head() == head_before - offset);
			ASSERT(_rab.tail() == _rab.head() + _rab.size());

			_rab.tail_move_backward(offset);

			cnt--;
		}
	}

	// -------------------------------------------------
	// Moving available test section
	// -------------------------------------------------
	void test_moving_forward_available_increment() noexcept {
		TEST_TRACE;

		test_moving_forward_available_increment(0);
		test_moving_forward_available_increment(1u);
		test_moving_forward_available_increment(Capacity);
		test_moving_forward_available_increment(SIZE_MAX - Capacity - 1u);
		test_moving_forward_available_increment(SIZE_MAX - Capacity);
		test_moving_forward_available_increment(SIZE_MAX - Capacity / 2u);
		test_moving_forward_available_increment(SIZE_MAX - 1u);
		test_moving_forward_available_increment(SIZE_MAX);
	}

	void test_moving_forward_available_rand() noexcept {
		TEST_TRACE;

		test_moving_forward_available_rand(0);
		test_moving_forward_available_rand(1u);
		test_moving_forward_available_rand(Capacity);
		test_moving_forward_available_rand(SIZE_MAX - Capacity - 1u);
		test_moving_forward_available_rand(SIZE_MAX - Capacity);
		test_moving_forward_available_rand(SIZE_MAX - Capacity / 2u);
		test_moving_forward_available_rand(SIZE_MAX - 1u);
		test_moving_forward_available_rand(SIZE_MAX);
	}

	void test_moving_backward_available_increment() noexcept {
		TEST_TRACE;

		test_moving_backward_available_increment(0);
		test_moving_backward_available_increment(1u);
		test_moving_backward_available_increment(Capacity);
		test_moving_backward_available_increment(SIZE_MAX - Capacity - 1u);
		test_moving_backward_available_increment(SIZE_MAX - Capacity);
		test_moving_backward_available_increment(SIZE_MAX - Capacity / 2u);
		test_moving_backward_available_increment(SIZE_MAX - 1u);
		test_moving_backward_available_increment(SIZE_MAX);
	}

	void test_moving_backward_available_rand() noexcept {
		TEST_TRACE;

		test_moving_backward_available_rand(0);
		test_moving_backward_available_rand(1u);
		test_moving_backward_available_rand(Capacity);
		test_moving_backward_available_rand(SIZE_MAX - Capacity - 1u);
		test_moving_backward_available_rand(SIZE_MAX - Capacity);
		test_moving_backward_available_rand(SIZE_MAX - Capacity / 2u);
		test_moving_backward_available_rand(SIZE_MAX - 1u);
		test_moving_backward_available_rand(SIZE_MAX);
	}

	void test_moving_forward_available_increment(const size_t head_offset) noexcept {
		_rab.initialize(head_offset, 0);
		size_t cnt = Capacity * 7u;
		while(_rab.tail_room() > 0 && cnt > 0) {

			sanity_check(_rab);
			ASSERT(_rab.size() == 0);

			const auto head_before = _rab.head();
			const auto tail_before = _rab.tail();

			_rab.tail_move_forward_available(1u);
			fill_index(_rab);
			test_index(_rab);
			ASSERT(count_not_empty(_rab) == 1u);

			ASSERT(_rab.head() == head_before);
			ASSERT(_rab.tail() == tail_before + 1u);
			ASSERT(_rab.size() == 1u);

			_rab.head_move_forward_available(1u);

			cnt--;
		}
	}

	void test_moving_forward_available_rand(const size_t head_offset) noexcept {
		_rab.initialize(head_offset, 0);
		size_t cnt = Capacity * 7u;
		while(_rab.tail_room() > 0 && cnt > 0) {

			sanity_check(_rab);
			ASSERT(_rab.size() == 0);

			const auto head_before = _rab.head();
			const auto tail_before = _rab.tail();
			const auto items_to_move = _dice_machine.lrand48() % (_rab.tail_room() + 1u);

			_rab.tail_move_forward_available(items_to_move);
			fill_index(_rab);
			test_index(_rab);
			ASSERT(count_not_empty(_rab) == items_to_move);

			ASSERT(_rab.head() == head_before);
			ASSERT(_rab.tail() == tail_before + items_to_move);
			ASSERT(_rab.size() == items_to_move);

			_rab.head_move_forward_available(items_to_move);

			cnt--;
		}
	}

	void test_moving_backward_available_increment(const size_t head_offset) noexcept {
		_rab.initialize(head_offset, 0);
		size_t cnt = Capacity * 7u;
		while(_rab.head_room() > 0 && cnt > 0) {

			sanity_check(_rab);
			ASSERT(_rab.size() == 0);

			const auto head_before = _rab.head();
			const auto tail_before = _rab.tail();

			_rab.head_move_backward_available(1u);
			fill_index(_rab);
			test_index(_rab);
			ASSERT(count_not_empty(_rab) == 1u);

			ASSERT(_rab.head() == head_before - 1u);
			ASSERT(_rab.tail() == tail_before);
			ASSERT(_rab.size() == 1u);

			_rab.tail_move_backward_available(1u);

			cnt--;
		}
	}

	void test_moving_backward_available_rand(const size_t head_offset) noexcept {
		_rab.initialize(head_offset, 0);
		size_t cnt = Capacity * 7u;
		while(_rab.head_room() > 0 && cnt > 0) {

			sanity_check(_rab);
			ASSERT(_rab.size() == 0);

			const auto head_before = _rab.head();
			const auto tail_before = _rab.tail();
			const auto items_to_move = _dice_machine.lrand48() % (_rab.head_room() + 1u);

			_rab.head_move_backward_available(items_to_move);
			fill_index(_rab);
			test_index(_rab);
			ASSERT(count_not_empty(_rab) == items_to_move);

			ASSERT(_rab.head() == head_before - items_to_move);
			ASSERT(_rab.tail() == tail_before);
			ASSERT(_rab.size() == items_to_move);

			_rab.tail_move_backward_available(items_to_move);

			cnt--;
		}
	}


	template <size_t Cap>
	static void dump(const utils::SlidingArray<Value_t, Cap>& rab) noexcept {
		printf("utils::SlidingArray :");
		printf(" head=%zu", rab.head());
		printf(" head_room=%zu", rab.head_room());
		printf(" tail=%zu", rab.tail());
		printf(" tail_room=%zu", rab.tail_room());
		printf(" size=%zu", rab.size());
		printf("\n");

		printf("IDX : ");
		for(size_t idx = rab.head(); idx < rab.tail(); ++idx) {
			printf("%.2zu ", idx);
		}
		printf("\nVAL : ");
		for(size_t idx = rab.head(); idx < rab.tail(); ++idx) {
			if(rab[idx] == EmptyValue) {
				printf("-- ");
			} else {
				printf("%.2zu ", rab[idx]);
			}
		}
		printf("\n");
	}

private:

	template <typename V, size_t C>
	void sanity_check(const utils::SlidingArray<V, C>& rab) noexcept {
		ASSERT(rab.head_room() <= rab.capacity());
		ASSERT(rab.tail_room() <= rab.capacity());
		ASSERT(rab.head() <= rab.tail());
		ASSERT(rab.head() + rab.size() == rab.tail());
		ASSERT(rab.size() <= rab.capacity());
	}

	template <typename V, size_t C>
	static void fill_const(utils::SlidingArray<V, C>& rab, const V val) noexcept {
		for(size_t i = rab.head(); i < rab.tail(); ++i) {
			rab[i] = val;
		}
	}

	template <typename V, size_t C>
	static void test_const(utils::SlidingArray<V, C>& rab, const V val) noexcept {
		for(size_t i = rab.head(); i < rab.tail(); ++i) {
			if(rab[i] != EmptyValue) {
				ASSERT(rab[i] == val);
			}
		}
	}

	template <typename V, size_t C>
	static void fill_index(utils::SlidingArray<V, C>& rab) noexcept {
		for(size_t i = rab.head(); i < rab.tail(); ++i) {
			rab[i] = i;
		}
	}

	template <typename V, size_t C>
	static void test_index(utils::SlidingArray<V, C>& rab) noexcept {
		for(size_t i = rab.head(); i < rab.tail(); ++i) {
			if(rab[i] != EmptyValue) {
				ASSERT(rab[i] == i);
			}
		}
	}

	template <typename V, size_t C>
	static size_t count_sum(utils::SlidingArray<V, C>& rab) noexcept {
		size_t result = 0;
		for(size_t i = rab.head(); i < rab.tail(); ++i) {
			if(rab[i] != EmptyValue) {
				result += rab[i];
			}
		}
		return result;
	}

	template <typename V, size_t C>
	static size_t count_not_empty(utils::SlidingArray<V, C>& rab) noexcept {
		size_t result = 0;
		for(size_t i = rab.head(); i < rab.tail(); ++i) {
			if(rab[i] != EmptyValue) {
				result++;
			}
		}
		return result;
	}

	template <typename V, size_t C>
	static void test_empty(utils::SlidingArray<V, C>& rab) noexcept {
		ASSERT(count_empty(rab) == rab.size());
		if(rab.head() > 0) {
			ASSERT(rab.get(rab.head() - 1u) == EmptyValue);
		}
		ASSERT(rab.get(rab.tail()) == EmptyValue);
	}

};
