#ifndef INTRUSIVE_TESTS_TEST_LIST_H
#define INTRUSIVE_TESTS_TEST_LIST_H

#include "../List.h"

#include <assert.h>
#include <cstdio>
#include <cstddef>

namespace intrusive {

class TestList {

	template <typename V>
	struct ListNode : public ListHook<ListNode<V> > {
		V value;

		ListNode() : value() { }

		ListNode(V v) : value(v) { }

		bool operator==(const ListNode& data) const {
			return value == data.value;
		}
	};

	template<typename T>
	struct StructValue {
		T value;

		StructValue() : value() { }

		StructValue(unsigned int x) : value(x) { }

		bool operator==(const StructValue& st_val) const {
			return value == st_val.value;
		}
	};

	using Key_t = unsigned;
	using Value_t = StructValue<unsigned>;
	using ListNode_t = ListNode<Value_t>;
	using List_t = List<ListNode_t>;

	List_t list;
	const unsigned storage_size;
	ListNode_t* storage;

public:

	TestList(unsigned storage_size) : list(), storage_size(storage_size), storage(new ListNode_t[storage_size]) {
		for (unsigned i = 0; i < storage_size; i++) {
			storage[i].value = Value_t(i);
		}
	}

	TestList(const TestList&) = delete;
	TestList(TestList&&) = delete;

	TestList operator=(const TestList&) = delete;
	TestList operator=(TestList&&) = delete;

	~TestList() {
		// The list must be empty before the storage has been destroyed.
		list.clear(); 
		delete [] storage;
	}

	size_t storage_bytes() {
		return storage_size * sizeof (ListNode_t);
	}

	void test() {
		printf("<intrusive::ListTest>...\n");
		printf("sizeof(Value_t)=%zu\n", sizeof (Value_t));
		printf("sizeof(ListData_t)=%zu\n", sizeof (ListNode_t));
		//		printf("offsetof(ListData_t, il_next)=%zu\n", offsetof(ListData_t, il_next));
		//		printf("offsetof(ListData_t, il_prev)=%zu\n", offsetof(ListData_t, il_prev));
		//		printf("offsetof(ListData_t, il_linked)=%zu\n", offsetof(ListData_t, il_linked));
		//		printf("offsetof(ListData_t, value)=%zu\n", offsetof(ListData_t, value));
		printf("memory used %zu Kb\n", storage_bytes() / (1024));
		test_raii();
		test_push_front();
		test_push_back();
		test_pop_front();
		test_pop_back();
		test_remove();
		test_insert_before();
		test_insert_after();
		test_iterators();
	}

	void test_raii() {
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
		assert(list.size() == 0);
		fill_backward(list);
		compare_backward(list);
		clear_list(list);
	}

	void test_push_back() {
		assert(list.size() == 0);
		fill_forward(list);
		compare_forward(list);
		clear_list(list);
	}

	void test_pop_front() {
		assert(list.size() == 0);
		fill_forward(list);
		for (Key_t i = 0; i < storage_size; i++) {
			auto it = list.cbegin();
			assert((*it) == storage[i]);
			assert(list.pop_front() == &storage[i]);
		}
		assert(list.pop_front() == nullptr);
		clear_list(list);
	}

	void test_pop_back() {
		assert(list.size() == 0);
		fill_backward(list);
		for (Key_t i = 0; i < storage_size; i++) {
			auto it = list.crbegin();
			assert((*it) == storage[i]);
			assert(list.pop_back() == &storage[i]);
		}
		assert(list.pop_back() == nullptr);
		clear_list(list);
	}

	void test_insert_before() {
		assert(list.size() == 0);
		list.push_front(storage[0]);
		for (Key_t i = 1; i < storage_size; i++) {
			assert(list.insert_before(storage[i - 1], storage[i]));
		}
		compare_backward(list);
		clear_list(list);
	}

	void test_insert_after() {
		assert(list.size() == 0);
		list.push_front(storage[0]);
		for (Key_t i = 1; i < storage_size; i++) {
			assert(list.insert_after(storage[i - 1], storage[i]));
		}
		compare_forward(list);
		clear_list(list);
	}

	void test_remove() {
		assert(list.size() == 0);
		fill_forward(list);
		Key_t half = storage_size / 2;
		for (Key_t i = half; i < storage_size; i++) {
			assert(list.remove(storage[i]));
			assert(not list.remove(storage[i]));
		}
		for (Key_t i = 0; i < half; i++) {
			assert(list.remove(storage[i]));
			assert(not list.remove(storage[i]));
		}
		assert(list.size() == 0);
		test_sanity();
	}

	void test_iterators() {
		assert(list.size() == 0);
		fill_forward(list);
		assert(list.size() == storage_size);

		// ConstIterator_t::operator++
		Key_t index = 0;
		for (auto it = list.cbegin(); it != list.cend(); ++it) {
			assert((*it) == storage[index]);
			assert((it->value) == storage[index].value);
			index++;
		}

		// ConstIterator_t::operator++(int)
		index = 0;
		for (auto it = list.cbegin(); it != list.cend(); it++) {
			assert((*it) == storage[index]);
			assert((it->value) == storage[index].value);
			index++;
		}

		// Iterator_t::operator++
		index = 0;
		for (auto it = list.begin(); it != list.end(); ++it) {
			assert((*it) == storage[index]);
			assert((it->value) == storage[index].value);
			index++;
		}

		// Iterator_t::operator++(int)
		index = 0;
		for (auto it = list.begin(); it != list.end(); it++) {
			assert((*it) == storage[index]);
			assert((it->value) == storage[index].value);
			index++;
		}
		clear_list(list);
	}

	void dump() const noexcept {
		std::cout << "list has " << list.size() << " elements \n";
		for (auto it = list.cbegin(); it != list.cend(); ++it) {
			std::cout << (*it).value.value << " ";
		}
		std::cout << "\n";
	}

private:

	void test_sanity() {
		for (unsigned i = 0; i < storage_size; i++) {
			assert(not storage[i].il_linked);
		}
		assert(list.size() == 0);
	}

	void fill_forward(List_t& list) {
		for (Key_t i = 0; i < storage_size; i++) {
			assert(list.push_back(storage[i]));
			assert(not list.push_back(storage[i]));
		}
		assert(list.size() == storage_size);
	}

	void fill_backward(List_t& list) {
		for (Key_t i = 0; i < storage_size; i++) {
			assert(list.push_front(storage[i]));
			assert(not list.push_front(storage[i]));
		}
		assert(list.size() == storage_size);
	}

	void compare_forward(List_t& list) {
		Key_t index = 0;
		assert(list.size() == storage_size);
		for (auto it = list.cbegin(); it != list.cend(); ++it) {
			assert((*it) == storage[index++]);
		}
	}

	void compare_backward(List_t& list) {
		Key_t index = 0;
		assert(list.size() == storage_size);
		for (auto it = list.crbegin(); it != list.crend(); ++it) {
			assert((*it) == storage[index++]);
		}
	}

	void clear_list(List_t& list) {
		list.clear();
		assert(list.size() == 0);
		test_sanity();
	}

};

}; // namespace intrusive

#endif /* INTRUSIVE_TESTS_TEST_LIST_H */

