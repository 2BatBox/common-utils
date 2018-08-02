#ifndef INTRUSIVE_MAP_H
#define INTRUSIVE_MAP_H

#include <memory>

namespace intrusive {

template <typename K, typename V>
struct MapHook {
	V* im_next;
	K im_key;
	bool im_linked;

	MapHook() noexcept : im_next(nullptr), im_key(), im_linked(false) { }

	MapHook(const MapHook&) = delete;
	MapHook& operator=(const MapHook&) = delete;

	MapHook(MapHook&&) = delete;
	MapHook& operator=(MapHook&&) = delete;
};

template <typename MapData_t>
struct MapBucket {
	MapData_t* head;
	size_t size;

	MapBucket() noexcept : head(nullptr), size(0) { }

	MapBucket(const MapBucket&) = delete;
	MapBucket& operator=(const MapBucket&) = delete;

	MapBucket(MapBucket&&) = delete;
	MapBucket& operator=(MapBucket&&) = delete;
};

template <typename K, typename MapNode, typename H = std::hash<K>, typename A = std::allocator<MapBucket<MapNode> > >
class Map {
public:
	using Bucket_t = MapBucket<MapNode>;

private:
	Bucket_t * bucket_list;
	size_t bucket_list_size;
	size_t elements;
	H hasher;
	A allocator;

	template<typename N>
	struct Iterator {
		friend class Map;

		Iterator() noexcept : m_node(nullptr) { }

		Iterator(N* node) noexcept : m_node(node) { }

		inline bool operator==(const Iterator& it) const noexcept {
			return m_node == it.m_node;
		}

		inline bool operator!=(const Iterator& it) const noexcept {
			return m_node != it.m_node;
		}

		inline Iterator& operator++() noexcept {
			m_node = m_node->im_next;
			return *this;
		}

		inline Iterator operator++(int)noexcept {
			m_node = m_node->im_next;
			return Iterator(m_node);
		}

		inline const N& operator*() const noexcept {
			return *m_node;
		}

		inline N& operator*() noexcept {
			return *m_node;
		}

		inline const N* operator->() const noexcept {
			return m_node;
		}

		inline N* operator->() noexcept {
			return m_node;
		}

		inline const K& key() noexcept {
			return m_node->im_key;
		}

		inline operator bool() const noexcept {
			return m_node;
		}

	private:
		N* m_node;
	};

public:

	using Iterator_t = Iterator<MapNode>;
	using ConstIterator_t = Iterator<const MapNode>;

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
			rv.clean_state();
		}
		return *this;
	}

	/**
	 * Be careful, The map must be empty before the storage has been destroyed.
	 * That means the method 'clean()' must be called before destroying the storage.
	 */
	virtual ~Map() noexcept {
		destroy();
	}

	/**
	 * Allocate the bucket storage of the map.
	 * @return true - if the bucket storage has been allocated successfully.
	 */
	bool allocate() noexcept {
		if (bucket_list)
			return false;

		bucket_list = allocator.allocate(bucket_list_size);
		if (bucket_list) {
			for (size_t i = 0; i < bucket_list_size; i++) {
				allocator.construct(bucket_list + i);
			}
		}
		return bucket_list != nullptr;
	}

	/**
	 * Unlink all the objects the map contains.
	 */
	void clear() noexcept {
		for (size_t i = 0; i < bucket_list_size; i++) {
			while (bucket_list[i].head)
				unlink_front(i);
		}
	}

	inline Iterator_t put(const K& key, MapNode& free_node) noexcept {
		bool recycled;
		return put(key, free_node, recycled);
	}

	Iterator_t put(const K& key, MapNode& free_node, bool& recycled) noexcept {
		MapNode* result = nullptr;
		if (sanity_check(free_node)) {
			size_t bucket_id = hasher(key) % bucket_list_size;
			result = find(bucket_id, key);
			if (result == nullptr) {
				link_front(bucket_id, key, free_node);
				result = &free_node;
				recycled = false;
			}
		}
		return Iterator_t(result);
	}

	ConstIterator_t find(const K& key) const noexcept {
		size_t bucket_id = hasher(key) % bucket_list_size;
		return ConstIterator_t(find(bucket_id, key));
	}

	Iterator_t find(const K& key) noexcept {
		size_t bucket_id = hasher(key) % bucket_list_size;
		return Iterator_t(find(bucket_id, key));
	}

	void remove(Iterator_t it) noexcept {
		MapNode* node = it.m_node;
		size_t bucket_id = hasher(node->im_key) % bucket_list_size;
		if (node == bucket_list[bucket_id].head) {
			unlink_front(bucket_id);
		} else {
			MapNode* prev = find_prev(bucket_id, node->im_key);
			unlink_next(bucket_id, *prev);
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
		return ConstIterator_t(bucket_list[bucket].head);
	}

	inline Iterator_t end() noexcept {
		return Iterator_t();
	}

	inline ConstIterator_t cend() const noexcept {
		return ConstIterator_t();
	}

private:

	void destroy() noexcept {
		if (bucket_list) {
			clear();
			for (size_t i = 0; i < bucket_list_size; i++) {
				allocator.destroy(bucket_list + i);
			}
			allocator.deallocate(bucket_list, bucket_list_size);
		}
		clean_state();
	}

	inline static bool sanity_check(const MapNode& node) noexcept {
		return (not node.im_linked);
	}

	inline void link_front(size_t bucket_id, const K& key, MapNode& node) noexcept {
		Bucket_t& bucket = bucket_list[bucket_id];
		node.im_next = bucket.head;
		node.im_linked = true;
		node.im_key = key;
		bucket.head = &node;
		bucket.size++;
		elements++;
	}

	inline void unlink_front(size_t bucket_id) noexcept {
		Bucket_t& bucket = bucket_list[bucket_id];
		MapNode* tmp_value = bucket.head;
		bucket.head = bucket.head->im_next;
		tmp_value->im_next = nullptr;
		tmp_value->im_linked = false;
		bucket.size--;
		elements--;
	}

	inline void unlink_next(size_t bucket_id, MapNode& node) noexcept {
		Bucket_t& bucket = bucket_list[bucket_id];
		MapNode* tmp_value = node.im_next;
		node.im_next = node.im_next->im_next;
		tmp_value->im_next = nullptr;
		tmp_value->im_linked = false;
		bucket.size--;
		elements--;
	}

	inline MapNode* find(size_t bucket_id, const K& key) noexcept {
		MapNode* cur = bucket_list[bucket_id].head;
		while (cur) {
			if (cur->im_key == key)
				break;
			cur = cur->im_next;
		}
		return cur;
	}

	inline const MapNode* find(size_t bucket_id, const K& key) const noexcept {
		MapNode* cur = bucket_list[bucket_id].head;
		while (cur) {
			if (cur->im_key == key) {
				break;
			}
			cur = cur->im_next;
		}
		return cur;
	}

	inline MapNode* find_prev(size_t bucket_id, const K& key) const noexcept {
		MapNode* cur = bucket_list[bucket_id].head;
		MapNode* prev = nullptr;
		while (cur) {
			if (cur->im_key == key) {
				return prev;
			}
			prev = cur;
			cur = cur->im_next;
		}
		return nullptr;
	}

	inline void clean_state() noexcept {
		bucket_list = nullptr;
		bucket_list_size = 0;
		elements = 0;
	}

};

}; // namespace intrusive

#endif /* INTRUSIVE_MAP_H */

