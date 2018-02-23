#ifndef INTRUSIVELIST_H
#define INTRUSIVELIST_H

#include <cstdlib>

template <typename T>
struct IntrusiveListHook {
	T* il_next;
	T* il_prev;
	bool il_linked;

	IntrusiveListHook() noexcept : il_next(nullptr), il_prev(nullptr), il_linked(false) { }
};

template <typename T>
class IntrusiveList {
	T* head;
	T* tail;
	size_t list_size;

	template<typename V>
	struct Iterator {
		friend class IntrusiveList;

		Iterator() noexcept : value(nullptr) { }

		Iterator(V* value) noexcept : value(value) { }

		bool operator==(const Iterator& it) const noexcept {
			return value == it.value;
		}

		bool operator!=(const Iterator& it) const noexcept {
			return value != it.value;
		}

		Iterator& operator++() noexcept {
			value = value->il_next;
			return *this;
		}

		Iterator operator++(int) noexcept {
			value = value->il_next;
			return Iterator(value);
		}

		V& operator*() noexcept {
			return *value;
		}

		V* operator->() noexcept {
			return value;
		}

		const V& operator*() const noexcept {
			return *value;
		}

		const V* operator->() const noexcept {
			return value;
		}

	private:
		V* value;
	};
	
	template<typename V>
	struct RecursiveIterator {
		friend class IntrusiveList;

		RecursiveIterator() noexcept : value(nullptr) { }

		RecursiveIterator(V* value) noexcept : value(value) { }

		bool operator==(const RecursiveIterator& it) const noexcept {
			return value == it.value;
		}

		bool operator!=(const RecursiveIterator& it) const noexcept {
			return value != it.value;
		}

		RecursiveIterator& operator++() noexcept {
			value = value->il_prev;
			return *this;
		}

		RecursiveIterator operator++(int) noexcept {
			value = value->il_prev;
			return RecursiveIterator(value);
		}

		V& operator*() noexcept {
			return *value;
		}

		V* operator->() noexcept {
			return value;
		}

		const V& operator*() const noexcept {
			return *value;
		}

		const V* operator->() const noexcept {
			return value;
		}

	private:
		V* value;
	};

	typedef Iterator<T> Iterator_t;
	typedef Iterator<const T> ConstIterator_t;
	typedef RecursiveIterator<T> RecursiveIterator_t;
	typedef RecursiveIterator<const T> ConstRecursiveIterator_t;

public:

	IntrusiveList() noexcept : head(nullptr), tail(nullptr), list_size(0) { }
	
	IntrusiveList(const IntrusiveList&) = delete;
	IntrusiveList& operator=(const IntrusiveList&) = delete;
	
	IntrusiveList(IntrusiveList&& rv) : head(rv.head), tail(rv.tail), list_size(rv.list_size) {
		rv.head = rv.tail = nullptr;
		rv.list_size = 0;
	}
	
	IntrusiveList& operator=(IntrusiveList&& rv){
		if(this != &rv){
			head = rv.head;
			tail = rv.tail;
			list_size = rv.list_size;
			rv.head = rv.tail = nullptr;
			rv.list_size = 0;
		}
		return *this;
	}

	virtual ~IntrusiveList() noexcept {
		reset();
	}

	bool push_front(T& value) noexcept {
		if (sanity_check(value)) {
			if (head)
				link_head(value);
			else
				link_first(value);

			return true;
		}
		return false;
	}

	bool push_back(T& value) noexcept {
		if (sanity_check(value)) {
			if (tail)
				link_tail(value);
			else
				link_first(value);

			return true;
		}
		return false;
	}

	T* pop_front() noexcept {
		if (head != tail) {
			return unlink_head();
		} else if (head) {
			return unlink_last();
		}
		return nullptr;
	}

	T* pop_back() noexcept {
		if (head != tail) {
			return unlink_tail();
		} else if (head) {
			return unlink_last();
		}
		return nullptr;
	}
	
	bool insert_before(T& before, T& value) noexcept {
		if(before.il_linked && sanity_check(value)){
			if(&before == head)
				link_head(value);
			else
				link_before(before, value);
			return true;
		}
		return false;
	}
	
	bool insert_after(T& after, T& value) noexcept {
		if(after.il_linked && sanity_check(value)){
			if(&after == tail)
				link_tail(value);
			else
				link_after(after, value);
			return true;
		}
		return false;
	}


	bool remove(T& value) noexcept {
		if (head && value.il_linked) {
			if (&value == head)
				pop_front();
			else if (&value == tail)
				pop_back();
			else
				unlink(value);

			return true;
		}
		return false;
	}
	
	void reset() noexcept {
		while(head)
			pop_front();
	}

	inline size_t size() const noexcept {
		return list_size;
	}
	
	Iterator_t begin() noexcept {
		return Iterator_t(head);
	}

	ConstIterator_t cbegin() const noexcept {
		return ConstIterator_t(head);
	}

	Iterator_t end() noexcept {
		return Iterator_t();
	}

	ConstIterator_t cend() const noexcept {
		return ConstIterator_t();
	}
	
	RecursiveIterator_t rbegin() noexcept {
		return RecursiveIterator_t(tail);
	}

	ConstRecursiveIterator_t crbegin() const noexcept {
		return ConstRecursiveIterator_t(tail);
	}

	RecursiveIterator_t rend() noexcept {
		return RecursiveIterator_t();
	}

	ConstRecursiveIterator_t crend() const noexcept {
		return ConstRecursiveIterator_t();
	}

private:

	inline static bool sanity_check(T& value) noexcept {
		return (not value.il_linked);
	}

	inline void link_first(T& value) noexcept {
		value.il_next = nullptr;
		value.il_prev = nullptr;
		value.il_linked = true;
		head = tail = &value;
		list_size++;
	}

	inline void link_head(T& value) noexcept {
		value.il_next = head;
		value.il_prev = nullptr;
		value.il_linked = true;
		head->il_prev = &value;
		head = &value;
		list_size++;
	}

	inline void link_tail(T& value) noexcept {
		value.il_next = nullptr;
		value.il_prev = tail;
		value.il_linked = true;
		tail->il_next = &value;
		tail = &value;
		list_size++;
	}
	
	inline void link_before(T& before, T& value) noexcept {
		value.il_next = &before;
		value.il_prev = before.il_prev;
		value.il_linked = true;
		before.il_prev->il_next = &value;
		before.il_prev = &value;
		list_size++;
	}
	
	inline void link_after(T& after, T& value) noexcept {
		value.il_next = after.il_next;
		value.il_prev = &after;
		value.il_linked = true;
		after.il_next->il_prev = &value;
		after.il_next = &value;
		list_size++;
	}

	inline T* unlink_last() noexcept {
		T* result = head;
		head->il_next = nullptr;
		head->il_prev = nullptr;
		head->il_linked = false;
		head = tail = nullptr;
		list_size--;
		return result;
	}

	inline T* unlink_head() noexcept {
		T* result = head;
		head = head->il_next;
		head->il_prev = nullptr;
		result->il_next = nullptr;
		result->il_prev = nullptr;
		result->il_linked = false;
		list_size--;
		return result;
	}

	inline T* unlink_tail() noexcept {
		T* result = tail;
		tail = tail->il_prev;
		tail->il_next = nullptr;
		result->il_next = nullptr;
		result->il_prev = nullptr;
		result->il_linked = false;
		list_size--;
		return result;
	}

	inline void unlink(T& value) noexcept {
		value.il_prev->il_next = value.il_next;
		value.il_next->il_prev = value.il_prev;
		value.il_next = nullptr;
		value.il_prev = nullptr;
		value.il_linked = false;
		list_size--;
	}

};

#endif /* INTRUSIVELIST_H */