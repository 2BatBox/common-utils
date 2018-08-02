#ifndef INTRUSIVE_LIST_H
#define INTRUSIVE_LIST_H

#include <cstdlib>

namespace intrusive {

template <typename N>
struct ListHook {
	N* il_next;
	N* il_prev;
	bool il_linked;

	ListHook() noexcept : il_next(nullptr), il_prev(nullptr), il_linked(false) { }

	ListHook(const ListHook&) = delete;
	ListHook& operator=(const ListHook&) = delete;

	ListHook(ListHook&&) = delete;
	ListHook& operator=(ListHook&&) = delete;
};

template <typename ListNode>
class List {
protected:
	ListNode* m_head;
	ListNode* m_tail;
	size_t m_size;

	template<typename N>
	struct Iterator {
		friend class List;

		Iterator() noexcept : node_ptr(nullptr) { }

		Iterator(N* node) noexcept : node_ptr(node) { }

		bool operator==(const Iterator& it) const noexcept {
			return node_ptr == it.node_ptr;
		}

		bool operator!=(const Iterator& it) const noexcept {
			return node_ptr != it.node_ptr;
		}

		Iterator& operator++() noexcept {
			node_ptr = node_ptr->il_next;
			return *this;
		}

		Iterator operator++(int)noexcept {
			node_ptr = node_ptr->il_next;
			return Iterator(node_ptr);
		}

		const N& operator*() const noexcept {
			return *node_ptr;
		}

		N& operator*() noexcept {
			return *node_ptr;
		}

		const N* operator->() const noexcept {
			return node_ptr;
		}

		N* operator->() noexcept {
			return node_ptr;
		}

		operator bool() const noexcept {
			return node_ptr;
		}

	private:
		N* node_ptr;
	};

	template<typename N>
	struct ReverseIterator {
		friend class List;

		ReverseIterator() noexcept : node_ptr(nullptr) { }

		ReverseIterator(N* value) noexcept : node_ptr(value) { }

		bool operator==(const ReverseIterator& it) const noexcept {
			return node_ptr == it.node_ptr;
		}

		bool operator!=(const ReverseIterator& it) const noexcept {
			return node_ptr != it.node_ptr;
		}

		ReverseIterator& operator++() noexcept {
			node_ptr = node_ptr->il_prev;
			return *this;
		}

		ReverseIterator operator++(int)noexcept {
			node_ptr = node_ptr->il_prev;
			return ReverseIterator(node_ptr);
		}

		const N& operator*() const noexcept {
			return *node_ptr;
		}

		N& operator*() noexcept {
			return *node_ptr;
		}

		const N* operator->() const noexcept {
			return node_ptr;
		}

		N* operator->() noexcept {
			return node_ptr;
		}

	private:
		N* node_ptr;
	};

	using Iterator_t = Iterator<ListNode>;
	using ConstIterator_t = Iterator<const ListNode>;
	using ReverseIterator_t = ReverseIterator<ListNode>;
	using ConstReverseIterator_t = ReverseIterator<const ListNode>;

public:

	List() noexcept : m_head(nullptr), m_tail(nullptr), m_size(0) { }

	List(const List&) = delete;
	List& operator=(const List&) = delete;

	List(List&& rv) : m_head(rv.m_head), m_tail(rv.m_tail), m_size(rv.m_size) {
		rv.m_head = rv.m_tail = nullptr;
		rv.m_size = 0;
	}

	List& operator=(List&& rv) {
		if (this != &rv) {
			m_head = rv.m_head;
			m_tail = rv.m_tail;
			m_size = rv.m_size;
			rv.clean_state();
		}
		return *this;
	}

	/**
	 * Be careful, The list must be empty before the storage has been destroyed.
	 */
	virtual ~List() noexcept {
		clear();
		clean_state();
	}

	ListNode* head() noexcept {
		return m_head;
	}

	ListNode* tail() noexcept {
		return m_tail;
	}

	bool push_front(ListNode& node) noexcept {
		if (sanity_check(node)) {
			if (m_head)
				link_head(node);
			else
				link_first(node);
			return true;
		}
		return false;
	}

	bool push_back(ListNode& node) noexcept {
		if (sanity_check(node)) {
			if (m_tail)
				link_tail(node);
			else
				link_first(node);
			return true;
		}
		return false;
	}

	ListNode* pop_front() noexcept {
		if (m_head != m_tail) {
			return unlink_head();
		} else if (m_head) {
			return unlink_last();
		}
		return nullptr;
	}

	ListNode* pop_back() noexcept {
		if (m_head != m_tail) {
			return unlink_tail();
		} else if (m_head) {
			return unlink_last();
		}
		return nullptr;
	}

	bool insert_before(ListNode& before, ListNode& node) noexcept {
		if (before.il_linked && sanity_check(node)) {
			if (&before == m_head)
				link_head(node);
			else
				link_before(before, node);
			return true;
		}
		return false;
	}

	bool insert_after(ListNode& after, ListNode& node) noexcept {
		if (after.il_linked && sanity_check(node)) {
			if (&after == m_tail)
				link_tail(node);
			else
				link_after(after, node);
			return true;
		}
		return false;
	}

	bool remove(ListNode& node) noexcept {
		if (m_head && node.il_linked) {
			if (&node == m_head)
				pop_front();
			else if (&node == m_tail)
				pop_back();
			else
				unlink(node);
			return true;
		}
		return false;
	}

	/**
	 * Unlink all objects in the list.
	 */
	void clear() noexcept {
		while (m_head)
			pop_front();
	}

	inline size_t size() const noexcept {
		return m_size;
	}

	inline Iterator_t begin() noexcept {
		return Iterator_t(m_head);
	}

	inline ConstIterator_t cbegin() const noexcept {
		return ConstIterator_t(m_head);
	}

	inline Iterator_t end() noexcept {
		return Iterator_t();
	}

	inline ConstIterator_t cend() const noexcept {
		return ConstIterator_t();
	}

	inline ReverseIterator_t rbegin() noexcept {
		return ReverseIterator_t(m_tail);
	}

	inline ConstReverseIterator_t crbegin() const noexcept {
		return ConstReverseIterator_t(m_tail);
	}

	inline ReverseIterator_t rend() noexcept {
		return ReverseIterator_t();
	}

	inline ConstReverseIterator_t crend() const noexcept {
		return ConstReverseIterator_t();
	}

private:

	inline static bool sanity_check(ListNode& node) noexcept {
		return (not node.il_linked);
	}

	inline void link_first(ListNode& node) noexcept {
		node.il_next = nullptr;
		node.il_prev = nullptr;
		node.il_linked = true;
		m_head = m_tail = &node;
		m_size++;
	}

	inline void link_head(ListNode& node) noexcept {
		node.il_next = m_head;
		node.il_prev = nullptr;
		node.il_linked = true;
		m_head->il_prev = &node;
		m_head = &node;
		m_size++;
	}

	inline void link_tail(ListNode& node) noexcept {
		node.il_next = nullptr;
		node.il_prev = m_tail;
		node.il_linked = true;
		m_tail->il_next = &node;
		m_tail = &node;
		m_size++;
	}

	inline void link_before(ListNode& before, ListNode& node) noexcept {
		node.il_next = &before;
		node.il_prev = before.il_prev;
		node.il_linked = true;
		before.il_prev->il_next = &node;
		before.il_prev = &node;
		m_size++;
	}

	inline void link_after(ListNode& after, ListNode& node) noexcept {
		node.il_next = after.il_next;
		node.il_prev = &after;
		node.il_linked = true;
		after.il_next->il_prev = &node;
		after.il_next = &node;
		m_size++;
	}

	inline ListNode* unlink_last() noexcept {
		ListNode* result = m_head;
		m_head->il_next = nullptr;
		m_head->il_prev = nullptr;
		m_head->il_linked = false;
		m_head = m_tail = nullptr;
		m_size--;
		return result;
	}

	inline ListNode* unlink_head() noexcept {
		ListNode* result = m_head;
		m_head = m_head->il_next;
		m_head->il_prev = nullptr;
		result->il_next = nullptr;
		result->il_prev = nullptr;
		result->il_linked = false;
		m_size--;
		return result;
	}

	inline ListNode* unlink_tail() noexcept {
		ListNode* result = m_tail;
		m_tail = m_tail->il_prev;
		m_tail->il_next = nullptr;
		result->il_next = nullptr;
		result->il_prev = nullptr;
		result->il_linked = false;
		m_size--;
		return result;
	}

	inline void unlink(ListNode& node) noexcept {
		node.il_prev->il_next = node.il_next;
		node.il_next->il_prev = node.il_prev;
		node.il_next = nullptr;
		node.il_prev = nullptr;
		node.il_linked = false;
		m_size--;
	}

	inline void clean_state() noexcept {
		m_head = m_tail = nullptr;
		m_size = 0;
	}

};

}; // namespace intrusive

#endif /* INTRUSIVE_LIST_H */