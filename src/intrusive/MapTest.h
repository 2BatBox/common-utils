#ifndef INTRUSIVE_MAP_TEST_H
#define INTRUSIVE_MAP_TEST_H

#include "Map.h"

#include <assert.h>
#include <cstdio>

namespace intrusive {

class MapTest {

	template<typename T>
	struct StructValue {
		T x;

		StructValue(): x() { }

		StructValue(unsigned int x): x(x) { }

		bool operator==(const StructValue& st_val) const {
			return x == st_val.x;
		}
	};

	typedef unsigned Key_t;
	typedef StructValue<long long unsigned> Value_t;

	typedef MapData<Key_t, Value_t> Data_t;
	typedef Map<Key_t, Data_t> Map_t;
	typedef Map_t::Bucket_t Bucket_t;

	const size_t storage_size;
	Data_t* storage;
	const size_t bucket_list_size;
	Map_t map;

public:

	MapTest(unsigned storage_size, float load_factor)
	: storage_size(storage_size),
	storage(new Data_t[storage_size]),
	bucket_list_size((storage_size / load_factor) + 1),
	map(bucket_list_size) {
		
		if (not map.allocate())
			throw std::logic_error("Cannot allocate Map instance");
		for (unsigned i = 0; i < storage_size; i++) {
			storage[i].value = i;
		}
	}

	MapTest(const MapTest&) = delete;
	MapTest(MapTest&&) = delete;

	MapTest operator=(const MapTest&) = delete;
	MapTest operator=(MapTest&&) = delete;

	~MapTest() {
		delete [] storage;
	}

	size_t storage_bytes() {
		return storage_size * sizeof (Data_t) + bucket_list_size * sizeof (Bucket_t);
	}

	void test() {
		printf("<intrusive::MapTest>...\n");
		printf("sizeof(Data_t)=%zu\n", sizeof (Data_t));
		printf("sizeof(Bucket_t)=%zu\n", sizeof (Bucket_t));
		printf("storage_size=%zu\n", storage_size);
		printf("bucket_list_size=%zu\n", bucket_list_size);
		printf("memory used %zu Kb\n", storage_bytes() / (1024));
		test_raii();
		test_put();
		test_remove();
		test_find();
	}

	void test_raii() {
		assert(map.size() == 0);
		fill(map);
		Map_t tmp_map(std::move(map));
		assert(map.size() == 0);
		assert(tmp_map.size() == storage_size);
		for (Key_t i = 0; i < storage_size; i++) {
			auto it = tmp_map.find(i);
			assert(it);
			assert((*it) == storage[i]);
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
			auto it = map.find(i);
			assert(it);
			assert((*it) == storage[i]);
		}
		clear(map);
	}

	void test_find() {
		assert(map.size() == 0);
		Key_t half = storage_size / 2;
		for (Key_t i = 0; i < half; i++) {
			assert(map.put(i, storage[i]));
		}
		assert(map.size() == half);
		for (Key_t i = 0; i < storage_size; i++) {
			auto it = map.find(i);
			if (i < half) {
				assert(it);
				assert((*it) == storage[i]);
			} else {
				assert(it == nullptr);
			}
		}
		clear(map);
	}

	void test_remove() {
		assert(map.size() == 0);
		fill(map);
		Key_t half = storage_size / 2;
		for (Key_t i = half; i < storage_size; i++) {
			assert(map.remove(i));
			assert(not map.remove(i));
		}
		for (Key_t i = 0; i < half; i++) {
			assert(map.remove(i));
			assert(not map.remove(i));
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
			assert(map.put(i, storage[i]));
		}
		assert(map.size() == storage_size);
	}

	void clear(Map_t& map) {
		map.reset();
		assert(map.size() == 0);
		test_sanity();
	}

};

}; // namespace intrusive

#endif /* INTRUSIVE_MAP_TEST_H */

