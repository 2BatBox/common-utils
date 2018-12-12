#ifndef INTRUSIVEPOOL_HASHQUEUEPOOL_H
#define INTRUSIVEPOOL_HASHQUEUEPOOL_H

#include "../intrusive/LinkedList.h"
#include "../intrusive/HashMap.h"

#include <bits/allocator.h>

namespace intrusive {

template<typename K>
struct HashQueuePoolEmptyNode
	: public intrusive::LinkedListHook<HashQueuePoolEmptyNode<K> >,
	  intrusive::HashMapHook<K, HashQueuePoolEmptyNode<K> > {
	using Key_t = K;

	HashQueuePoolEmptyNode() noexcept = default;

	HashQueuePoolEmptyNode(const HashQueuePoolEmptyNode&) = delete;
	HashQueuePoolEmptyNode& operator=(const HashQueuePoolEmptyNode&) = delete;

	HashQueuePoolEmptyNode(HashQueuePoolEmptyNode&&) = delete;
	HashQueuePoolEmptyNode& operator=(HashQueuePoolEmptyNode&&) = delete;

};

template<typename K, typename V>
struct HashQueuePoolNode
	: public intrusive::LinkedListHook<HashQueuePoolNode<K, V> >, intrusive::HashMapHook<K, HashQueuePoolNode<K, V> > {
	using Key_t = K;
	using Value_t = V;
	V value;

	HashQueuePoolNode() : value() {}

	HashQueuePoolNode(const HashQueuePoolNode&) = delete;
	HashQueuePoolNode& operator=(const HashQueuePoolNode&) = delete;

	HashQueuePoolNode(HashQueuePoolNode&&) = delete;
	HashQueuePoolNode& operator=(HashQueuePoolNode&&) = delete;

	bool operator==(const HashQueuePoolNode& data) const noexcept {
		return value == data.value;
	}
};

template<
	typename Node_t,
	typename H = std::hash<typename Node_t::Key_t>,
	typename SA = std::allocator<Node_t>,
	typename BA = std::allocator<intrusive::HashMapBucket<Node_t> >
>
class HashQueuePool {
	friend class TestHashQueuePool;

	using Key_t = typename Node_t::Key_t;
	using List_t = intrusive::LinkedList<Node_t>;
	using Map_t = intrusive::HashMap<Key_t, Node_t, H, BA>;
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

	HashQueuePool(unsigned capacity, float load_factor) noexcept
		: m_capacity(capacity)
		, m_storage(nullptr)
		, m_map((capacity / load_factor) + 1)
		, m_list_cached()
		, m_list_freed()
		, m_allocator() {}

	HashQueuePool(const HashQueuePool&) = delete;
	HashQueuePool& operator=(const HashQueuePool&) = delete;

	HashQueuePool(HashQueuePool&& rv) = delete;
	HashQueuePool& operator=(HashQueuePool&&) = delete;

	virtual ~HashQueuePool() noexcept {
		destroy();
	}

	/**
	 * Allocate the node storage.
	 * @return 0 - if the storage has been allocated successfully.
	 */
	int allocate() noexcept {
		if(m_storage)
			return -1;

		m_storage = m_allocator.allocate(m_capacity);

		if(m_storage == nullptr)
			return -1;

		for(unsigned i = 0; i < m_capacity; i++) {
			m_allocator.construct(m_storage + i);
			m_list_freed.push_back(m_storage[i]);
		}

		if(not m_map.allocate()) {
			destroy();
			return -1;
		}
		return 0;
	}

	inline Iterator_t end() noexcept {
		return m_map.end();
	}

	inline ConstIterator_t cend() const noexcept {
		return m_map.cend();
	}

	Iterator_t push_back(const Key_t& key) noexcept {
		Node_t* freed = nullptr;
		if(available()) {
			freed = m_list_freed.pop_back();
			m_list_cached.push_back(*freed);
			m_map.link(key, *freed);
		}
		return Iterator_t(freed);
	}

	inline Iterator_t peek_front() noexcept {
		Iterator_t result;
		if(size()) {
			result = Iterator_t(m_list_cached.begin().get());
		}
		return result;
	}

	inline Iterator_t pop_front() noexcept {
		Node_t* result = nullptr;
		if(size()) {
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

	void reset() noexcept {
		m_map.clear();
		m_list_cached.clear();
		m_list_freed.clear();
		for(unsigned i = 0; i < m_capacity; i++) {
			m_list_freed.push_back(m_storage[i]);
		}
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

	inline size_t storage_bytes() noexcept {
		return m_capacity * sizeof(Node_t) + m_map.buckets() * sizeof(Bucket_t);
	}

private:

	void destroy() noexcept {
		if(m_storage) {
			m_list_freed.clear();
			m_list_cached.clear();
			m_map.clear();
			for(size_t i = 0; i < m_capacity; i++) {
				m_allocator.destroy(m_storage + i);
			}
			m_allocator.deallocate(m_storage, m_capacity);
			m_storage = nullptr;
		}
	}

};

}; // namespace intrusive

#endif /* INTRUSIVEPOOL_HASHQUEUEPOOL_H */

