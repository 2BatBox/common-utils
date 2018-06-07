#ifndef INTRUSIVE_TESTS_TEST_MAP_H
#define INTRUSIVE_TESTS_TEST_MAP_H

#include "../Map.h"

#include <assert.h>
#include <cstdio>

namespace intrusive {

class TestMap {

	template<typename T>
	struct StructValue {
		T value;

		StructValue() : value() { }

		StructValue(unsigned int x) : value(x) { }

		bool operator==(const StructValue& st_val) const {
			return value == st_val.value;
		}

	};

	template <typename K, typename V>
	struct MapNode : public MapHook<K, MapNode<K, V> > {
		V value;

		MapNode() : value() { }

		explicit MapNode(V v) : value(v) { }

		bool operator==(const MapNode& data) const {
			return value == data.value;
		}

	};

	using Key_t = unsigned;
	using Value_t = StructValue<unsigned long long>;

	using MapNode_t = MapNode<Key_t, Value_t>;
	using Map_t = Map<Key_t, MapNode_t>;
	using Bucket_t = Map_t::Bucket_t;

	const size_t storage_size;
	MapNode_t* storage;
	const size_t bucket_list_size;
	Map_t map;

public:

	TestMap(unsigned storage_size, float load_factor)
	: storage_size(storage_size),
	storage(new MapNode_t[storage_size]),
	bucket_list_size((storage_size / load_factor) + 1),
	map(bucket_list_size) {
		if (not map.allocate())
			throw std::logic_error("Cannot allocate Map instance");
	}

	TestMap(const TestMap&) = delete;
	TestMap(TestMap&&) = delete;

	TestMap operator=(const TestMap&) = delete;
	TestMap operator=(TestMap&&) = delete;

	~TestMap() {
		// The map must be empty before the storage has been destroyed.
		map.clear();
		delete [] storage;
	}

	size_t storage_bytes() {
		return storage_size * sizeof (MapNode_t) + bucket_list_size * sizeof (Bucket_t);
	}

	void test() {
		printf("<intrusive::MapTest>...\n");
		printf("sizeof(Value_t)=%zu\n", sizeof (Value_t));
		printf("sizeof(MapData_t)=%zu\n", sizeof (MapNode_t));
		printf("sizeof(Bucket_t)=%zu\n", sizeof (Bucket_t));
		printf("storage_size=%zu\n", storage_size);
		printf("bucket_list_size=%zu\n", bucket_list_size);
		printf("memory used %zu Kb\n", storage_bytes() / (1024));
		test_raii();
		test_put_find_remove();
		test_clear();
	}

	void test_raii() {
		assert(map.size() == 0);
		for (size_t i = 0; i < storage_size; i++) {
			put_once(i, i, i);
		}
		Map_t tmp_map(std::move(map));
		assert(map.size() == 0);
		assert(tmp_map.size() == storage_size);
		map = std::move(map);
		map = std::move(tmp_map);
		std::swap(map, tmp_map);
		std::swap(map, tmp_map);
		for (size_t i = 0; i < storage_size; i++) {
			find_once(i, i);
			remove_once(i, i);
		}
		assert(map.size() == 0);
		test_sanity();
	}

	void test_put_find_remove() {
		assert(map.size() == 0);
		unsigned step = 0;

		// forward
		step++;
		for (size_t i = 0; i < storage_size; i++) {
			put_once(i, i * step, i + step);
		}
		for (size_t i = 0; i < storage_size; i++) {
			find_once(i * step, i + step);
			find_once_const(i * step, i + step);
		}
		for (size_t i = 0; i < storage_size; i++) {
			remove_once(i * step, i + step);
		}
		assert(map.size() == 0);
		test_sanity();

		// backward
		step++;
		for (size_t i = 0; i < storage_size; i++) {
			size_t index = storage_size - i - 1;
			put_once(index, i * step, i + step);
		}
		for (size_t i = 0; i < storage_size; i++) {
			find_once(i * step, i + step);
			find_once_const(i * step, i + step);
		}
		for (size_t i = 0; i < storage_size; i++) {
			remove_once(i * step, i + step);
		}
		assert(map.size() == 0);
		test_sanity();

		// odd
		step++;
		for (size_t i = 0; i < storage_size; i++) {
			if (i % 2 != 0) {
				put_once(i, i * step, i + step);
			}
		}
		for (size_t i = 0; i < storage_size; i++) {
			if (i % 2 != 0) {
				find_once(i * step, i + step);
				find_once_const(i * step, i + step);
			} else {
				miss_once(i * step);
			}
		}
		for (size_t i = 0; i < storage_size; i++) {
			if (i % 2 != 0) {
				remove_once(i * step, i + step);
			}
		}
		assert(map.size() == 0);
		test_sanity();

		// even
		step++;
		for (size_t i = 0; i < storage_size; i++) {
			if (i % 2 == 0) {
				put_once(i, i * step, i + step);
			}
		}
		for (size_t i = 0; i < storage_size; i++) {
			if (i % 2 == 0) {
				find_once(i * step, i + step);
				find_once_const(i * step, i + step);
			} else {
				miss_once(i * step);
			}
		}
		for (size_t i = 0; i < storage_size; i++) {
			if (i % 2 == 0) {
				remove_once(i * step, i + step);
			}
		}
		assert(map.size() == 0);
		test_sanity();
	}

	void test_clear() {
		for (size_t i = 0; i < storage_size; i++) {
			put_once(i, i, i);
		}
		map.clear();
		assert(map.size() == 0);
		test_sanity();
	}

	void dump() {
		std::cout << "map has " << map.size() << " elements \n";
		for (size_t bucket = 0; bucket < map.buckets(); ++bucket) {
			std::cout << "B[" << bucket << "] ";
			for (auto it = map.cbegin(bucket); it != map.cend(); ++it) {
				std::cout << "[" << &(*it) << "] " << (*it).im_key << ":" << (*it).value.value << " -> ";
			}
			std::cout << "\n";
		}
	}

private:

	void put_once(size_t node_index, const Key_t& key, const Value_t& value) noexcept {
		MapNode_t& node = storage[node_index];
		node.value = value;
		auto it = map.put(key, node);
		assert(it != map.end());
		assert(*it == node);
		assert(it->value == value);
		assert(it->im_key == key);
		assert(it->im_linked);
	}

	void find_once(const Key_t& key, const Value_t& value) noexcept {
		auto it = map.find(key);
		assert(it != map.end());
		assert(it->value == value);
		assert(it->im_key == key);
		assert(it->im_linked);
	}

	void find_once_const(const Key_t& key, const Value_t& value) const noexcept {
		auto it = map.find(key);
		assert(it != map.cend());
		assert(it->value == value);
		assert(it->im_key == key);
		assert(it->im_linked);
	}

	void miss_once(const Key_t& key) noexcept {
		auto it = map.find(key);
		assert(it == map.end());
	}

	void remove_once(const Key_t& key, const Value_t& value) noexcept {
		auto it = map.find(key);
		assert(it != map.end());
		assert(it->value == value);
		assert(it->im_key == key);
		assert(it->im_linked);
		map.remove(it);
		assert(not it->im_linked);
	}

	void test_sanity() {
		for (unsigned i = 0; i < storage_size; i++) {
			assert(not storage[i].im_linked);
		}
	}

};

}; // namespace intrusive

#endif /* INTRUSIVE_TESTS_TEST_MAP_H */

