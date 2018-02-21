#ifndef LRU_CACHE_H
#define LRU_CACHE_H

#include <assert.h>
#include <cstdio>
#include <bits/allocator.h>

#include "IntrusiveList.h"
#include "IntrusiveMap.h"

template <typename K, typename V>
class LruCache {
	
	friend class LruCacheTest;

	struct Data : public IntrusiveMap<K, Data>::Hook, IntrusiveList<Data>::Hook {
		V value;
		Data() : value() {}
		Data(V v) : value(v) {}
		bool operator==(const Data& data) const {
			return value == data.value;
		}
	};
	
public:
	typedef Data Data_t;
	typedef IntrusiveMap<K, Data> Map_t;
	typedef IntrusiveList<Data> List_t;
	typedef typename Map_t::Bucket Bucket_t;

private:
	const size_t storage_size;
	Data* storage;
	const size_t bucket_list_size;
	Bucket_t* bucket_list;
	Map_t map;
	List_t list_cached;
	List_t list_freed;
	
public:
	
	LruCache(unsigned storage_size, float load_factor) noexcept
		: storage_size(storage_size),
			storage(new Data_t[storage_size]),
			bucket_list_size((storage_size / load_factor) + 1),
			bucket_list(new Bucket_t[bucket_list_size]),
			map(bucket_list, bucket_list_size),
			list_cached(),
			list_freed()
		{
		for (unsigned i = 0; i < storage_size; i++) {
			list_freed.push_back(storage[i]);
		}

	}
	
	LruCache(const LruCache&) = delete;
	LruCache(LruCache&&) = delete;
	
	LruCache operator=(const LruCache&) = delete;
	LruCache operator=(LruCache&&) = delete;
	
	~LruCache() noexcept {
		delete [] storage;
		delete [] bucket_list;
	}
	
	bool put(K key, const V& value) noexcept
	{
		auto it = map.find(key);
		if(it == nullptr){
			if(list_freed.size() == 0){
				cycle(value);
			} else {
				push_back(key, value);
			}
		} else {
			update_value(*it, value);
		}
		return false;
	}
	
	size_t mem_used() const noexcept {
		return storage_size * sizeof(Data) + bucket_list_size * sizeof(Bucket_t);
	}
	
	void test(){
		printf("<LruCache>...\n");
		printf("sizeof(Data)=%zu\n", sizeof(Data));
		printf("sizeof(Bucket_t)=%zu\n", sizeof(Bucket_t));
		printf("storage_size=%zu\n", storage_size);
		printf("bucket_list_size=%zu\n", bucket_list_size);
		printf("memory used %zu Kb\n", mem_used() / (1024));
	}
	
	void dump()
	{
		std::cout << "map has " << map.size() << " elements \n";
		for (size_t bucket = 0; bucket < map.bucket_list_size; ++bucket) {
			std::cout << "B[" << bucket << "] ";
			for (auto it = map.cbegin(bucket); it != map.cend(); ++it) {
				std::cout << (*it).value << " ";
			}
			std::cout << "\n";
		}
		std::cout << "cached list has " << list_cached.size() << " elements \n";
		for (auto it = list_cached.cbegin(); it != list_cached.cend(); ++it) {
			std::cout << (*it).value << " ";
		}
		std::cout << "\n";
		std::cout << "freed list has " << list_freed.size() << " elements \n";
		for (auto it = list_freed.cbegin(); it != list_freed.cend(); ++it) {
			std::cout << (*it).value << " ";
		}
		std::cout << "\n";

	}
	
private:
	
	inline void cycle(const V& value) noexcept
	{
		Data_t& last = *(list_cached.begin());
		list_cached.pop_front();
		last.value = value;
		list_cached.push_back(last);
	}
	
	inline void push_back(K key, const V& value) noexcept
	{
		Data_t& freed = *(list_freed.begin());
		list_freed.pop_front();
		freed.value = value;
		list_cached.push_back(freed);
		map.put(key, freed);
	}
	
	inline void update_value(Data_t& cell, const V& value) noexcept
	{
		cell.value = value;
		list_cached.remove(cell);
		list_cached.push_back(cell);
	}
		

};

#endif /* LRU_CACHE_H */

