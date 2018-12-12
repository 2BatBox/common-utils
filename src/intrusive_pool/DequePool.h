#ifndef INTRUSIVEPOOL_DEQUEDPOOL_H
#define INTRUSIVEPOOL_DEQUEDPOOL_H

#include "../intrusive/LinkedList.h"
#include "../intrusive/HashMap.h"

#include <bits/allocator.h>

namespace intrusive {

template<typename T>
struct DequePoolNode : public intrusive::LinkedListHook<DequePoolNode<T> > {
	using Value_t = T;
	T value;

	DequePoolNode() : value() {}

	DequePoolNode(const DequePoolNode&) = delete;
	DequePoolNode& operator=(const DequePoolNode&) = delete;

	DequePoolNode(DequePoolNode&&) = delete;
	DequePoolNode& operator=(DequePoolNode&&) = delete;

	bool operator==(const DequePoolNode& data) const noexcept {
		return value == data.value;
	}
};

template<
	typename Node_t,
	typename SA = std::allocator<Node_t>
>
class DequePool {
	friend class TestDequePool;

	using Value_t = typename Node_t::Value_t;
	using List_t = intrusive::LinkedList<Node_t>;

	const size_t m_capacity;
	Node_t* m_storage;
	List_t m_list_cached;
	List_t m_list_freed;
	SA m_allocator;

public:
	using Iterator_t = typename List_t::Iterator_t;
	using ConstIterator_t = typename List_t::ConstIterator_t;
	using ReverseIterator_t = typename List_t::ReverseIterator_t;
	using ConstReverseIterator_t = typename List_t::ConstReverseIterator_t;

	DequePool(unsigned capacity) noexcept
		: m_capacity(capacity), m_storage(nullptr), m_list_cached(), m_list_freed(), m_allocator() {}

	DequePool(const DequePool&) = delete;
	DequePool& operator=(const DequePool&) = delete;

	DequePool(DequePool&& rv) = delete;
	DequePool& operator=(DequePool&&) = delete;

	virtual ~DequePool() noexcept {
		destroy();
	}

	/**
	 * Allocate the node storage.
	 * @return 0 - if the cache has been allocated successfully.
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

	inline Iterator_t end() noexcept {
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

	inline Iterator_t push_front() noexcept {
		Node_t* result = nullptr;
		if(available()) {
			result = m_list_freed.pop_back();
			m_list_cached.push_front(*result);
		}
		return Iterator_t(result);
	}

	inline Iterator_t push_back() noexcept {
		Node_t* result = nullptr;
		if(available()) {
			result = m_list_freed.pop_back();
			m_list_cached.push_back(*result);
		}
		return Iterator_t(result);
	}

	inline Iterator_t pop_front() noexcept {
		Node_t* result = nullptr;
		if(size()) {
			result = m_list_cached.pop_front();
			m_list_freed.push_back(*result);
		}
		return Iterator_t(result);
	}

	inline Iterator_t pop_back() noexcept {
		Node_t* result = nullptr;
		if(size()) {
			result = m_list_cached.pop_back();
			m_list_freed.push_back(*result);
		}
		return Iterator_t(result);
	}

	inline void remove(Iterator_t it) noexcept {
		m_list_cached.remove(*it);
		m_list_freed.push_back(*it);
	}

	inline void remove(ReverseIterator_t it) noexcept {
		m_list_cached.remove(*it);
		m_list_freed.push_back(*it);
	}

	void reset() noexcept {
		m_list_cached.clear();
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

private:

	void destroy() noexcept {
		if(m_storage) {
			m_list_freed.clear();
			m_list_cached.clear();
			for(size_t i = 0; i < m_capacity; i++) {
				m_allocator.destroy(m_storage + i);
			}
			m_allocator.deallocate(m_storage, m_capacity);
			m_storage = nullptr;
		}
	}

};

}; // namespace intrusive

#endif /* INTRUSIVEPOOL_DEQUEDPOOL_H */

