#pragma once

#include <cstdlib>
#include <cassert>

namespace intrusive {

template<typename N>
struct LinkedListHook {
	N* next;
	N* prev;
	bool linked;

	LinkedListHook() noexcept : next(nullptr), prev(nullptr), linked(false) {}

	LinkedListHook(const LinkedListHook&) = delete;
	LinkedListHook& operator=(const LinkedListHook&) = delete;

	LinkedListHook(LinkedListHook&&) = delete;
	LinkedListHook& operator=(LinkedListHook&&) = delete;
};

template<typename ListNode, bool SanityCheck = false>
class LinkedList {
protected:
	ListNode* _head;
	ListNode* _tail;
	size_t _size;

	template<typename V, bool IsForward>
	struct IteratorBasic {
		friend class LinkedList;

		IteratorBasic() noexcept : node_ptr(nullptr) {}

		IteratorBasic(V* node) noexcept : node_ptr(node) {}

		bool operator==(const IteratorBasic& it) const noexcept {
			return node_ptr == it.node_ptr;
		}

		bool operator!=(const IteratorBasic& it) const noexcept {
			return node_ptr != it.node_ptr;
		}

		IteratorBasic& operator++() noexcept {
			if constexpr (IsForward) {
				node_ptr = node_ptr->__ill.next;
			} else {
				node_ptr = node_ptr->__ill.prev;
			}
			return *this;
		}

		IteratorBasic operator++(int)noexcept {
			if constexpr (IsForward) {
				node_ptr = node_ptr->__ill.next;
			} else {
				node_ptr = node_ptr->__ill.prev;
			}
			return IteratorBasic(node_ptr);
		}

		const V& operator*() const noexcept {
			return *node_ptr;
		}

		V& operator*() noexcept {
			return *node_ptr;
		}

		const V* operator->() const noexcept {
			return node_ptr;
		}

		V* operator->() noexcept {
			return node_ptr;
		}

		operator bool() const noexcept {
			return node_ptr != nullptr;
		}

		inline const V* get() const noexcept {
			return node_ptr;
		}

		inline V* get() noexcept {
			return node_ptr;
		}

	private:
		V* node_ptr;
	};


public:

	using Iterator_t = IteratorBasic<ListNode, true>;
	using ConstIterator_t = IteratorBasic<const ListNode, true>;
	using ReverseIterator_t = IteratorBasic<ListNode, false>;
	using ConstReverseIterator_t = IteratorBasic<const ListNode, false>;

	LinkedList() noexcept : _head(nullptr), _tail(nullptr), _size(0) {}

	LinkedList(const LinkedList&) = delete;
	LinkedList& operator=(const LinkedList&) = delete;

	LinkedList(LinkedList&& rv) noexcept : _head(rv._head), _tail(rv._tail), _size(rv._size) {
		rv.make_empty();
	}

	LinkedList& operator=(LinkedList&& rv) noexcept {
		if(this != &rv) {
			_head = rv._head;
			_tail = rv._tail;
			_size = rv._size;
			rv.make_empty();
		}
		return *this;
	}

	/**
	 * Be careful, The list must be empty before the storage has been destroyed.
	 */
	virtual ~LinkedList() noexcept {
		clear();
		make_empty();
	}

	ListNode* head() noexcept {
		return _head;
	}

	ListNode* tail() noexcept {
		return _tail;
	}

	void push_front(ListNode& node) noexcept {
		check_free(node);
		if(_head) {
			link_head(node);
		} else {
			link_first(node);
		}
	}

	void push_back(ListNode& node) noexcept {
		check_free(node);
		if(_tail) {
			link_tail(node);
		} else {
			link_first(node);
		}
	}

	void insert_before(ListNode& before, ListNode& node) noexcept {
		check_linked(before);
		check_free(node);
		if(&before == _head) {
			link_head(node);
		} else {
			link_before(before, node);
		}
	}

	void insert_after(ListNode& after, ListNode& node) noexcept {
		check_linked(after);
		check_free(node);
		if(&after == _tail) {
			link_tail(node);
		} else {
			link_after(after, node);
		}
	}

	ListNode* pop_front() noexcept {
		if(_head != _tail) {
			return unlink_head();
		} else if(_head) {
			return unlink_last();
		}
		return nullptr;
	}

	ListNode* pop_back() noexcept {
		if(_head != _tail) {
			return unlink_tail();
		} else if(_head) {
			return unlink_last();
		}
		return nullptr;
	}

	void remove(ListNode& node) noexcept {
		check_linked(node);
		if(_head) {
			if(&node == _head) {
				pop_front();
			} else if(&node == _tail) {
				pop_back();
			} else {
				unlink(node);
			}
		}
	}

	/**
	 * Unlink all objects in the list.
	 */
	void clear() noexcept {
		while(_head) {
			pop_front();
		}
	}

	inline size_t size() const noexcept {
		return _size;
	}

	inline Iterator_t begin() noexcept {
		return Iterator_t(_head);
	}

	inline ReverseIterator_t rbegin() noexcept {
		return ReverseIterator_t(_tail);
	}

	inline ConstIterator_t cbegin() const noexcept {
		return ConstIterator_t(_head);
	}

	inline ConstReverseIterator_t crbegin() const noexcept {
		return ConstReverseIterator_t(_tail);
	}

	inline Iterator_t end() const noexcept {
		return Iterator_t();
	}

	inline ReverseIterator_t rend() noexcept {
		return ReverseIterator_t();
	}

	inline ConstIterator_t cend() const noexcept {
		return ConstIterator_t();
	}

	inline ConstReverseIterator_t crend() const noexcept {
		return ConstReverseIterator_t();
	}

private:

	inline static void check_free(ListNode& node) noexcept {
		if constexpr (SanityCheck) {
			assert(not node.__ill.linked);
		}
	}

	inline static void check_linked(ListNode& node) noexcept {
		if constexpr (SanityCheck) {
			assert(node.__ill.linked);
		}
	}

	inline void link_first(ListNode& node) noexcept {
		node.__ill.next = nullptr;
		node.__ill.prev = nullptr;
		node.__ill.linked = true;
		_head = _tail = &node;
		_size++;
	}

	inline void link_head(ListNode& node) noexcept {
		node.__ill.next = _head;
		node.__ill.prev = nullptr;
		node.__ill.linked = true;
		_head->__ill.prev = &node;
		_head = &node;
		_size++;
	}

	inline void link_tail(ListNode& node) noexcept {
		node.__ill.next = nullptr;
		node.__ill.prev = _tail;
		node.__ill.linked = true;
		_tail->__ill.next = &node;
		_tail = &node;
		_size++;
	}

	inline void link_before(ListNode& before, ListNode& node) noexcept {
		node.__ill.next = &before;
		node.__ill.prev = before.__ill.prev;
		node.__ill.linked = true;
		before.__ill.prev->__ill.next = &node;
		before.__ill.prev = &node;
		_size++;
	}

	inline void link_after(ListNode& after, ListNode& node) noexcept {
		node.__ill.next = after.__ill.next;
		node.__ill.prev = &after;
		node.__ill.linked = true;
		after.__ill.next->__ill.prev = &node;
		after.__ill.next = &node;
		_size++;
	}

	inline ListNode* unlink_last() noexcept {
		ListNode* node = _head;
		_head->__ill.next = nullptr;
		_head->__ill.prev = nullptr;
		_head->__ill.linked = false;
		_head = nullptr;
		_tail = nullptr;
		_size--;
		return node;
	}

	inline ListNode* unlink_head() noexcept {
		ListNode* node = _head;
		_head = _head->__ill.next;
		_head->__ill.prev = nullptr;
		node->__ill.next = nullptr;
		node->__ill.prev = nullptr;
		node->__ill.linked = false;
		_size--;
		return node;
	}

	inline ListNode* unlink_tail() noexcept {
		ListNode* node = _tail;
		_tail = _tail->__ill.prev;
		_tail->__ill.next = nullptr;
		node->__ill.next = nullptr;
		node->__ill.prev = nullptr;
		node->__ill.linked = false;
		_size--;
		return node;
	}

	inline void unlink(ListNode& node) noexcept {
		node.__ill.prev->__ill.next = node.__ill.next;
		node.__ill.next->__ill.prev = node.__ill.prev;
		node.__ill.next = nullptr;
		node.__ill.prev = nullptr;
		node.__ill.linked = false;
		_size--;
	}

	inline void make_empty() noexcept {
		_head = _tail = nullptr;
		_size = 0;
	}

};

}; // namespace intrusive
