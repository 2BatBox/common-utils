#ifndef INTRUSIVE_TESTS_TEST_MAP_H
#define INTRUSIVE_TESTS_TEST_MAP_H

#include "../Map.h"

#include <assert.h>
#include <cstdio>

#pragma GCC diagnostic ignored "-Wnon-virtual-dtor"
#pragma GCC diagnostic ignored "-Weffc++"

namespace intrusive {

class TestMap {

	template<typename T>
	struct StructValue {
		T x;

		StructValue() : x() { }

		StructValue(unsigned int x) : x(x) { }

		bool operator==(const StructValue& st_val) const {
			return x == st_val.x;
		}

	};

	template <typename K, typename V>
	struct MapData : public MapHook<K, MapData<K, V> > {
		V value;

		MapData() : value() { }

		MapData(V v) : value(v) { }

		bool operator==(const MapData& data) const {
			return value == data.value;
		}

	};

	using Key_t = unsigned;
	using Value_t = StructValue<unsigned long long>;

	using MapData_t = MapData<Key_t, Value_t>;
	using Map_t = Map<Key_t, MapData_t>;
	using Bucket_t = Map_t::Bucket_t;

	const size_t storage_size;
	MapData_t* storage;
	const size_t bucket_list_size;
	Map_t map;

public:

	TestMap(unsigned storage_size, float load_factor)
	: storage_size(storage_size),
	storage(new MapData_t[storage_size]),
	bucket_list_size((storage_size / load_factor) + 1),
	map(bucket_list_size) {

		if (not map.allocate())
			throw std::logic_error("Cannot allocate Map instance");
		for (unsigned i = 0; i < storage_size; i++) {
			storage[i].value = i;
		}
	}

	TestMap(const TestMap&) = delete;
	TestMap(TestMap&&) = delete;

	TestMap operator=(const TestMap&) = delete;
	TestMap operator=(TestMap&&) = delete;

	~TestMap() {
		// the map must be destroyed before the storage has been destroyed.
		map.destroy();
		delete [] storage;
	}

	size_t storage_bytes() {
		return storage_size * sizeof (MapData_t) + bucket_list_size * sizeof (Bucket_t);
	}

	void test() {
		printf("<intrusive::MapTest>...\n");
		printf("sizeof(Value_t)=%zu\n", sizeof (Value_t));
		printf("sizeof(MapData_t)=%zu\n", sizeof (MapData_t));
		printf("sizeof(Bucket_t)=%zu\n", sizeof (Bucket_t));
		printf("storage_size=%zu\n", storage_size);
		printf("bucket_list_size=%zu\n", bucket_list_size);
		printf("memory used %zu Kb\n", storage_bytes() / (1024));
		test_raii();
		test_put();
		test_find();
		test_find_const();
		test_remove();
	}

	void test_raii() {
		assert(map.size() == 0);
		fill(map);
		Map_t tmp_map(std::move(map));
		assert(map.size() == 0);
		assert(tmp_map.size() == storage_size);
		for (Key_t i = 0; i < storage_size; i++) {
			auto it = tmp_map.find(i);
			assert(it != tmp_map.end());
			assert((*it) == storage[i]);
			assert(it->value == storage[i].value);
		}
		map = std::move(map);
		map = std::move(tmp_map);
		std::swap(map, tmp_map);
		std::swap(map, tmp_map);
		clear(map);
	}

	void test_put() {
		assert(map.size() == 0);
		fill(map);
		for (Key_t i = 0; i < storage_size; i++) {
			auto it = map.put(i, storage[i]);
			assert(it == map.end());
		}
		clear(map);
	}

	void test_find() {
		assert(map.size() == 0);
		fill(map);
		for (Key_t i = 0; i < storage_size; i++) {
			auto it = map.find(i);
			assert(it != map.end());
			assert(*it == storage[i]);
			assert(it->value == storage[i].value);
		}
		clear(map);
		for (Key_t i = 0; i < storage_size; i++) {
			auto it = map.find(i);
			assert(it == map.end());
		}
	}

	void test_find_const() {
		assert(map.size() == 0);
		fill(map);

		const Map_t& local_map = map;
		for (Key_t i = 0; i < storage_size; i++) {
			auto it = local_map.find(i);
			assert(it != local_map.cend());
			assert(*it == storage[i]);
			assert(it->value == storage[i].value);
		}
		clear(map);
		for (Key_t i = 0; i < storage_size; i++) {
			auto it = local_map.find(i);
			assert(it == local_map.cend());
		}
	}

	void test_remove() {
		assert(map.size() == 0);
		fill(map);
		for (Key_t i = 0; i < storage_size; i++) {
			auto it = map.find(i);
			assert(it != map.end());
			assert(*it == storage[i]);
			assert(it->value == storage[i].value);
			assert(map.remove(it));
		}
		assert(map.size() == 0);
		test_sanity();
	}

	void dump() {
		std::cout << "map has " << map.size() << " elements \n";
		for (size_t bucket = 0; bucket < map.buckets(); ++bucket) {
			std::cout << "B[" << bucket << "] ";
			for (auto it = map.cbegin(bucket); it != map.cend(); ++it) {
				std::cout << (*it).value.x << " ";
			}
			std::cout << "\n";
		}
	}

private:

	void test_sanity() {
		for (unsigned i = 0; i < storage_size; i++) {
			assert(not storage[i].im_linked);
		}
	}

	void fill(Map_t& map) {
		for (Key_t i = 0; i < storage_size; i++) {
			auto it = map.put(i, storage[i]);
			assert(it != map.end());
			assert(*it == storage[i]);
			assert(it->value == storage[i].value);
		}
		assert(map.size() == storage_size);
	}

	void clear(Map_t& map) {
		map.clear();
		assert(map.size() == 0);
		test_sanity();
	}

};

}; // namespace intrusive

#endif /* INTRUSIVE_TESTS_TEST_MAP_H */

