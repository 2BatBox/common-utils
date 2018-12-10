#ifndef STORAGE_TIMEDQUEUE_H
#define STORAGE_TIMEDQUEUE_H

#include "TimedQueueStat.h"
#include "../intrusive_pool/HashQueuePool.h"
#include "../dpdk/Allocator.h"

#include <cstdint>
#include <cstdio>
#include <ctime>

namespace storage {

template <typename K, typename V>
struct TimedQueueNode : public intrusive::LinkedListHook<TimedQueueNode<K, V> >, intrusive::HashMapHook<K, TimedQueueNode<K, V> > {
	template <typename Tmp1, typename Tmp2>
	friend class TimedQueue;
private:
	std::time_t time;
public:
	using Key_t = K;
	V value;

	TimedQueueNode() : value() { }

	TimedQueueNode(const TimedQueueNode&) = delete;
	TimedQueueNode& operator=(const TimedQueueNode&) = delete;

	TimedQueueNode(TimedQueueNode&&) = delete;
	TimedQueueNode& operator=(TimedQueueNode&&) = delete;

	bool operator==(const TimedQueueNode & data) const noexcept {
		return value == data.value;
	}
};

template <typename K>
struct TimedQueueEmptyNode : public intrusive::LinkedListHook<TimedQueueEmptyNode<K> >, intrusive::HashMapHook<K, TimedQueueEmptyNode<K> > {
	template <typename Tmp1, typename Tmp2>
	friend class TimedQueue;
private:
	std::time_t time;
public:
	using Key_t = K;

	TimedQueueEmptyNode() = default;

	TimedQueueEmptyNode(const TimedQueueEmptyNode&) = delete;
	TimedQueueEmptyNode& operator=(const TimedQueueEmptyNode&) = delete;

	TimedQueueEmptyNode(TimedQueueEmptyNode&&) = delete;
	TimedQueueEmptyNode& operator=(TimedQueueEmptyNode&&) = delete;

};

template <
typename Node_t,
typename H = std::hash<typename Node_t::Key_t>
>
class TimedQueue {
	friend class TestTimedQueue;

	using Pool_t = intrusive::HashQueuePool<Node_t, H, dpdk::Allocator<Node_t>, dpdk::Allocator<intrusive::HashMapBucket<Node_t> > >;
	Pool_t m_pool;
	const size_t m_capacity;
	std::time_t m_push_time;
	TimedQueueStat m_stat;

public:

	using Key_t = typename Node_t::Key_t;
	using Iterator_t = typename Pool_t::Iterator_t;

	TimedQueue(size_t capacity, float load_factor) noexcept
	: m_pool(capacity, load_factor)
	, m_capacity(capacity)
	, m_push_time(0)
	, m_stat() { }

	int allocate() noexcept {
		return m_pool.allocate();
	}

	Iterator_t push_back(const Key_t& key) noexcept {
		std::time_t time = std::time(nullptr);
		assert(time >= m_push_time); // TODO: 
		auto it = m_pool.push_back(key);
		if (it) {
			it->time = time;
			m_push_time = time;
		}
		return it;
	}

	Iterator_t pop_front(unsigned sec) noexcept {
		Iterator_t result;
		auto it = m_pool.peek_front();
		if (it) {
			std::time_t now = std::time(nullptr);
			if (std::difftime(now, it->time) >= sec) {
				m_pool.remove(it);
				result = it;
			}
		}
		return result;
	}

	Iterator_t remove(const Key_t& key) noexcept {
		auto it = m_pool.find(key);
		if (it) {
			m_pool.remove(it);
		}
		return it;
	}

	inline void remove_all(const Key_t& key) noexcept {
		Iterator_t tmp;
		auto it = m_pool.find(key);
		while (it) {
			tmp = it;
			it.next(key);
			m_pool.remove(tmp);
		}
	}

	void reset() noexcept {
		m_pool.reset();
	}

	inline size_t size() const noexcept {
		return m_pool.size();
	}

	inline size_t storage_bytes() noexcept {
		return m_pool.capacity() * sizeof (Node_t);
	}

	void load(TimedQueueStat& stat) const noexcept {
		stat = m_stat;
		stat.size = m_pool.size();
		stat.capacity = m_capacity;
	}

	inline Iterator_t end() noexcept {
		return m_pool.end();
	}

};

}; // namespace storage

#endif /* STORAGE_TIMEDQUEUE_H */