#ifndef TIME_CACHE_H
#define TIME_CACHE_H

#include <assert.h>
#include <cstdio>
#include <ctime>
#include <bits/allocator.h>

#include "../intrusive/List.h"
#include "../intrusive/Map.h"

namespace cache {

template <typename K, typename V>
struct TimeCacheData: public intrusive::ListHook<TimeCacheData<K, V> >, intrusive::MapHook<K, TimeCacheData<K, V> > {
	typedef K Key_t;
	typedef V Value_t;
	V value;
	std::time_t time;

	TimeCacheData(): value(), time() { }

	TimeCacheData(const V& v): value(v), time() { }

	bool operator==(const TimeCacheData& data) const {
		return value == data.value;
	}
};

template <
typename TimeCacheData_t,
typename H = std::hash<typename TimeCacheData_t::Key_t>,
typename SA = std::allocator<TimeCacheData_t>,
typename BA = std::allocator<intrusive::MapBucket<TimeCacheData_t> >
>
class TimeCache {
	friend class TimeCacheTest;

	typedef typename TimeCacheData_t::Key_t Key_t;
	typedef typename TimeCacheData_t::Value_t Value_t;
	typedef intrusive::List<TimeCacheData_t > List_t;
	typedef intrusive::Map<Key_t, TimeCacheData_t, H, BA> Map_t;
	typedef typename Map_t::Bucket_t Bucket_t;

	const size_t cache_capacity;
	TimeCacheData_t* storage;
	Map_t map;
	List_t list_cached;
	List_t list_freed;
	SA allocator;

public:

	TimeCache(unsigned capacity, float load_factor) noexcept
	: cache_capacity(capacity),
	storage(nullptr),
	map((capacity / load_factor) + 1),
	list_cached(),
	list_freed(),
	allocator() { }

public:

	TimeCache(const TimeCache&) = delete;
	TimeCache& operator=(const TimeCache&) = delete;

	TimeCache(TimeCache&& rv) = delete;
	TimeCache& operator=(TimeCache&&) = delete;

	virtual ~TimeCache() noexcept {
		destroy();
	}

	bool allocate() {
		if (storage)
			return false;

		storage = allocator.allocate(cache_capacity);
		if (storage == nullptr)
			return false;

		TimeCacheData_t empty;
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
	 * @return Returns true if a new record has been added or updated,
	 * false - cache has no space to append a new record.
	 */
	bool put(Key_t key, const Value_t& value) noexcept {
		std::time_t time = std::time(nullptr);
		TimeCacheData_t* cell = map.find(key);
		if (cell) {
			cell->value = value;
			cell->time = time;
			list_cached.remove(*cell);
			list_cached.push_back(*cell);
			return true;
		} else {
			TimeCacheData_t* freed;
			if (list_freed.size()) {
				freed = list_freed.pop_front();
				freed->value = value;
				freed->time = time;
				list_cached.push_back(*freed);
				map.put(key, *freed);
				return true;
			}
		}
		return false;
	}

	bool get(Key_t key, Value_t& value) const noexcept {
		const TimeCacheData_t* cell = map.find(key);
		if (cell) {
			value = cell->value;
			return true;
		}
		return false;
	}

	bool get_refresh(Key_t key, Value_t& value) noexcept {
		TimeCacheData_t* cell = map.find(key);
		if (cell) {
			value = cell->value;
			cell->time = std::time(nullptr);
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

	/**
	 * |R|R|R|R|R|R|R|R|V|V|V|V|V|V|V|
	 *                 ^-----time----^
	 * 
	 * @param key_row
	 * @param value_row
	 * @param row_size
	 * @param time 
	 * @return 
	 */
	size_t timeout(Key_t* key_row, Value_t* value_row, size_t row_size, std::time_t time) noexcept {
		size_t result = 0;
		time = std::time(nullptr) - time;
		while (result < row_size) {
			auto it = list_cached.begin();
			if (it == list_cached.end() || (*it).time > time)
				break;
			TimeCacheData_t* cell = list_cached.pop_front();
			key_row[result] = cell->im_key;
			value_row[result] = cell->value;
			map.remove(cell->im_key);
			list_freed.push_back(*cell);
			result++;
		}
		return result;
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
		return cache_capacity * sizeof (TimeCacheData_t) + map.buckets() * sizeof (Bucket_t);
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

#endif /* TIME_CACHE_H */

