#ifndef INTRUSIVELIST_H
#define INTRUSIVELIST_H

template <typename T>
class IntrusiveList {
	T* head;
	T* tail;
	size_t list_size;
	
	template<typename V>
	struct Iterator {

		Iterator() noexcept : value(nullptr) { }

		Iterator(V* value) noexcept : value(value) { }

		bool operator==(const Iterator& it) const noexcept {
			return value == it.value;
		}

		bool operator!=(const Iterator& it) const noexcept {
			return value != it.value;
		}

		Iterator& operator++() noexcept {
			value = value->next;
			return *this;
		}

		Iterator operator++(int) noexcept {
			value = value->next;
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
	struct ReverseIterator {

		ReverseIterator() noexcept : value(nullptr) { }

		ReverseIterator(V* value) noexcept : value(value) { }

		bool operator==(const ReverseIterator& it) const noexcept {
			return value == it.value;
		}

		bool operator!=(const ReverseIterator& it) const noexcept {
			return value != it.value;
		}

		ReverseIterator& operator++() noexcept {
			value = value->prev;
			return *this;
		}

		ReverseIterator operator++(int) noexcept {
			value = value->prev;
			return ReverseIterator(value);
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
	typedef ReverseIterator<T> ReverseIterator_t;
	typedef ReverseIterator<const T> ConstReverseIterator_t;

public:

	struct Hook {
		T* next;
		T* prev;
		bool linked;

		Hook() noexcept : next(nullptr), prev(nullptr), linked(false) { }
	};

	IntrusiveList() noexcept : head(nullptr), tail(nullptr), list_size(0) { }

	~IntrusiveList() noexcept = default;

	void push_front(T* value) noexcept {
		if (sanity_check(value)) {
			if (head)
				link_front(value, head);
			else
				init(value);
		}
	}

	void push_back(T* value) noexcept {
		if (sanity_check(value)) {
			if (tail)
				link_back(tail, value);
			else
				init(value);
		}
	}

	void pop_front() noexcept {
		if (head != tail)
			unlink_front();
		else if (head)
			clear();
	}

	void pop_back() noexcept {
		if (head != tail)
			unlink_back();
		else if (head)
			clear();
	}

	void remove(T* value) noexcept {
		if (value == head)
			pop_front();
		else if (value == tail)
			pop_back();
		else
			unlink(value);
	}

	T* front() noexcept {
		return head;
	}

	T* back() noexcept {
		return tail;
	}
	
	const T* front() const noexcept {
		return head;
	}

	const T* back() const noexcept {
		return tail;
	}

	inline size_t size() const noexcept {
		return list_size;
	}

	// Iterators
	Iterator_t begin() const noexcept {
		return Iterator_t(head);
	}
	
	ReverseIterator_t rbegin() const noexcept {
		return ReverseIterator_t(tail);
	}
	
	const ConstIterator_t cbegin() const noexcept {
		return ConstIterator_t(head);
	}
	
	const ConstReverseIterator_t crbegin() const noexcept {
		return ConstReverseIterator_t(tail);
	}

	Iterator_t end() const noexcept {
		return Iterator_t();
	}

	ReverseIterator_t rend() const noexcept {
		return ReverseIterator_t();
	}
	
	const ConstIterator_t cend() const noexcept {
		return ConstIterator_t();
	}
	
	const ConstReverseIterator_t crend() const noexcept {
		return ConstReverseIterator_t();
	}

private:

	inline static bool sanity_check(const T* value) noexcept {
		return value && (not value->linked);
	}

	inline void link_front(T* value, T* next) noexcept {
		value->next = next;
		value->prev = nullptr;
		next->linked = true;
		next->prev = value;
		head = value;
		list_size++;
	}

	inline void link_back(T* prev, T* value) noexcept {
		value->next = nullptr;
		value->prev = prev;
		value->linked = true;
		prev->next = value;
		tail = value;
		list_size++;
	}

	inline void init(T* value) noexcept {
		value->next = nullptr;
		value->prev = nullptr;
		value->linked = true;
		head = tail = value;
		list_size++;
	}

	inline void clear() noexcept {
		head->next = nullptr;
		head->prev = nullptr;
		head->linked = false;
		head = tail = nullptr;
		list_size--;
	}

	inline void unlink_front() noexcept {
		T* tmp_head = head->next;
		head->next = nullptr;
		head->prev = nullptr;
		head->linked = false;
		head = tmp_head;
		head->prev = nullptr;
		list_size--;
	}

	inline void unlink_back() noexcept {
		T* tmp_tail = tail->prev;
		tail->next = nullptr;
		tail->prev = nullptr;
		tail->linked = false;
		tail = tmp_tail;
		tail->next = nullptr;
		list_size--;
	}

	inline void unlink(T* value) noexcept {
		value->prev->next = value->next;
		value->next->prev = value->prev;
		value->next = nullptr;
		value->prev = nullptr;
		value->linked = true;
		list_size--;
	}

};

#endif /* INTRUSIVELIST_H */

