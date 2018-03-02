#ifndef CACHE_TIMER_QUEUE_H
#define CACHE_TIMER_QUEUE_H

#include <assert.h>
#include <cstdio>
#include <ctime>
#include <bits/allocator.h>

#include "../intrusive/List.h"
#include "../intrusive/Map.h"

namespace cache {

template <typename K, typename V>
struct TimerQueueData: public intrusive::ListHook<TimerQueueData<K, V> >, intrusive::MapHook<K, TimerQueueData<K, V> > {
	typedef K Key_t;
	typedef V Value_t;
	V value;
	std::time_t time;

	TimerQueueData(): value(), time() { }

	TimerQueueData(const V& v): value(v), time() { }

	bool operator==(const TimerQueueData& data) const {
		return value == data.value;
	}
};

template <
typename TimerQueueData_t,
typename H = std::hash<typename TimerQueueData_t::Key_t>,
typename SA = std::allocator<TimerQueueData_t>,
typename BA = std::allocator<intrusive::MapBucket<TimerQueueData_t> >
>
class TimerQueue {
	friend class TimerQueueTest;

	typedef typename TimerQueueData_t::Key_t Key_t;
	typedef typename TimerQueueData_t::Value_t Value_t;
	typedef intrusive::List<TimerQueueData_t > List_t;
	typedef intrusive::Map<Key_t, TimerQueueData_t, H, BA> Map_t;
	typedef typename Map_t::Bucket_t Bucket_t;

	const size_t cache_capacity;
	TimerQueueData_t* storage;
	Map_t map;
	List_t list_cached;
	List_t list_freed;
	SA allocator;

public:

	TimerQueue(unsigned capacity, float load_factor) noexcept
	: cache_capacity(capacity),
	storage(nullptr),
	map((capacity / load_factor) + 1),
	list_cached(),
	list_freed(),
	allocator() { }

public:

	TimerQueue(const TimerQueue&) = delete;
	TimerQueue& operator=(const TimerQueue&) = delete;

	TimerQueue(TimerQueue&& rv) = delete;
	TimerQueue& operator=(TimerQueue&&) = delete;

	virtual ~TimerQueue() noexcept {
		destroy();
	}

	bool allocate() {
		if (storage)
			return false;

		storage = allocator.allocate(cache_capacity);
		if (storage == nullptr)
			return false;

		TimerQueueData_t empty;
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
	 * false - the queue has no space to append a new record.
	 */
	bool push_back(Key_t key, const Value_t& value) noexcept {
		std::time_t time = std::time(nullptr);
		TimerQueueData_t* cell = map.find(key);
		if (cell) {
			cell->value = value;
			cell->time = time;
			list_cached.remove(*cell);
			list_cached.push_back(*cell);
			return true;
		} else {
			TimerQueueData_t* freed;
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
	
	/**
	 * head                         tail 
	 * |--cells to pop-|             |
	 * |C|C|C|C|C|C|C|C|C|C|C|C|C|C|C|
	 *                 ^---timeout---^
	 *                               |
	 *                              now
	 * @param key_row
	 * @param value_row
	 * @param row_size
	 * @param time 
	 * @return 
	 */
	bool pop_front(Key_t& key, Value_t& value, int timeout) noexcept {
		std::time_t time = std::time(nullptr) - timeout;
		auto it = list_cached.cbegin();
		if(it != list_cached.cend() && (*it).time < time){
			TimerQueueData_t* cell = list_cached.pop_front();
			key = cell->im_key;
			value = cell->value;
			map.remove(cell->im_key);
			list_freed.push_back(*cell);
			return true;
		}
		return false;
	}

	bool get(Key_t key, Value_t& value) const noexcept {
		const TimerQueueData_t* cell = map.find(key);
		if (cell) {
			value = cell->value;
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
		return cache_capacity * sizeof (TimerQueueData_t) + map.buckets() * sizeof (Bucket_t);
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

#endif /* CACHE_TIMER_QUEUE_H */

