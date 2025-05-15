#pragma once

#include "test_environment.h"
#include <intrusive/LinkedList.h>

#include <assert.h>
#include <cstdio>
#include <cstddef>
#include <iostream>
#include <memory>

class TestIntrusiveLinkedList {

	template<typename T>
	struct StructValue {
		T value;
		intrusive::LinkedListHook<StructValue<T> > __ill;

		StructValue() : value() {}

		StructValue(unsigned int x) : value(x) {}

		bool operator==(const StructValue& st_val) const {
			return value == st_val.value;
		}
	};

	using Key_t = unsigned;
	using ListNode_t = StructValue<unsigned>;
	using List_t = intrusive::LinkedList<ListNode_t, true>;

	List_t list;
	const unsigned storage_size;
	std::unique_ptr<ListNode_t[]> storage;

public:

	TestIntrusiveLinkedList(unsigned storage_size) : list(), storage_size(storage_size), storage(new ListNode_t[storage_size]) {
		for(Key_t i = 0; i < storage_size; i++) {
			storage[i].value = i;
		}

		test_raii();
		test_push_front();
		test_push_back();
		test_pop_front();
		test_pop_back();
		test_remove();
		test_insert_before();
		test_insert_after();
		test_iterators_forward();
		test_iterators_backward();

	}

	TestIntrusiveLinkedList(const TestIntrusiveLinkedList&) = delete;
	TestIntrusiveLinkedList(TestIntrusiveLinkedList&&) = delete;

	TestIntrusiveLinkedList operator=(const TestIntrusiveLinkedList&) = delete;
	TestIntrusiveLinkedList operator=(TestIntrusiveLinkedList&&) = delete;

	~TestIntrusiveLinkedList() {
		// The list must be empty before the storage has been destroyed.
		list.clear();
	}

	size_t storage_bytes() {
		return storage_size * sizeof(ListNode_t);
	}

	void test_raii() {
		TEST_TRACE;

		assert(list.size() == 0);

		List_t list_tmp(std::move(list));
		fill_forward(list_tmp);

		assert(list.size() == 0);

		list = std::move(list);
		list = std::move(list_tmp);
		std::swap(list, list_tmp);
		std::swap(list, list_tmp);

		compare_forward(list);
		assert(list_tmp.size() == 0);

		clear_list(list);
	}

	void test_push_front() {
		TEST_TRACE;

		assert(list.size() == 0);
		fill_backward(list);
		compare_backward(list);
		clear_list(list);
	}

	void test_push_back() {
		TEST_TRACE;

		assert(list.size() == 0);
		fill_forward(list);
		compare_forward(list);
		clear_list(list);
	}

	void test_pop_front() {
		TEST_TRACE;

		assert(list.size() == 0);
		fill_forward(list);
		for(Key_t i = 0; i < storage_size; i++) {
			auto it = list.cbegin();
			assert((*it) == storage[i]);
			assert(list.pop_front() == &storage[i]);
		}
		assert(list.pop_front() == nullptr);
		clear_list(list);
	}

	void test_pop_back() {
		TEST_TRACE;

		assert(list.size() == 0);
		fill_backward(list);
		for(Key_t i = 0; i < storage_size; i++) {
			auto it = list.crbegin();
			assert((*it) == storage[i]);
			assert(list.pop_back() == &storage[i]);
		}
		assert(list.pop_back() == nullptr);
		clear_list(list);
	}

	void test_insert_before() {
		TEST_TRACE;

		assert(list.size() == 0);
		list.push_front(storage[0]);
		for(Key_t i = 1; i < storage_size; i++) {
			list.insert_before(storage[i - 1], storage[i]);
		}
		compare_backward(list);
		clear_list(list);
	}

	void test_insert_after() {
		TEST_TRACE;

		assert(list.size() == 0);
		list.push_front(storage[0]);
		for(Key_t i = 1; i < storage_size; i++) {
			list.insert_after(storage[i - 1], storage[i]);
		}
		compare_forward(list);
		clear_list(list);
	}

	void test_remove() {
		TEST_TRACE;

		assert(list.size() == 0);
		fill_forward(list);
		for(Key_t i = 0; i < storage_size; i++) {
			if(i % 2 == 0) {
				list.remove(storage[i]);
			}
		}
		for(Key_t i = 0; i < storage_size; i++) {
			if(i % 2 != 0) {
				list.remove(storage[i]);
			}
		}
		assert(list.size() == 0);
		test_sanity();
	}

	void test_iterators_forward() {
		TEST_TRACE;

		assert(list.size() == 0);
		fill_forward(list);
		assert(list.size() == storage_size);

		// ConstIterator_t::operator++
		Key_t index = 0;
		for(auto it = list.cbegin(); it != list.cend(); ++it) {
			assert((*it) == storage[index]);
			assert((it->value) == storage[index].value);
			index++;
		}

		// ConstIterator_t::operator++(int)
		index = 0;
		for(auto it = list.cbegin(); it != list.cend(); it++) {
			assert((*it) == storage[index]);
			assert((it->value) == storage[index].value);
			index++;
		}

		// Iterator_t::operator++
		index = 0;
		for(auto it = list.begin(); it != list.end(); ++it) {
			assert((*it) == storage[index]);
			assert((it->value) == storage[index].value);
			index++;
		}

		// Iterator_t::operator++(int)
		index = 0;
		for(auto it = list.begin(); it != list.end(); it++) {
			assert((*it) == storage[index]);
			assert((it->value) == storage[index].value);
			index++;
		}
		clear_list(list);
	}

	void test_iterators_backward() {
		TEST_TRACE;

		assert(list.size() == 0);
		fill_backward(list);
		assert(list.size() == storage_size);

		// ConstIterator_t::operator++
		Key_t index = 0;
		for(auto it = list.crbegin(); it != list.crend(); ++it) {
			assert((*it) == storage[index]);
			assert((it->value) == storage[index].value);
			index++;
		}

		// ConstIterator_t::operator++(int)
		index = 0;
		for(auto it = list.crbegin(); it != list.crend(); it++) {
			assert((*it) == storage[index]);
			assert((it->value) == storage[index].value);
			index++;
		}

		// Iterator_t::operator++
		index = 0;
		for(auto it = list.rbegin(); it != list.rend(); ++it) {
			assert((*it) == storage[index]);
			assert((it->value) == storage[index].value);
			index++;
		}

		// Iterator_t::operator++(int)
		index = 0;
		for(auto it = list.rbegin(); it != list.rend(); it++) {
			assert((*it) == storage[index]);
			assert((it->value) == storage[index].value);
			index++;
		}
		clear_list(list);
	}

	void dump() const noexcept {
		std::cout << "list has " << list.size() << " elements \n";
		for(auto it = list.cbegin(); it != list.cend(); ++it) {
			std::cout << (*it).value << " ";
		}
		std::cout << "\n";
	}

private:

	void test_sanity() {
		for(unsigned i = 0; i < storage_size; i++) {
			assert(not storage[i].__ill.linked);
		}
		assert(list.size() == 0);
	}

	void fill_forward(List_t& list) {
		for(Key_t i = 0; i < storage_size; i++) {
			list.push_back(storage[i]);
		}
		assert(list.size() == storage_size);
	}

	void fill_backward(List_t& list) {
		for(Key_t i = 0; i < storage_size; i++) {
			list.push_front(storage[i]);
		}
		assert(list.size() == storage_size);
	}

	void compare_forward(List_t& list) {
		Key_t index = 0;
		assert(list.size() == storage_size);
		for(auto it = list.cbegin(); it != list.cend(); ++it) {
			assert((*it) == storage[index++]);
		}
	}

	void compare_backward(List_t& list) {
		Key_t index = 0;
		assert(list.size() == storage_size);
		for(auto it = list.crbegin(); it != list.crend(); ++it) {
			assert((*it) == storage[index++]);
		}
	}

	void clear_list(List_t& list) {
		list.clear();
		assert(list.size() == 0);
		test_sanity();
	}

};

