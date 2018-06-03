#ifndef INTRUSIVE_MAP_H
#define INTRUSIVE_MAP_H

#include <memory>

// gcc 4.8.2's -Wnon-virtual-dtor is broken and turned on by -Weffc++
#if __GNUC__ < 3 || (__GNUC__ == 4 && __GNUC_MINOR__ <= 8)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnon-virtual-dtor"
#pragma GCC diagnostic ignored "-Weffc++"
#define GCC_DIAG_POP_NEEDED
#endif

namespace intrusive {

template <typename K, typename V>
struct MapHook {
	V* im_next;
	K im_key;
	bool im_linked;

	MapHook() noexcept : im_next(nullptr), im_key(), im_linked(false) { }
};

template <typename MapData_t>
struct MapBucket {
	MapData_t* head;
	size_t size;

	MapBucket() noexcept : head(nullptr), size(0) { }
};

template <typename K, typename MapNode, typename H = std::hash<K>, typename A = std::allocator<MapBucket<MapNode> > >
class Map {
public:
	typedef MapBucket<MapNode> Bucket_t;

private:
	Bucket_t * bucket_list;
	size_t bucket_list_size;
	size_t elements;
	H hasher;
	A allocator;

	template<typename N>
	struct Iterator {
		friend class Map;

		Iterator() noexcept : node_ptr(nullptr), bucket_index(bucket_index) { }

		Iterator(N* node, size_t index) noexcept : node_ptr(node), bucket_index(index) { }

		bool operator==(const Iterator& it) const noexcept {
			return node_ptr == it.node_ptr;
		}

		bool operator!=(const Iterator& it) const noexcept {
			return node_ptr != it.node_ptr;
		}

		Iterator& operator++() noexcept {
			node_ptr = node_ptr->im_next;
			return *this;
		}

		Iterator operator++(int)noexcept {
			node_ptr = node_ptr->im_next;
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

		size_t index() const noexcept {
			return bucket_index;
		}

	private:
		N* node_ptr;
		size_t bucket_index;
	};

	using Iterator_t = Iterator<MapNode>;
	using ConstIterator_t = Iterator<const MapNode>;

public:

	Map(size_t bucket_list_size) noexcept :
	bucket_list(nullptr),
	bucket_list_size(bucket_list_size),
	elements(0),
	hasher(),
	allocator() { }

	Map(const Map&) = delete;
	Map& operator=(const Map&) = delete;

	Map(Map&& rv) noexcept :
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

	void destroy() noexcept {
		if (bucket_list) {
			clear();
			for (size_t i = 0; i < bucket_list_size; i++) {
				allocator.destroy(bucket_list + i);
			}
			allocator.deallocate(bucket_list, bucket_list_size);
			bucket_list = nullptr;
		}
		bucket_list_size = 0;
		elements = 0;
	}

	Iterator_t put(const K& key, MapNode& value) noexcept {
		MapNode* result = nullptr;
		size_t index = 0;
		if (sanity_check(value)) {
			index = hasher(key) % bucket_list_size;
			result = find(bucket_list[index], key);
			if (result == nullptr){
				link_front(bucket_list[index], key, value);
				result = &value;
			}
		}
		return Iterator_t(result, index);
	}

	ConstIterator_t find(const K& key) const noexcept {
		size_t index = hasher(key) % bucket_list_size;
		return ConstIterator_t(find(bucket_list[index], key), index);
	}

	Iterator_t find(const K& key) noexcept {
		size_t index = hasher(key) % bucket_list_size;
		return Iterator_t(find(bucket_list[index], key), index);
	}

	bool remove(Iterator_t it) noexcept {
		if (it->im_linked) {
			MapNode& node = *it;
			size_t index = it.index();
			Bucket_t& bucket = bucket_list[index];
			if (&node == bucket.head)
				unlink_front(bucket);
			else
				unlink_next(bucket, node);
			return true;
		}
		return false;
	}

	void clear() noexcept {
		for (size_t i = 0; i < bucket_list_size; i++) {
			while (bucket_list[i].head)
				unlink_front(bucket_list[i]);
		}
	}

	inline size_t size() const noexcept {
		return elements;
	}

	inline size_t buckets() const noexcept {
		return bucket_list_size;
	}

	inline Iterator_t begin(size_t bucket) noexcept {
		return Iterator_t(bucket_list[bucket].head, bucket);
	}

	inline ConstIterator_t cbegin(size_t bucket) const noexcept {
		return ConstIterator_t(bucket_list[bucket].head, bucket);
	}

	inline Iterator_t end() noexcept {
		return Iterator_t();
	}

	inline ConstIterator_t cend() const noexcept {
		return ConstIterator_t();
	}

private:

	inline static bool sanity_check(const MapNode& node) noexcept {
		return (not node.im_linked);
	}

	inline void link_front(Bucket_t& bucket, const K& key, MapNode& node) noexcept {
		node.im_next = bucket.head;
		node.im_linked = true;
		node.im_key = key;
		bucket.head = &node;
		bucket.size++;
		elements++;
	}

	inline void unlink_front(Bucket_t& bucket) noexcept {
		MapNode* tmp_value = bucket.head;
		bucket.head = bucket.head->im_next;
		tmp_value->im_next = nullptr;
		tmp_value->im_linked = false;
		bucket.size--;
		elements--;
	}

	inline void unlink_next(Bucket_t& bucket, MapNode& node) noexcept {
		MapNode* tmp_value = node.im_next;
		node.im_next = node.im_next->im_next;
		tmp_value->im_next = nullptr;
		tmp_value->im_linked = false;
		bucket.size--;
		elements--;
	}

	inline MapNode* find(Bucket_t& bucket, const K& key) noexcept {
		MapNode* cur = bucket.head;
		while (cur) {
			if (cur->im_key == key)
				break;
			cur = cur->im_next;
		}
		return cur;
	}

	inline const MapNode* find(Bucket_t& bucket, const K& key) const noexcept {
		MapNode* cur = bucket.head;
		while (cur) {
			if (cur->im_key == key)
				break;
			cur = cur->im_next;
		}
		return cur;
	}

};

}; // namespace intrusive

#if defined(GCC_DIAG_POP_NEEDED)
#pragma GCC diagnostic pop
#undef GCC_DIAG_POP_NEEDED
#endif

#endif /* INTRUSIVE_MAP_H */

