#ifndef INTRUSIVEMAP_H
#define INTRUSIVEMAP_H

#include <memory>

template <typename V>
struct IntrusiveMapBucket {
	V* list;
	size_t size;

	IntrusiveMapBucket() noexcept : list(nullptr), size(0) { }
};

template <typename K, typename V, typename A = std::allocator<IntrusiveMapBucket<V> >, typename H = std::hash<K> >
class IntrusiveMap {
public:
	typedef IntrusiveMapBucket<V> Bucket_t;

	struct Hook {
		V* im_next;
		K im_key;
		bool im_linked;

		Hook() noexcept : im_next(nullptr), im_key(), im_linked(false) { }
	};

private:
	Bucket_t * bucket_list;
	size_t bucket_list_size;
	size_t elements;
	A allocator;
	H hasher;

	template<typename ITV>
	struct Iterator {
		friend class IntrusiveMap;

		Iterator() noexcept : value(nullptr) { }

		Iterator(ITV* value) noexcept : value(value) { }

		bool operator==(const Iterator& it) const noexcept {
			return value == it.value;
		}

		bool operator!=(const Iterator& it) const noexcept {
			return value != it.value;
		}

		Iterator& operator++() noexcept {
			value = value->im_next;
			return *this;
		}

		Iterator operator++(int) noexcept {
			value = value->im_next;
			return Iterator(value);
		}

		ITV& operator*() noexcept {
			return *value;
		}

		ITV* operator->() noexcept {
			return value;
		}

		const ITV& operator*() const noexcept {
			return *value;
		}

		const ITV* operator->() const noexcept {
			return value;
		}

	private:
		ITV* value;
	};

	typedef Iterator<V> Iterator_t;
	typedef Iterator<const V> ConstIterator_t;

public:

	IntrusiveMap(size_t bucket_list_size) noexcept :
	bucket_list(nullptr),
	bucket_list_size(bucket_list_size),
	elements(0),
	allocator(),
	hasher() { }

	IntrusiveMap(const IntrusiveMap&) = delete;
	IntrusiveMap& operator=(const IntrusiveMap&) = delete;

	IntrusiveMap(IntrusiveMap&& rv) noexcept :
	bucket_list(rv.bucket_list),
	bucket_list_size(rv.bucket_list_size),
	elements(rv.elements),
	allocator(rv.allocator),
	hasher(rv.hasher) {
		rv.bucket_list = nullptr;
		rv.destroy();
	}

	IntrusiveMap& operator=(IntrusiveMap&& rv) noexcept {
		if (this != &rv) {
			destroy();
			bucket_list = rv.bucket_list;
			bucket_list_size = rv.bucket_list_size;
			elements = rv.elements;
			allocator = rv.allocator;
			hasher = rv.hasher;

			rv.bucket_list = nullptr;
			rv.destroy();
		}
		return *this;
	}

	virtual ~IntrusiveMap() noexcept {
		destroy();
	}

	bool allocate() noexcept {
		if (bucket_list)
			return false;

		Bucket_t empty;
		bucket_list = allocator.allocate(bucket_list_size);
		if (bucket_list) {
			for (size_t i = 0; i < bucket_list_size; i++) {
				allocator.construct(bucket_list + i, empty);
			}
		}

		return bucket_list != nullptr;
	}

	bool put(K key, V& value) noexcept {
		if (sanity_check(value)) {
			size_t index = hasher(key) % bucket_list_size;
			if (find(bucket_list[index], key) == nullptr) {
				link_front(bucket_list[index], key, value);
				return true;
			}
		}
		return false;
	}

	V* find(K key) noexcept {
		size_t index = key % bucket_list_size;
		return find(bucket_list[index], key);
	}

	const V* find(K key) const noexcept {
		size_t index = key % bucket_list_size;
		return find(bucket_list[index], key);
	}

	bool remove(K key) noexcept {
		size_t index = hasher(key) % bucket_list_size;
		Bucket_t& bucket = bucket_list[index];
		V* prev = nullptr;
		V* result = find(bucket, key, prev);
		if (result) {
			if (result == bucket.list)
				unlink_front(bucket);
			else
				unlink_next(bucket, *prev);
			return true;
		}
		return false;
	}

	void reset() noexcept {
		for (size_t i = 0; i < bucket_list_size; i++) {
			while (bucket_list[i].list)
				unlink_front(bucket_list[i]);
		}
	}

	size_t size() const noexcept {
		return elements;
	}

	size_t buckets() const noexcept {
		return bucket_list_size;
	}

	Iterator_t begin(size_t bucket) noexcept {
		return Iterator_t(bucket_list[bucket].list);
	}

	ConstIterator_t cbegin(size_t bucket) const noexcept {
		return ConstIterator_t(bucket_list[bucket].list);
	}

	Iterator_t end() noexcept {
		return Iterator_t();
	}

	ConstIterator_t cend() const noexcept {
		return ConstIterator_t();
	}

private:

	inline static bool sanity_check(const V& value) noexcept {
		return (not value.im_linked);
	}

	inline void link_front(Bucket_t& bucket, K key, V& value) noexcept {
		value.im_next = bucket.list;
		value.im_linked = true;
		value.im_key = key;
		bucket.list = &value;
		bucket.size++;
		elements++;
	}

	inline void unlink_front(Bucket_t& bucket) noexcept {
		V* tmp_value = bucket.list;
		bucket.list = bucket.list->im_next;
		tmp_value->im_next = nullptr;
		tmp_value->im_linked = false;
		bucket.size--;
		elements--;
	}

	inline void unlink_next(Bucket_t& bucket, V& value) noexcept {
		V* tmp_value = value.im_next;
		value.im_next = value.im_next->im_next;
		tmp_value->im_next = nullptr;
		tmp_value->im_linked = false;
		bucket.size--;
		elements--;
	}

	inline V* find(Bucket_t& bucket, K key) noexcept {
		V* cur = bucket.list;
		while (cur) {
			if (cur->im_key == key)
				break;
			cur = cur->im_next;
		}
		return cur;
	}

	inline const V* find(Bucket_t& bucket, K key) const noexcept {
		V* cur = bucket.list;
		while (cur) {
			if (cur->im_key == key)
				break;
			cur = cur->im_next;
		}
		return cur;
	}

	inline V* find(Bucket_t& bucket, K key, V*& prev) noexcept {
		V* cur = bucket.list;
		while (cur) {
			if (cur->im_key == key)
				break;
			prev = cur;
			cur = cur->im_next;
		}
		return cur;
	}

private:

	void destroy() noexcept {
		if (bucket_list) {
			for (size_t i = 0; i < bucket_list_size; i++) {
				allocator.destroy(bucket_list + i);
			}
			allocator.deallocate(bucket_list, bucket_list_size);
			bucket_list = nullptr;
		}
		bucket_list_size = 0;
		elements = 0;
	}

};

#endif /* INTRUSIVEMAP_H */
