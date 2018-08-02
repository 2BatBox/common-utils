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

		unsigned step = 1;
		test_put_remove_forward(step++);
		test_put_remove_backward(step++);
		test_put_remove_odd_even(step++);
		test_recycle(step++);
		test_clear(step++);
		test_raii();
	}

	void test_put_remove_forward(unsigned step) noexcept {
		printf("-> test_put_remove_forward()\n");
		assert(map.size() == 0);

		for (size_t i = 0; i < storage_size; i++) {
			put_one(i, i * step, i + step);
		}
		for (size_t i = 0; i < storage_size; i++) {
			find_one(i * step, i + step);
		}
		for (size_t i = 0; i < storage_size; i++) {
			remove_one(i * step, i + step);
			miss_one(i * step);
		}
		assert(map.size() == 0);
		test_sanity();
	}

	void test_put_remove_backward(unsigned step) noexcept {
		printf("-> test_put_remove_backward()\n");
		assert(map.size() == 0);

		for (size_t i = 0; i < storage_size; i++) {
			size_t index = storage_size - i - 1;
			put_one(index, i * step, i + step);
		}
		for (size_t i = 0; i < storage_size; i++) {
			find_one(i * step, i + step);
		}
		for (size_t i = 0; i < storage_size; i++) {
			remove_one(i * step, i + step);
			miss_one(i * step);
		}
		assert(map.size() == 0);
		test_sanity();
	}

	void test_put_remove_odd_even(unsigned step) noexcept {
		printf("-> test_put_remove_odd_even()\n");
		assert(map.size() == 0);

		for (size_t i = 0; i < storage_size; i++) {
			if (i % 2 != 0) {
				put_one(i, i * step, i + step);
			}
		}

		for (size_t i = 0; i < storage_size; i++) {
			if (i % 2 == 0) {
				put_one(i, i * step, i + step + 1);
			}
		}

		for (size_t i = 0; i < storage_size; i++) {
			if (i % 2 != 0) {
				find_one(i * step, i + step);
			} else {
				find_one(i * step, i + step + 1);
			}
		}

		for (size_t i = 0; i < storage_size; i++) {
			if (i % 2 != 0) {
				remove_one(i * step, i + step);
			} else {
				remove_one(i * step, i + step + 1);
			}
			miss_one(i * step);
		}
		assert(map.size() == 0);
		test_sanity();
	}

	void test_recycle(unsigned step) noexcept {
		printf("-> test_recycle()\n");
		for (size_t i = 0; i < storage_size; i++) {
			if (i % 2 == 0) {
				put_one(i, i, i);
			} else {
				put_one_recycled(i - 1, i - 1, (i - 1) * step);
			}
		}

		for (size_t i = 0; i < storage_size; i++) {
			if (i % 2 == 0) {
				find_one(i, i * step);
				remove_one(i, i * step);
				miss_one(i);
			} else {
				miss_one(i);
			}

		}
		assert(map.size() == 0);
		test_sanity();
	}

	void test_clear(unsigned step) noexcept {
		printf("-> test_clear()\n");

		for (size_t i = 0; i < storage_size; i++) {
			put_one(i, i + step, i + step);
		}
		map.clear();
		assert(map.size() == 0);
		test_sanity();
	}

	void test_raii() noexcept {
		printf("-> test_raii()\n");

		assert(map.size() == 0);
		for (size_t i = 0; i < storage_size; i++) {
			put_one(i, i, i);
		}
		Map_t tmp_map(std::move(map));
		assert(map.size() == 0);
		assert(tmp_map.size() == storage_size);
		map = std::move(map);
		map = std::move(tmp_map);
		std::swap(map, tmp_map);
		std::swap(map, tmp_map);
		for (size_t i = 0; i < storage_size; i++) {
			find_one(i, i);
			remove_one(i, i);
		}
		assert(map.size() == 0);
		test_sanity();
	}

	void dump() noexcept {
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

	void put_one(size_t node_index, const Key_t& key, const Value_t& value) noexcept {
		bool recycled = true;
		MapNode_t& node = storage[node_index];
		auto it = map.put(key, node, recycled);
		assert(it != map.end());
		assert(not recycled);
		it->value = value;
		assert(*it == node);
		assert(it->im_key == key);
		assert(it->im_linked);
	}

	void put_one_recycled(size_t node_index, const Key_t& key, const Value_t& value) noexcept {
		bool recycled = false;
		MapNode_t tmp;
		MapNode_t& node_recycled = storage[node_index];
		auto it = map.put(key, tmp);
		assert(it != map.end());
		assert(not recycled);
		it->value = value;
		assert(*it == node_recycled);
		assert(it->im_key == key);
		assert(it->im_linked);
	}

	void find_one(const Key_t& key, const Value_t& value) noexcept {
		auto it = map.find(key);
		assert(it != map.end());
		assert(it->value == value);
		assert(it->im_key == key);
		assert(it->im_linked);
	}

	void miss_one(const Key_t& key) noexcept {
		auto it = map.find(key);
		assert(it == map.end());
	}

	void remove_one(const Key_t& key, const Value_t& value) noexcept {
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

