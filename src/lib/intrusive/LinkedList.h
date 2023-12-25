#pragma once

#include <cstdlib>
#include <cassert>

namespace intrusive {

template<typename N>
struct LinkedListHook {
	N* __il_next;
	N* __il_prev;
	bool __il_linked;

	LinkedListHook() noexcept : __il_next(nullptr), __il_prev(nullptr), __il_linked(false) {}

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
				node_ptr = node_ptr->__il_next;
			} else {
				node_ptr = node_ptr->__il_prev;
			}
			return *this;
		}

		IteratorBasic operator++(int)noexcept {
			if constexpr (IsForward) {
				node_ptr = node_ptr->__il_next;
			} else {
				node_ptr = node_ptr->__il_prev;
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
			assert(not node.__il_linked);
		}
	}

	inline static void check_linked(ListNode& node) noexcept {
		if constexpr (SanityCheck) {
			assert(node.__il_linked);
		}
	}

	inline void link_first(ListNode& node) noexcept {
		node.__il_next = nullptr;
		node.__il_prev = nullptr;
		node.__il_linked = true;
		_head = _tail = &node;
		_size++;
	}

	inline void link_head(ListNode& node) noexcept {
		node.__il_next = _head;
		node.__il_prev = nullptr;
		node.__il_linked = true;
		_head->__il_prev = &node;
		_head = &node;
		_size++;
	}

	inline void link_tail(ListNode& node) noexcept {
		node.__il_next = nullptr;
		node.__il_prev = _tail;
		node.__il_linked = true;
		_tail->__il_next = &node;
		_tail = &node;
		_size++;
	}

	inline void link_before(ListNode& before, ListNode& node) noexcept {
		node.__il_next = &before;
		node.__il_prev = before.__il_prev;
		node.__il_linked = true;
		before.__il_prev->__il_next = &node;
		before.__il_prev = &node;
		_size++;
	}

	inline void link_after(ListNode& after, ListNode& node) noexcept {
		node.__il_next = after.__il_next;
		node.__il_prev = &after;
		node.__il_linked = true;
		after.__il_next->__il_prev = &node;
		after.__il_next = &node;
		_size++;
	}

	inline ListNode* unlink_last() noexcept {
		ListNode* result = _head;
		_head->__il_next = nullptr;
		_head->__il_prev = nullptr;
		_head->__il_linked = false;
		_head = nullptr;
		_tail = nullptr;
		_size--;
		return result;
	}

	inline ListNode* unlink_head() noexcept {
		ListNode* result = _head;
		_head = _head->__il_next;
		_head->__il_prev = nullptr;
		result->__il_next = nullptr;
		result->__il_prev = nullptr;
		result->__il_linked = false;
		_size--;
		return result;
	}

	inline ListNode* unlink_tail() noexcept {
		ListNode* result = _tail;
		_tail = _tail->__il_prev;
		_tail->__il_next = nullptr;
		result->__il_next = nullptr;
		result->__il_prev = nullptr;
		result->__il_linked = false;
		_size--;
		return result;
	}

	inline void unlink(ListNode& node) noexcept {
		node.__il_prev->__il_next = node.__il_next;
		node.__il_next->__il_prev = node.__il_prev;
		node.__il_next = nullptr;
		node.__il_prev = nullptr;
		node.__il_linked = false;
		_size--;
	}

	inline void make_empty() noexcept {
		_head = _tail = nullptr;
		_size = 0;
	}

};

}; // namespace intrusive
