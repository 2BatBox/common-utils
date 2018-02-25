#ifndef INTRUSIVELISTTEST_H
#define INTRUSIVELISTTEST_H

#include <assert.h>
#include <cstdio>

#include "IntrusiveList.h"

class IntrusiveListTest {
	
	template<typename T>
	struct StructValue {
		T x;
		StructValue() : x() {};
		StructValue(unsigned int x) : x(x) {};
		bool operator==(const StructValue& st_val) const {
			return x == st_val.x;
		}
	};

	template <typename V>
	struct Data : public IntrusiveListHook<Data<V> > {
		V value;
		Data() : value() {}
		Data(V v) : value(v) {}
		bool operator==(const Data& data) const {
			return value == data.value;
		}
	};
	
	typedef unsigned Key_t;
	typedef StructValue<long long unsigned> Value_t;
	typedef Data<Value_t> Data_t;
	typedef IntrusiveList<Data_t> List_t;
	
	List_t list;
	const unsigned storage_size;
	Data_t* storage;
	
public:
	
	IntrusiveListTest(unsigned storage_size) : list(), storage_size(storage_size), storage(new Data_t[storage_size]) {
		for (unsigned i = 0; i < storage_size; i++) {
			storage[i].value = Value_t(i);
		}
	}
	
	IntrusiveListTest(const IntrusiveListTest&) = delete;
	IntrusiveListTest(IntrusiveListTest&&) = delete;
	
	IntrusiveListTest operator=(const IntrusiveListTest&) = delete;
	IntrusiveListTest operator=(IntrusiveListTest&&) = delete;
	
	~IntrusiveListTest(){
		delete [] storage;
	}
	
	size_t mem_used(){
		return storage_size * sizeof(Data_t);
	}
	
	void test(){
		printf("<IntrusiveListTest>...\n");
		printf("sizeof(Data_t)=%zu\n", sizeof(Data_t));
		printf("memory used %zu Kb\n", mem_used() / (1024));
		test_raii();
		test_push_front();
		test_push_back();
		test_pop_front();
		test_pop_back();
		test_remove();
		test_insert_before();
		test_insert_after();
	}
	
	void test_sanity(){
		for (unsigned i = 0; i < storage_size; i++) {
			assert(not storage[i].il_linked);
		}
	}
	
	void test_raii() { 
		Key_t index = 0;
		assert(list.size() == 0);
		
		List_t list_tmp(std::move(list));
		
		for (Key_t i = 0; i < storage_size; i++) {
			assert(list_tmp.push_front(storage[i]));
		}
		assert(list.size() == 0);
		assert(list_tmp.size() == storage_size);
		
		list = std::move(list);
		list = std::move(list_tmp);
		std::swap(list, list_tmp);
		std::swap(list, list_tmp);
		
		for (auto it = list.crbegin(); it != list.crend(); ++it) {
			assert((*it) == storage[index++]);
		}
		assert(list.size() == storage_size);
		assert(list_tmp.size() == 0);
		
		list.reset();
		assert(list.size() == 0);
		test_sanity();
	}

	void test_push_front() { 
		Key_t index = 0;
		assert(list.size() == 0);
		
		for (Key_t i = 0; i < storage_size; i++) {
			assert(list.push_front(storage[i]));
		}
		assert(list.size() == storage_size);
		
		for (auto it = list.crbegin(); it != list.crend(); ++it) {
			assert((*it) == storage[index++]);
		}
		
		list.reset();
		assert(list.size() == 0);
		test_sanity();
	}
	
	void test_push_back() { 
		Key_t index = 0;
		assert(list.size() == 0);
		
		for (Key_t i = 0; i < storage_size; i++) {
			assert(list.push_back(storage[i]));
		}
		assert(list.size() == storage_size);
		
		for (auto it = list.cbegin(); it != list.cend(); ++it) {
			assert((*it) == storage[index++]);
		}
		
		list.reset();
		assert(list.size() == 0);
		test_sanity();
	}
	
	void test_pop_front() { 
		assert(list.size() == 0);
		
		for (Key_t i = 0; i < storage_size; i++) {
			assert(list.push_back(storage[i]));
		}
		assert(list.size() == storage_size);
		
		for (Key_t i = 0; i < storage_size; i++) {
			auto it = list.cbegin();
			assert((*it) == storage[i]);
			assert(list.pop_front() == &storage[i]);
		}
		assert(list.pop_front() == nullptr);
		
		assert(list.size() == 0);
		test_sanity();
	}
	
	void test_pop_back() { 
		assert(list.size() == 0);
		
		for (Key_t i = 0; i < storage_size; i++) {
			list.push_front(storage[i]);
		}
		assert(list.size() == storage_size);
		
		for (Key_t i = 0; i < storage_size; i++) {
			auto it = list.crbegin();
			assert((*it) == storage[i]);
			assert(list.pop_back() == &storage[i]);
		}
		assert(list.pop_back() == nullptr);
		
		assert(list.size() == 0);
		test_sanity();
	}
	
	void test_insert_before() { 
		Key_t index = 0;
		assert(list.size() == 0);
		
		list.push_front(storage[0]);
		for (Key_t i = 1; i < storage_size; i++) {
			assert(list.insert_before(storage[i-1], storage[i]));
		}
		assert(list.size() == storage_size);
		
		for (auto it = list.crbegin(); it != list.crend(); ++it) {
			assert((*it) == storage[index++]);
		}
		
		list.reset();
		assert(list.size() == 0);
		test_sanity();
	}
	
	void test_insert_after() { 
		Key_t index = 0;
		assert(list.size() == 0);
		
		list.push_front(storage[0]);
		for (Key_t i = 1; i < storage_size; i++) {
			assert(list.insert_after(storage[i-1], storage[i]));
		}
		assert(list.size() == storage_size);
		
		for (auto it = list.cbegin(); it != list.cend(); ++it) {
			assert((*it) == storage[index++]);
		}
		
		list.reset();
		assert(list.size() == 0);
		test_sanity();
	}

	
	void test_remove() { 
		assert(list.size() == 0);
		
		for (Key_t i = 0; i < storage_size; i++) {
			assert(list.push_front(storage[i]));
		}
		assert(list.size() == storage_size);
		
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
	
	void dump() const noexcept
	{
		std::cout << "list has " << list.size() << " elements \n";
		for (auto it = list.cbegin(); it != list.cend(); ++it) {
//			std::cout << (*it).value << " ";
		}
		std::cout << "\n";
	}
	
	template <typename T>
	static void dump(const IntrusiveList<T>& list) noexcept
	{
		std::cout << "list has " << list.size() << " elements \n";
		for (auto it = list.cbegin(); it != list.cend(); ++it) {
			std::cout << *it << " ";
		}
		std::cout << "\n";
	}

};

#endif /* INTRUSIVELISTTEST_H */

