#ifndef LRU_CACHE_H
#define LRU_CACHE_H

#include <assert.h>
#include <cstdio>
#include <bits/allocator.h>

#include "IntrusiveList.h"
#include "IntrusiveMap.h"

template <typename K, typename V>
struct LruCacheData : public IntrusiveListHook<LruCacheData<K, V> >, IntrusiveMapHook<K, LruCacheData<K, V> > {
	typedef K Key_t;
	typedef V Value_t;
	typedef IntrusiveList<LruCacheData<K,V> > List_t;
	typedef IntrusiveMap<K, LruCacheData<K,V> > Map_t;
	V value;
	LruCacheData() : value() {}
	LruCacheData(const V& v) : value(v) {}
	bool operator==(const LruCacheData& data) const {
		return value == data.value;
	}
};

template <
	typename Data_t,
	typename SA = std::allocator<Data_t>,
	typename BA = std::allocator<IntrusiveMapBucket<Data_t> >,
	typename H = std::hash<typename Data_t::Key_t>
>
class LruCache {
	
	friend class LruCacheTest;

	typedef typename Data_t::Key_t Key_t;
	typedef typename Data_t::Value_t Value_t;
	typedef typename Data_t::List_t List_t;
	typedef typename Data_t::Map_t Map_t;
	typedef typename Map_t::Bucket_t Bucket_t;
	
private:
	size_t cache_capacity;
	Data_t* storage;
	Map_t map;
	List_t list_cached;
	List_t list_freed;
	SA allocator;
	
	LruCache(unsigned capacity, float load_factor) noexcept
		: cache_capacity(capacity),
			storage(nullptr),
			map((capacity / load_factor) + 1),
			list_cached(),
			list_freed(),
			allocator()
		{
	}
	
public:
	
	LruCache(const LruCache&) = delete;
	LruCache& operator=(const LruCache&) = delete;
	
	LruCache(LruCache&& rv) = delete;
	LruCache& operator=(LruCache&&) = delete;
	
	virtual ~LruCache() noexcept {
		destroy();
	}
	
	bool allocate(){
		if(storage)
			return false;
		
		storage = allocator.allocate(cache_capacity);
		if(storage == nullptr)
			return false;
		
		Data_t empty;
		for (unsigned i = 0; i < cache_capacity; i++) {
			allocator.construct(storage + i, empty);
			list_freed.push_back(storage[i]);
		}
		
		return map.allocate();
	}
	
	void put(Key_t key, const Value_t& value) noexcept
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
	
	bool get(Key_t key, Value_t& value) const noexcept {
		const Data_t* cell = map.find(key);
		if(cell){
			value = cell->value;
			return true;
		}
		return false;
	}
	
	bool get_refresh(Key_t key, Value_t& value) noexcept {
		Data_t* cell = map.find(key);
		if(cell){
			value = cell->value;
			list_cached.remove(*cell);
			list_cached.push_back(*cell);
			return true;
		}
		return false;
	}
	
	bool remove(Key_t key) noexcept {
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
		for (unsigned i = 0; i < cache_capacity; i++) {
			list_freed.push_back(storage[i]);
		}
	}
	
	size_t capacity() const noexcept {
		return cache_capacity;
	}
	
	size_t size() const noexcept {
		return map.size();
	}
	
	size_t mem_used() const noexcept {
		return cache_capacity * sizeof(Data_t) + map.buckets() * sizeof(Bucket_t);
	}
	
private:
	
	void destroy() noexcept {
		if (storage && cache_capacity) {
			
			list_freed.reset();
			list_cached.reset();
			map.reset();
			
			for (size_t i = 0; i < cache_capacity; i++) {
				allocator.destroy(storage + i);
			}
			allocator.deallocate(storage, cache_capacity);
			storage = nullptr;
			cache_capacity = 0;
		}
	}
	
};

#endif /* LRU_CACHE_H */

