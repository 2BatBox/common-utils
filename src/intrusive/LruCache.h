#ifndef LRU_CACHE_H
#define LRU_CACHE_H

#include <assert.h>
#include <cstdio>
#include <bits/allocator.h>

#include "IntrusiveList.h"
#include "IntrusiveMap.h"

template <typename K, typename V, typename H = DummyHasher<K> >
class LruCache {
	
	friend class LruCacheTest;

	struct Data : public IntrusiveMap<K, Data, H>::Hook, IntrusiveList<Data>::Hook {
		V value;
		Data() : value() {}
		Data(V v) : value(v) {}
		bool operator==(const Data& data) const {
			return value == data.value;
		}
	};
	
public:
	typedef Data Data_t;
	typedef IntrusiveMap<K, Data, H> Map_t;
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
	
	void put(K key, const V& value) noexcept
	{
		auto it = map.find(key);
		if(it == nullptr){
			if(list_freed.size()){
				push_back(key, value);
			} else {
				cycle(key, value);
			}
		} else {
			update_value(*it, value);
		}
	}
	
	bool get(K key, V& value) const noexcept {
		auto it = map.find(key);
		if(it != nullptr){
			value = (*it).value;
			return true;
		}
		return false;
	}
	
	bool remove(K key) noexcept {
		auto it = map.find(key);
		if(it != nullptr){
			map.remove(key);
			list_cached.remove(*it);
			list_freed.push_back(*it);
			return true;
		}
		return false;
	}
	
	void reset() noexcept {
		map.reset();
		list_cached.reset();
		for (unsigned i = 0; i < storage_size; i++) {
			list_freed.push_back(storage[i]);
		}
	}
	
	size_t size() const noexcept {
		return map.size();
	}
	
	size_t mem_used() const noexcept {
		return storage_size * sizeof(Data) + bucket_list_size * sizeof(Bucket_t);
	}
	
private:
	
	inline void cycle(K key, const V& value) noexcept
	{
		Data_t& last = *(list_cached.begin());
		list_cached.pop_front();
		last.value = value;
		list_cached.push_back(last);
		map.remove(last.im_key);
		map.put(key, last);
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

