#ifndef CACHE_LRUCACHE_H
#define CACHE_LRUCACHE_H

#include "../intrusive/List.h"
#include "../intrusive/Map.h"

#include <bits/allocator.h>

namespace cache {

template <typename K, typename V>
struct LruCacheNode : public intrusive::ListHook<LruCacheNode<K, V> >, intrusive::MapHook<K, LruCacheNode<K, V> > {
	using Key_t = K;
	using Value_t = V;
	V value;

	LruCacheNode() : value() { }

	LruCacheNode(const LruCacheNode&) = delete;
	LruCacheNode& operator=(const LruCacheNode&) = delete;

	LruCacheNode(LruCacheNode&&) = delete;
	LruCacheNode& operator=(LruCacheNode&&) = delete;

	bool operator==(const LruCacheNode& data) const noexcept {
		return value == data.value;
	}
};

template <
typename Node_t,
typename H = std::hash<typename Node_t::Key_t>,
typename SA = std::allocator<Node_t>,
typename BA = std::allocator<intrusive::MapBucket<Node_t> >
>
class LruCache {
	friend class TestLruCache;

	using Key_t = typename Node_t::Key_t;
	using Value_t = typename Node_t::Value_t;
	using List_t = intrusive::List<Node_t>;
	using Map_t = intrusive::Map<Key_t, Node_t, H, BA>;
	using Bucket_t = typename Map_t::Bucket_t;

	const size_t m_capacity;
	Node_t* m_storage;
	Map_t m_map;
	List_t m_list_cached;
	List_t m_list_freed;
	SA m_allocator;

public:
	using Iterator_t = typename Map_t::Iterator_t;
	using ConstIterator_t = typename Map_t::ConstIterator_t;

	LruCache(unsigned capacity, float load_factor) noexcept
	: m_capacity(capacity),
	m_storage(nullptr),
	m_map((capacity / load_factor) + 1),
	m_list_cached(),
	m_list_freed(),
	m_allocator() { }

	LruCache(const LruCache&) = delete;
	LruCache& operator=(const LruCache&) = delete;

	LruCache(LruCache&& rv) = delete;
	LruCache& operator=(LruCache&&) = delete;

	virtual ~LruCache() noexcept {
		destroy();
	}

	/**
	 * Allocate the node storage.
	 * @return true - if the cache has been allocated successfully.
	 */
	bool allocate() noexcept {
		if (m_storage)
			return false;

		m_storage = m_allocator.allocate(m_capacity);
		if (m_storage == nullptr)
			return false;

		for (unsigned i = 0; i < m_capacity; i++) {
			m_allocator.construct(m_storage + i);
			m_list_freed.push_back(m_storage[i]);
		}

		if (not m_map.allocate()) {
			destroy();
			return false;
		}
		return true;
	}

	Iterator_t push_back(const Key_t& key, bool& recycled) noexcept {
		recycled = false;
		auto it = m_map.find(key);
		if (it) {
			update(it);
			recycled = true;
		} else {
			Node_t* freed;
			if (m_list_freed.size()) {
				freed = m_list_freed.pop_front();
			} else {
				freed = m_list_cached.pop_front();
				auto freed_it = m_map.find(freed->im_key);
				m_map.remove(freed_it);
				recycled = true;
			}
			m_list_cached.push_back(*freed);
			it = m_map.put(key, *freed);
		}
		return it;
	}

	Iterator_t peek_front() noexcept {
		Iterator_t result(m_list_cached.head());
		return result;
	}

	Iterator_t pop_front() noexcept {
		Iterator_t result(m_list_cached.head());
		if (result) {
			remove(result);
		}
		return result;
	}

	inline ConstIterator_t find(const Key_t& key) const noexcept {
		return m_map.find(key);
	}

	inline Iterator_t find(const Key_t& key) noexcept {
		return m_map.find(key);
	}

	inline void update(Iterator_t it) noexcept {
		m_list_cached.remove(*it);
		m_list_cached.push_back(*it);
	}

	inline void remove(Iterator_t it) noexcept {
		m_map.remove(it);
		m_list_cached.remove(*it);
		m_list_freed.push_back(*it);
	}

	void clear() noexcept {
		m_map.clear();
		m_list_cached.clear();
		for (unsigned i = 0; i < m_capacity; i++) {
			m_list_freed.push_back(m_storage[i]);
		}
	}

	inline size_t capacity() const noexcept {
		return m_capacity;
	}

	inline size_t size() const noexcept {
		return m_map.size();
	}

	inline size_t storage_bytes() const noexcept {
		return m_capacity * sizeof (Node_t) + m_map.buckets() * sizeof (Bucket_t);
	}

	inline Iterator_t end() noexcept {
		return Iterator_t();
	}

	inline ConstIterator_t cend() const noexcept {
		return ConstIterator_t();
	}

private:

	void destroy() noexcept {
		if (m_storage) {
			m_list_freed.clear();
			m_list_cached.clear();
			m_map.clear();
			for (size_t i = 0; i < m_capacity; i++) {
				m_allocator.destroy(m_storage + i);
			}
			m_allocator.deallocate(m_storage, m_capacity);
			m_storage = nullptr;
		}
	}

};

}; // namespace cache

#endif /* CACHE_LRUCACHE_H */

