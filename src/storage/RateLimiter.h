#ifndef STORAGE_RATELIMITER_H
#define STORAGE_RATELIMITER_H

#include "RateLimiterStat.h"
#include "../intrusive_pool/HashQueuePool.h"
#include "../dpdk/Allocator.h"

#include <rte_cycles.h>

#include <cstdint>
#include <cstdio>
#include <ctime>

namespace storage {

template <typename K>
struct RateLimiterNode : public intrusive::LinkedListHook<RateLimiterNode<K> >, intrusive::HashMapHook<K, RateLimiterNode<K> > {
	template <typename Tmp1, typename Tmp2>
	friend class RateLimiter;
private:
	uint64_t time;
public:
	using Key_t = K;

	RateLimiterNode() = default;

	RateLimiterNode(const RateLimiterNode&) = delete;
	RateLimiterNode& operator=(const RateLimiterNode&) = delete;

	RateLimiterNode(RateLimiterNode&&) = delete;
	RateLimiterNode& operator=(RateLimiterNode&&) = delete;

};

template <
typename Node_t,
typename H = std::hash<typename Node_t::Key_t>
>
class RateLimiter {
	friend class TestRateLimiter;

	using Pool_t = intrusive::HashQueuePool<Node_t, H, dpdk::Allocator<Node_t>, dpdk::Allocator<intrusive::HashMapBucket<Node_t> > >;
	Pool_t m_pool;
	const size_t m_capacity;
	uint64_t m_period;

	std::time_t m_push_time;
	RateLimiterStat m_stat;

public:

	using Key_t = typename Node_t::Key_t;
	using Iterator_t = typename Pool_t::Iterator_t;

	RateLimiter(size_t capacity, float load_factor) noexcept
	: m_pool(capacity, load_factor)
	, m_capacity(capacity)
	, m_push_time(0)
	, m_stat() { }

	void set_period(uint64_t period) noexcept {
		m_period = period;
	}

	int allocate() noexcept {
		return m_pool.allocate();
	}

	bool check(const Key_t& key) noexcept {
		bool result = true;
		uint64_t current = rte_rdtsc();
		auto it = m_pool.find(key);
		if (it) {
			if (current - it->time > m_period) {
				m_pool.remove(it);
				it = m_pool.push_back(key);
				it->time = current;
			} else {
				result = false;
			}
		} else {
			if (not m_pool.available()) {
				m_pool.pop_front();
			}
			it = m_pool.push_back(key);
			it->time = current;
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

	void reset() noexcept {
		m_pool.reset();
	}

	inline size_t size() const noexcept {
		return m_pool.size();
	}

	inline Iterator_t end() noexcept {
		return m_pool.end();
	}

	void load(RateLimiterStat& stat) const noexcept {
		stat = m_stat;
		stat.size = m_pool.size();
		stat.capacity = m_capacity;
	}

	inline size_t storage_bytes() noexcept {
		return m_pool.storage_bytes();
	}

};

}; // namespace storage

#endif /* STORAGE_RATELIMITER_H */