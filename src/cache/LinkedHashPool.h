#ifndef CACHE_LINKEDHASHPOOL_H
#define CACHE_LINKEDHASHPOOL_H

#include "../intrusive/List.h"
#include "../intrusive/Map.h"

#include <bits/allocator.h>

namespace cache {

template <typename K>
struct LinkedHashPoolEmptyNode : public intrusive::ListHook<LinkedHashPoolEmptyNode<K> >, intrusive::MapHook<K, LinkedHashPoolEmptyNode<K> > {
	using Key_t = K;

	LinkedHashPoolEmptyNode() noexcept = default;

	LinkedHashPoolEmptyNode(const LinkedHashPoolEmptyNode&) = delete;
	LinkedHashPoolEmptyNode& operator=(const LinkedHashPoolEmptyNode&) = delete;

	LinkedHashPoolEmptyNode(LinkedHashPoolEmptyNode&&) = delete;
	LinkedHashPoolEmptyNode& operator=(LinkedHashPoolEmptyNode&&) = delete;

};

template <typename K, typename V>
struct LinkedHashPoolNode : public intrusive::ListHook<LinkedHashPoolNode<K, V> >, intrusive::MapHook<K, LinkedHashPoolNode<K, V> > {
	using Key_t = K;
	using Value_t = V;
	V value;

	LinkedHashPoolNode() : value() { }

	LinkedHashPoolNode(const LinkedHashPoolNode&) = delete;
	LinkedHashPoolNode& operator=(const LinkedHashPoolNode&) = delete;

	LinkedHashPoolNode(LinkedHashPoolNode&&) = delete;
	LinkedHashPoolNode& operator=(LinkedHashPoolNode&&) = delete;

	bool operator==(const LinkedHashPoolNode& data) const noexcept {
		return value == data.value;
	}
};

template <
typename Node_t,
typename H = std::hash<typename Node_t::Key_t>,
typename SA = std::allocator<Node_t>,
typename BA = std::allocator<intrusive::MapBucket<Node_t> >
>
class LinkedHashPool {
	friend class TestLinkedHashPool;

	using Key_t = typename Node_t::Key_t;
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
	using Iterator_t = typename List_t::Iterator_t;
	using ConstIterator_t = typename List_t::ConstIterator_t;
	using ReverseIterator_t = typename List_t::ReverseIterator_t;
	using ConstReverseIterator_t = typename List_t::ConstReverseIterator_t;

	LinkedHashPool(unsigned capacity, float load_factor) noexcept
	: m_capacity(capacity),
	m_storage(nullptr),
	m_map((capacity / load_factor) + 1),
	m_list_cached(),
	m_list_freed(),
	m_allocator() { }

	LinkedHashPool(const LinkedHashPool&) = delete;
	LinkedHashPool& operator=(const LinkedHashPool&) = delete;

	LinkedHashPool(LinkedHashPool&& rv) = delete;
	LinkedHashPool& operator=(LinkedHashPool&&) = delete;

	virtual ~LinkedHashPool() noexcept {
		destroy();
	}

	/**
	 * Allocate the node storage.
	 * @return 0 - if the cache has been allocated successfully.
	 */
	int allocate() noexcept {
		if (m_storage)
			return -1;

		m_storage = m_allocator.allocate(m_capacity);
		if (m_storage == nullptr)
			return -1;

		for (unsigned i = 0; i < m_capacity; i++) {
			m_allocator.construct(m_storage + i);
			m_list_freed.push_back(m_storage[i]);
		}

		if (not m_map.allocate()) {
			destroy();
			return -1;
		}
		return 0;
	}

	inline Iterator_t begin() noexcept {
		return m_list_cached.begin();
	}

	inline ConstIterator_t cbegin() const noexcept {
		return m_list_cached.cbegin();
	}

	inline ReverseIterator_t rbegin() noexcept {
		return m_list_cached.rbegin();
	}

	inline ConstReverseIterator_t crbegin() const noexcept {
		return m_list_cached.crbegin();
	}

	inline Iterator_t end() const noexcept {
		return m_list_cached.end();
	}

	inline ConstIterator_t cend() const noexcept {
		return m_list_cached.cend();
	}

	inline ReverseIterator_t rend() noexcept {
		return m_list_cached.rend();
	}

	inline ConstReverseIterator_t crend() const noexcept {
		return m_list_cached.crend();
	}

	Iterator_t push_back(const Key_t& key, bool& key_collision) noexcept {
		key_collision = false;
		auto it = m_map.find(key);
		if (it) {
			m_list_cached.remove(*it);
			m_list_cached.push_back(*it);
			key_collision = true;
		} else {
			Node_t* freed;
			if (available()) {
				freed = m_list_freed.pop_back();
				m_list_cached.push_back(*freed);
				it = m_map.link(key, *freed);
			}
		}
		return Iterator_t(it.get());
	}

	inline Iterator_t pop_front() noexcept {
		Node_t* result = nullptr;
		if (size()) {
			result = m_list_cached.pop_front();
			m_list_freed.push_back(*result);
			m_map.remove(*result);
		}
		return Iterator_t(result);
	}

	inline ConstIterator_t find(const Key_t& key) const noexcept {
		return ConstIterator_t(m_map.find(key).get());
	}

	inline Iterator_t find(const Key_t& key) noexcept {
		return Iterator_t(m_map.find(key).get());
	}

	inline void move_back(Iterator_t it) noexcept {
		m_list_cached.remove(*it);
		m_list_cached.push_back(*it);
	}

	inline void remove(Iterator_t it) noexcept {
		m_map.remove(*it);
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

	inline static Iterator_t iterator_reverse(ReverseIterator_t it) noexcept {
		return Iterator_t(it.get());
	}

	inline static ConstIterator_t iterator_reverse(ConstReverseIterator_t it) noexcept {
		return ConstIterator_t(it.get());
	}

	inline static ReverseIterator_t iterator_reverse(Iterator_t it) noexcept {
		return ReverseIterator_t(it.get());
	}

	inline static ConstReverseIterator_t iterator_reverse(ConstIterator_t it) noexcept {
		return ConstReverseIterator_t(it.get());
	}

	inline size_t capacity() const noexcept {
		return m_capacity;
	}

	inline size_t size() const noexcept {
		return m_list_cached.size();
	}

	inline size_t available() const noexcept {
		return m_list_freed.size();
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

#endif /* CACHE_LINKEDHASHPOOL_H */

