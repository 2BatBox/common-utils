#ifndef INTRUSIVE_MAP_TEST_H
#define INTRUSIVE_MAP_TEST_H

#include <assert.h>
#include <cstdio>

#include "IntrusiveMap.h"

class IntrusiveMapTest {

	typedef unsigned Key_t;
	typedef unsigned Value_t;
	
	template <typename K, typename V>
	struct Data : public IntrusiveMapHook<K, Data<K, V> > {
		V value;
		Data() : value() {}
		Data(V v) : value(v) {}
		bool operator==(const Data& data) const {
			return value == data.value;
		}
	};
	
	typedef Data<Key_t, Value_t> Data_t;
	typedef IntrusiveMap<Key_t, Data_t> Map_t;
	typedef Map_t::Bucket_t Bucket_t;
	
	const size_t storage_size;
	Data_t* storage;
	const size_t bucket_list_size;
	Map_t map;
	
public:
	
	IntrusiveMapTest(unsigned storage_size, float load_factor)
		: storage_size(storage_size),
			storage(new Data_t[storage_size]),
			bucket_list_size((storage_size / load_factor) + 1),
			map(bucket_list_size)
		{
			
			if(not map.allocate())
				throw std::logic_error("Cannot allocate IntrusiveMap instance");
			for (unsigned i = 0; i < storage_size; i++) {
				storage[i].value = i;
			}
	}
	
	IntrusiveMapTest(const IntrusiveMapTest&) = delete;
	IntrusiveMapTest(IntrusiveMapTest&&) = delete;
	
	IntrusiveMapTest operator=(const IntrusiveMapTest&) = delete;
	IntrusiveMapTest operator=(IntrusiveMapTest&&) = delete;
	
	~IntrusiveMapTest(){
		delete [] storage;
	}
	
	size_t mem_used(){
		return storage_size * sizeof(Data_t) + bucket_list_size * sizeof(Bucket_t);
	}
	
	void test(){
		printf("<IntrusiveMapTest>...\n");
		printf("sizeof(Data_t)=%zu\n", sizeof(Data_t));
		printf("sizeof(Bucket_t)=%zu\n", sizeof(Bucket_t));
		printf("storage_size=%zu\n", storage_size);
		printf("bucket_list_size=%zu\n", bucket_list_size);
		printf("memory used %zu Kb\n", mem_used() / (1024));
		test_raii();
		test_put();
		test_remove();
		test_find();
	}
	
	void test_sanity(){
		for (unsigned i = 0; i < storage_size; i++) {
			assert(not storage[i].im_linked);
		}
	}
	
	void test_raii() { 
		assert(map.size() == 0);
		
		for (Key_t i = 0; i < storage_size; i++) {
			assert(map.put(i, storage[i]));
		}
		
		assert(map.size() == storage_size);
		
		Map_t tmp_map(std::move(map));
		assert(map.size() == 0);
		assert(tmp_map.size() == storage_size);
		
		for (Key_t i = 0; i < storage_size; i++) {
			auto it = tmp_map.find(i);
			assert(it);
			assert((*it) == storage[i]);
		}
		
		map = std::move(map);
		map = std::move(tmp_map);
		std::swap(map, tmp_map);
		std::swap(map, tmp_map);
		
		map.reset();
		assert(map.size() == 0);
		test_sanity();
	}

	void test_put() { 
		assert(map.size() == 0);
		
		for (Key_t i = 0; i < storage_size; i++) {
			assert(map.put(i, storage[i]));
		}
		assert(map.size() == storage_size);
		
		for (Key_t i = 0; i < storage_size; i++) {
			auto it = map.find(i);
			assert(it);
			assert((*it) == storage[i]);
		}
		
		map.reset();
		assert(map.size() == 0);
		test_sanity();
	}
	
	void test_find() { 
		assert(map.size() == 0);
		
		Key_t half = storage_size / 2;
		for (Key_t i = 0; i < half; i++) {
			assert(map.put(i, storage[i]));
		}
		assert(map.size() == half);
		
		for (Key_t i = 0; i < storage_size; i++) {
			auto it = map.find(i);
			if(i < half){
				assert(it);
				assert((*it) == storage[i]);
			} else {
				assert(it == nullptr);
			}
		}
		
		map.reset();
		assert(map.size() == 0);
		test_sanity();
	}
	
	void test_remove() { 
		assert(map.size() == 0);
		
		for (Key_t i = 0; i < storage_size; i++) {
			assert(map.put(i, storage[i]));
		}
		assert(map.size() == storage_size);
		
		Key_t half = storage_size / 2;
		for (Key_t i = half; i < storage_size; i++) {
			assert(map.remove(i));
			assert(not map.remove(i));
		}
		
		for (Key_t i = 0; i < half; i++) {
			assert(map.remove(i));
			assert(not map.remove(i));
		}
		
		assert(map.size() == 0);
		test_sanity();
	}
	
	void dump()
	{
		std::cout << "map has " << map.size() << " elements \n";
		for (size_t bucket = 0; bucket < map.buckets(); ++bucket) {
			std::cout << "B[" << bucket << "] ";
			for (auto it = map.cbegin(bucket); it != map.cend(); ++it) {
				std::cout << (*it).value << " ";
			}
			std::cout << "\n";
		}
	}
	
	template <typename K, typename V>
	static void dump(const IntrusiveMap<K,V>& map)
	{
		std::cout << "map has " << map.size() << " elements \n";
		for (size_t bucket = 0; bucket < map.buckets(); ++bucket) {
			std::cout << "B[" << bucket << "] ";
			for (auto it = map.cbegin(bucket); it != map.cend(); ++it) {
				std::cout << *it << " ";
			}
			std::cout << "\n";
		}
	}

};

#endif /* INTRUSIVE_MAP_TEST_H */

