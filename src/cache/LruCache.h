#ifndef CACHE_LRU_CACHE_H
#define CACHE_LRU_CACHE_H

#include "../intrusive/List.h"
#include "../intrusive/Map.h"

#include <assert.h>
#include <cstdio>
#include <bits/allocator.h>

namespace cache {

template <typename K, typename V>
struct LruCacheData: public intrusive::ListHook<LruCacheData<K, V> >, intrusive::MapHook<K, LruCacheData<K, V> > {
	typedef K Key_t;
	typedef V Value_t;
	V value;

	LruCacheData(): value() { }

	LruCacheData(const V& v): value(v) { }

	bool operator==(const LruCacheData& data) const {
		return value == data.value;
	}
};

template <
typename LruCacheData_t,
typename H = std::hash<typename LruCacheData_t::Key_t>,
typename SA = std::allocator<LruCacheData_t>,
typename BA = std::allocator<intrusive::MapBucket<LruCacheData_t> >
>
class LruCache {
	friend class LruCacheTest;

	typedef typename LruCacheData_t::Key_t Key_t;
	typedef typename LruCacheData_t::Value_t Value_t;
	typedef intrusive::List<LruCacheData<Key_t, Value_t> > List_t;
	typedef intrusive::Map<Key_t, LruCacheData<Key_t, Value_t>, H, BA> Map_t;
	typedef typename Map_t::Bucket_t Bucket_t;

	const size_t cache_capacity;
	LruCacheData_t* storage;
	Map_t map;
	List_t list_cached;
	List_t list_freed;
	SA allocator;

public:

	LruCache(unsigned capacity, float load_factor) noexcept
	: cache_capacity(capacity),
	storage(nullptr),
	map((capacity / load_factor) + 1),
	list_cached(),
	list_freed(),
	allocator() { }

public:

	LruCache(const LruCache&) = delete;
	LruCache& operator=(const LruCache&) = delete;

	LruCache(LruCache&& rv) = delete;
	LruCache& operator=(LruCache&&) = delete;

	virtual ~LruCache() noexcept {
		destroy();
	}

	bool allocate() {
		if (storage)
			return false;

		storage = allocator.allocate(cache_capacity);
		if (storage == nullptr)
			return false;

		LruCacheData_t empty;
		for (unsigned i = 0; i < cache_capacity; i++) {
			allocator.construct(storage + i, empty);
			list_freed.push_back(storage[i]);
		}

		if (not map.allocate()) {
			destroy();
			return false;
		}

		return true;
	}

	/**
	 * @param key
	 * @param value
	 * @return Returns true if a new record has been added,
	 * false - cache has a record with the key, so this record has been updated.
	 */
	bool put(Key_t key, const Value_t& value) noexcept {
		bool result = true;
		LruCacheData_t* cell = map.find(key);
		if (cell) {
			result = false;
			cell->value = value;
			list_cached.remove(*cell);
			list_cached.push_back(*cell);
		} else {
			LruCacheData_t* freed;
			if (list_freed.size())
				freed = list_freed.pop_front();
			else {
				freed = list_cached.pop_front();
				map.remove(freed->im_key);
			}
			freed->value = value;
			list_cached.push_back(*freed);
			map.put(key, *freed);
		}
		return result;
	}

	bool get(Key_t key, Value_t& value) const noexcept {
		const LruCacheData_t* cell = map.find(key);
		if (cell) {
			value = cell->value;
			return true;
		}
		return false;
	}

	bool get_refresh(Key_t key, Value_t& value) noexcept {
		LruCacheData_t* cell = map.find(key);
		if (cell) {
			value = cell->value;
			list_cached.remove(*cell);
			list_cached.push_back(*cell);
			return true;
		}
		return false;
	}

	bool remove(Key_t key) noexcept {
		auto it = map.find(key);
		if (it != nullptr) {
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

	size_t storage_bytes() const noexcept {
		return cache_capacity * sizeof (LruCacheData_t) + map.buckets() * sizeof (Bucket_t);
	}

private:

	void destroy() noexcept {
		if (storage) {

			list_freed.reset();
			list_cached.reset();
			map.reset();

			for (size_t i = 0; i < cache_capacity; i++) {
				allocator.destroy(storage + i);
			}
			allocator.deallocate(storage, cache_capacity);
			storage = nullptr;
		}
	}

};

}; // namespace cache

#endif /* CACHE_LRU_CACHE_H */

