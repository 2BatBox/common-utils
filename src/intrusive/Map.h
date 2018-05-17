#ifndef INTRUSIVE_MAP_H
#define INTRUSIVE_MAP_H

#include <memory>

namespace intrusive {

template <typename K, typename V>
struct MapHook {
	V* im_next;
	K im_key;
	bool im_linked;

	MapHook() noexcept: im_next(nullptr), im_key(), im_linked(false) { }
	MapHook(const MapHook&) noexcept = default;
	MapHook& operator=(const MapHook&)noexcept = default;
	virtual ~MapHook() noexcept = default;
};

template <typename MapData_t>
struct MapBucket {
	MapData_t* head;
	size_t size;

	MapBucket() noexcept: head(nullptr), size(0) { }
};

template <typename K, typename MapData_t, typename H = std::hash<K>, typename A = std::allocator<MapBucket<MapData_t> > >
class Map {
public:
	typedef MapBucket<MapData_t> Bucket_t;

private:
	Bucket_t * bucket_list;
	size_t bucket_list_size;
	size_t elements;
	H hasher;
	A allocator;

	template<typename V>
	struct Iterator {
		friend class Map;

		Iterator() noexcept: value(nullptr) { }

		Iterator(V* value) noexcept: value(value) { }

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

		Iterator operator++(int)noexcept {
			value = value->im_next;
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

public:
	
	typedef Iterator<MapData_t> Iterator_t;
	typedef Iterator<const MapData_t> ConstIterator_t;

	Map(size_t bucket_list_size) noexcept:
	bucket_list(nullptr),
	bucket_list_size(bucket_list_size),
	elements(0),
	hasher(),
	allocator() { }

	Map(const Map&) = delete;
	Map& operator=(const Map&) = delete;

	Map(Map&& rv) noexcept:
	bucket_list(rv.bucket_list),
	bucket_list_size(rv.bucket_list_size),
	elements(rv.elements),
	hasher(rv.hasher),
	allocator(rv.allocator) {
		rv.bucket_list = nullptr;
		rv.destroy();
	}

	Map& operator=(Map&& rv) noexcept {
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

	virtual ~Map() noexcept {
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

	Iterator_t put(const K& key, MapData_t& value) noexcept {
		MapData_t* result = nullptr;
		if (sanity_check(value)) {
			size_t index = hasher(key) % bucket_list_size;
			result = find(bucket_list[index], key);
			if (result == nullptr) 
				link_front(bucket_list[index], key, value);
		}
		return Iterator_t(result);
	}

	Iterator_t find(const K& key) noexcept {
		size_t index = hasher(key) % bucket_list_size;
		return Iterator_t(find(bucket_list[index], key));
	}

	ConstIterator_t find(const K& key) const noexcept {
		size_t index = hasher(key) % bucket_list_size;
		return ConstIterator_t(find(bucket_list[index], key));
	}

	Iterator_t remove(const K& key) noexcept {
		size_t index = hasher(key) % bucket_list_size;
		Bucket_t& bucket = bucket_list[index];
		MapData_t* prev = nullptr;
		MapData_t* result = find(bucket, key, prev);
		if (result) {
			if (result == bucket.head)
				unlink_front(bucket);
			else
				unlink_next(bucket, *prev);
		}
		return Iterator_t(result);
	}

	void reset() noexcept {
		for (size_t i = 0; i < bucket_list_size; i++) {
			while (bucket_list[i].head)
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
		return Iterator_t(bucket_list[bucket].head);
	}

	ConstIterator_t cbegin(size_t bucket) const noexcept {
		return ConstIterator_t(bucket_list[bucket].head);
	}

	Iterator_t end() noexcept {
		return Iterator_t();
	}

	ConstIterator_t cend() const noexcept {
		return ConstIterator_t();
	}

private:

	inline static bool sanity_check(const MapData_t& value) noexcept {
		return (not value.im_linked);
	}

	inline void link_front(Bucket_t& bucket, const K& key, MapData_t& value) noexcept {
		value.im_next = bucket.head;
		value.im_linked = true;
		value.im_key = key;
		bucket.head = &value;
		bucket.size++;
		elements++;
	}

	inline void unlink_front(Bucket_t& bucket) noexcept {
		MapData_t* tmp_value = bucket.head;
		bucket.head = bucket.head->im_next;
		tmp_value->im_next = nullptr;
		tmp_value->im_linked = false;
		bucket.size--;
		elements--;
	}

	inline void unlink_next(Bucket_t& bucket, MapData_t& value) noexcept {
		MapData_t* tmp_value = value.im_next;
		value.im_next = value.im_next->im_next;
		tmp_value->im_next = nullptr;
		tmp_value->im_linked = false;
		bucket.size--;
		elements--;
	}

	inline MapData_t* find(Bucket_t& bucket, const K& key) noexcept {
		MapData_t* cur = bucket.head;
		while (cur) {
			if (cur->im_key == key)
				break;
			cur = cur->im_next;
		}
		return cur;
	}

	inline const MapData_t* find(Bucket_t& bucket, const K& key) const noexcept {
		MapData_t* cur = bucket.head;
		while (cur) {
			if (cur->im_key == key)
				break;
			cur = cur->im_next;
		}
		return cur;
	}

	inline MapData_t* find(Bucket_t& bucket, const K& key, MapData_t*& prev) noexcept {
		MapData_t* cur = bucket.head;
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
			reset();
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

}; // namespace intrusive

#endif /* INTRUSIVE_MAP_H */

