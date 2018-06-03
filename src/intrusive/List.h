#ifndef INTRUSIVE_LIST_H
#define INTRUSIVE_LIST_H

#include <cstdlib>

// gcc 4.8.2's -Wnon-virtual-dtor is broken and turned on by -Weffc++
#if __GNUC__ < 3 || (__GNUC__ == 4 && __GNUC_MINOR__ <= 8)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnon-virtual-dtor"
#pragma GCC diagnostic ignored "-Weffc++"
#define GCC_DIAG_POP_NEEDED
#endif

namespace intrusive {

template <typename V>
struct ListHook {
	V* il_next;
	V* il_prev;
	bool il_linked;

	ListHook() noexcept : il_next(nullptr), il_prev(nullptr), il_linked(false) { }
};

template <typename ListNode>
class List {
	ListNode* head;
	ListNode* tail;
	size_t list_size;

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

	List() noexcept : head(nullptr), tail(nullptr), list_size(0) { }

	List(const List&) = delete;
	List& operator=(const List&) = delete;

	List(List&& rv) : head(rv.head), tail(rv.tail), list_size(rv.list_size) {
		rv.head = rv.tail = nullptr;
		rv.list_size = 0;
	}

	List& operator=(List&& rv) {
		if (this != &rv) {
			head = rv.head;
			tail = rv.tail;
			list_size = rv.list_size;
			rv.head = rv.tail = nullptr;
			rv.list_size = 0;
		}
		return *this;
	}

	virtual ~List() noexcept {
		clear();
	}

	bool push_front(ListNode& node) noexcept {
		if (sanity_check(node)) {
			if (head)
				link_head(node);
			else
				link_first(node);
			return true;
		}
		return false;
	}

	bool push_back(ListNode& node) noexcept {
		if (sanity_check(node)) {
			if (tail)
				link_tail(node);
			else
				link_first(node);
			return true;
		}
		return false;
	}

	ListNode* pop_front() noexcept {
		if (head != tail) {
			return unlink_head();
		} else if (head) {
			return unlink_last();
		}
		return nullptr;
	}

	ListNode* pop_back() noexcept {
		if (head != tail) {
			return unlink_tail();
		} else if (head) {
			return unlink_last();
		}
		return nullptr;
	}

	bool insert_before(ListNode& before, ListNode& node) noexcept {
		if (before.il_linked && sanity_check(node)) {
			if (&before == head)
				link_head(node);
			else
				link_before(before, node);
			return true;
		}
		return false;
	}

	bool insert_after(ListNode& after, ListNode& node) noexcept {
		if (after.il_linked && sanity_check(node)) {
			if (&after == tail)
				link_tail(node);
			else
				link_after(after, node);
			return true;
		}
		return false;
	}

	bool remove(ListNode& node) noexcept {
		if (head && node.il_linked) {
			if (&node == head)
				pop_front();
			else if (&node == tail)
				pop_back();
			else
				unlink(node);
			return true;
		}
		return false;
	}

	void clear() noexcept {
		while (head)
			pop_front();
	}

	inline size_t size() const noexcept {
		return list_size;
	}

	inline Iterator_t begin() noexcept {
		return Iterator_t(head);
	}

	inline ConstIterator_t cbegin() const noexcept {
		return ConstIterator_t(head);
	}

	inline Iterator_t end() noexcept {
		return Iterator_t();
	}

	inline ConstIterator_t cend() const noexcept {
		return ConstIterator_t();
	}

	inline ReverseIterator_t rbegin() noexcept {
		return ReverseIterator_t(tail);
	}

	inline ConstReverseIterator_t crbegin() const noexcept {
		return ConstReverseIterator_t(tail);
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
		head = tail = &node;
		list_size++;
	}

	inline void link_head(ListNode& node) noexcept {
		node.il_next = head;
		node.il_prev = nullptr;
		node.il_linked = true;
		head->il_prev = &node;
		head = &node;
		list_size++;
	}

	inline void link_tail(ListNode& node) noexcept {
		node.il_next = nullptr;
		node.il_prev = tail;
		node.il_linked = true;
		tail->il_next = &node;
		tail = &node;
		list_size++;
	}

	inline void link_before(ListNode& before, ListNode& node) noexcept {
		node.il_next = &before;
		node.il_prev = before.il_prev;
		node.il_linked = true;
		before.il_prev->il_next = &node;
		before.il_prev = &node;
		list_size++;
	}

	inline void link_after(ListNode& after, ListNode& node) noexcept {
		node.il_next = after.il_next;
		node.il_prev = &after;
		node.il_linked = true;
		after.il_next->il_prev = &node;
		after.il_next = &node;
		list_size++;
	}

	inline ListNode* unlink_last() noexcept {
		ListNode* result = head;
		head->il_next = nullptr;
		head->il_prev = nullptr;
		head->il_linked = false;
		head = tail = nullptr;
		list_size--;
		return result;
	}

	inline ListNode* unlink_head() noexcept {
		ListNode* result = head;
		head = head->il_next;
		head->il_prev = nullptr;
		result->il_next = nullptr;
		result->il_prev = nullptr;
		result->il_linked = false;
		list_size--;
		return result;
	}

	inline ListNode* unlink_tail() noexcept {
		ListNode* result = tail;
		tail = tail->il_prev;
		tail->il_next = nullptr;
		result->il_next = nullptr;
		result->il_prev = nullptr;
		result->il_linked = false;
		list_size--;
		return result;
	}

	inline void unlink(ListNode& node) noexcept {
		node.il_prev->il_next = node.il_next;
		node.il_next->il_prev = node.il_prev;
		node.il_next = nullptr;
		node.il_prev = nullptr;
		node.il_linked = false;
		list_size--;
	}

};

}; // namespace intrusive

#if defined(GCC_DIAG_POP_NEEDED)
#pragma GCC diagnostic pop
#undef GCC_DIAG_POP_NEEDED
#endif

#endif /* INTRUSIVE_LIST_H */