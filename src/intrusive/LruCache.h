#ifndef LRU_CACHE_H
#define LRU_CACHE_H

#include <assert.h>
#include <cstdio>
#include <bits/allocator.h>

#include "IntrusiveList.h"
#include "IntrusiveMap.h"

template <typename K, typename V, typename H = std::hash<K> >
struct Data : public IntrusiveMap<K, Data<K,V,H>, H>::Hook, IntrusiveList<Data<K,V,H> >::Hook {
	
	typedef K Key_t;
	typedef V Value_t;
	typedef IntrusiveList<Data> List_t;
	typedef IntrusiveMap<K, Data<K,V,H>, H> Map_t;
	typedef typename Map_t::Bucket_t Bucket_t;
	
	V value;
	Data() : value() {}
	Data(V v) : value(v) {}
	bool operator==(const Data& data) const {
		return value == data.value;
	}
};


template <typename Data_t>
class LruCache {
	
	friend class LruCacheTest;

	typedef typename Data_t::Key_t K;
	typedef typename Data_t::Value_t V;
	typedef typename Data_t::List_t List_t;
	typedef typename Data_t::Map_t Map_t;
	typedef typename Data_t::Bucket_t Bucket_t;
	
public:

	const unsigned storage_size;
	const unsigned bucket_list_size;
	
private:
	Data_t* storage;
	Bucket_t* bucket_list;
	Map_t map;
	List_t list_cached;
	List_t list_freed;
	
	LruCache(unsigned storage_size, float load_factor) noexcept
		: storage_size(storage_size),
			bucket_list_size((storage_size / load_factor) + 1),
			storage(nullptr),
			bucket_list(nullptr),
			map(bucket_list_size),
			list_cached(),
			list_freed()
		{
		for (unsigned i = 0; i < storage_size; i++) {
			list_freed.push_back(storage[i]);
		}
	}
	
public:
	
	LruCache(const LruCache&) = delete;
	LruCache operator=(const LruCache&) = delete;
	
	LruCache(LruCache&&) = delete;
	LruCache operator=(LruCache&&) = delete;
	
	~LruCache() noexcept {
		delete [] storage;
		delete [] bucket_list;
	}
	
	bool aloocate(){
		if(storage)
			return false;
	}
	
	void put(K key, const V& value) noexcept
	{
		Data_t* cell = map.find(key);
		if(cell){
			cell->value = value;
			list_cached.remove(*cell);
			list_cached.push_back(*cell);
		} else {
			Data_t* freed;
			if(list_freed.size())
				freed = list_freed.pop_front();
			else {
				freed = list_cached.pop_front();
				map.remove(freed->im_key);
			}
			
			freed->value = value;
			list_cached.push_back(*freed);
			map.put(key, *freed);
		}
	}
	
	bool get(K key, V& value) const noexcept {
		const Data_t* cell = map.find(key);
		if(cell){
			value = cell->value;
			return true;
		}
		return false;
	}
	
	bool get_refresh(K key, V& value) noexcept {
		Data_t* cell = map.find(key);
		if(cell){
			value = cell->value;
			list_cached.remove(*cell);
			list_cached.push_back(*cell);
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
		return storage_size * sizeof(Data_t) + bucket_list_size * sizeof(Bucket_t);
	}
	
};

#endif /* LRU_CACHE_H */

