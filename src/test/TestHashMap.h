#ifndef INTRUSIVE_TESTS_TESTHASHMAP_H
#define INTRUSIVE_TESTS_TESTHASHMAP_H

#include <intrusive/HashMap.h>

#include <assert.h>
#include <cstdio>
#include <cstdlib>

namespace intrusive {

class TestHashMap {

	template<typename T>
	struct StructValue {
		T value;

		StructValue() : value() {}

		StructValue(unsigned int x) : value(x) {}

		bool operator==(const StructValue& st_val) const {
			return value == st_val.value;
		}

	};

	using Key_t = unsigned;
	using Value_t = StructValue<unsigned long long>;

	template<typename K>
	struct SimpleHasher {

		inline size_t operator()(const K& key) const {
			return size_t(key);
		}
	};

	template<typename K, typename V>
	struct MapNode : public HashMapHook<K, MapNode<K, V> > {
		V value;

		MapNode() : value() {}

		explicit MapNode(V v) : value(v) {}

		bool operator==(const MapNode& data) const {
			return value == data.value;
		}

	};

	using MapNode_t = MapNode<Key_t, Value_t>;
	using Map_t = HashMap<Key_t, MapNode_t, SimpleHasher<Key_t> >;
	using Bucket_t = Map_t::Bucket_t;

	const size_t storage_size;
	MapNode_t* storage;
	const size_t bucket_list_size;
	Map_t map_default;
	Map_t map_one_bucket;

public:

	TestHashMap(unsigned storage_size, float load_factor)
		: storage_size(storage_size), storage(new MapNode_t[storage_size]), bucket_list_size(
		(storage_size / load_factor) + 1), map_default(bucket_list_size), map_one_bucket(1) {
		if(not map_default.allocate())
			throw std::logic_error("Cannot allocate 'default map' instance");

		if(not map_one_bucket.allocate())
			throw std::logic_error("Cannot allocate 'map with one bucket' instance");
	}

	TestHashMap(const TestHashMap&) = delete;
	TestHashMap(TestHashMap&&) = delete;

	TestHashMap operator=(const TestHashMap&) = delete;
	TestHashMap operator=(TestHashMap&&) = delete;

	~TestHashMap() {
		// The map must be empty before the storage has been destroyed.
		map_default.clear();
		map_one_bucket.clear();
		delete[] storage;
	}

	size_t storage_bytes() {
		return storage_size * sizeof(MapNode_t) + bucket_list_size * sizeof(Bucket_t);
	}

	void test() {
		printf("<intrusive::MapTest>...\n");
		printf("sizeof(Value_t)=%zu\n", sizeof(Value_t));
		printf("sizeof(MapData_t)=%zu\n", sizeof(MapNode_t));
		printf("sizeof(Bucket_t)=%zu\n", sizeof(Bucket_t));
		printf("storage_size=%zu\n", storage_size);
		printf("bucket_list_size=%zu\n", bucket_list_size);
		printf("memory used %zu Kb\n", storage_bytes() / (1024));

		unsigned step = 1;
		test_put_remove_forward(map_default, step++);
		test_put_remove_forward(map_one_bucket, step++);

		test_put_remove_backward(map_default, step++);
		test_put_remove_backward(map_one_bucket, step++);

		test_put_remove_odd_even(map_default, step++);
		test_put_remove_odd_even(map_one_bucket, step++);

		test_put_remove_same_key(map_default, step++);
		test_put_remove_same_key(map_one_bucket, step++);

		test_clear(map_default, step++);
		test_clear(map_one_bucket, step++);

		test_raii(map_default);
		test_raii(map_one_bucket);
	}

	void test_put_remove_forward(Map_t& map, unsigned step) noexcept {
		printf("-> test_put_remove_forward()\n");
		assert(map.size() == 0);

		for(size_t i = 0; i < storage_size; i++) {
			put_one(map, i, i * step, i + step);
		}
		for(size_t i = 0; i < storage_size; i++) {
			find_one(map, i * step, i + step);
		}
		for(size_t i = 0; i < storage_size; i++) {
			remove_one(map, i * step, i + step);
			miss_one(map, i * step);
		}
		assert(map.size() == 0);
		test_sanity();
	}

	void test_put_remove_backward(Map_t& map, unsigned step) noexcept {
		printf("-> test_put_remove_backward()\n");
		assert(map.size() == 0);

		for(size_t i = 0; i < storage_size; i++) {
			size_t index = storage_size - i - 1;
			put_one(map, index, i * step, i + step);
		}
		for(size_t i = 0; i < storage_size; i++) {
			find_one(map, i * step, i + step);
		}
		for(size_t i = 0; i < storage_size; i++) {
			remove_one(map, i * step, i + step);
			miss_one(map, i * step);
		}
		assert(map.size() == 0);
		test_sanity();
	}

	void test_put_remove_odd_even(Map_t& map, unsigned step) noexcept {
		printf("-> test_put_remove_odd_even()\n");
		assert(map.size() == 0);

		for(size_t i = 0; i < storage_size; i++) {
			if(i % 2 != 0) {
				put_one(map, i, i * step, i + step);
			}
		}

		for(size_t i = 0; i < storage_size; i++) {
			if(i % 2 == 0) {
				put_one(map, i, i * step, i + step + 1);
			}
		}

		for(size_t i = 0; i < storage_size; i++) {
			if(i % 2 != 0) {
				find_one(map, i * step, i + step);
			} else {
				find_one(map, i * step, i + step + 1);
			}
		}

		for(size_t i = 0; i < storage_size; i++) {
			if(i % 2 != 0) {
				remove_one(map, i * step, i + step);
			} else {
				remove_one(map, i * step, i + step + 1);
			}
			miss_one(map, i * step);
		}
		assert(map.size() == 0);
		test_sanity();
	}

	void test_put_remove_same_key(Map_t& map, unsigned step) noexcept {
		printf("-> test_put_remove_same_key()\n");
		assert(map.size() == 0);

		for(size_t i = 0; i < storage_size; i++) {
			put_one(map, i, step, i + step);
		}

		for(size_t i = 0; i < storage_size; i++) {
			find_multi(map, step, i + step);
		}

		for(size_t i = 0; i < storage_size; i++) {
			remove_multi(map, step, i + step);
		}

		miss_one(map, step);

		assert(map.size() == 0);
		test_sanity();
	}

	void test_clear(Map_t& map, unsigned step) noexcept {
		printf("-> test_clear()\n");

		for(size_t i = 0; i < storage_size; i++) {
			put_one(map, i, i + step, i + step);
		}
		map.clear();
		assert(map.size() == 0);
		test_sanity();
	}

	void test_raii(Map_t& map) noexcept {
		printf("-> test_raii()\n");

		assert(map.size() == 0);
		for(size_t i = 0; i < storage_size; i++) {
			put_one(map, i, i, i);
		}
		Map_t tmp_map(std::move(map));
		assert(map.size() == 0);
		assert(tmp_map.size() == storage_size);
		map = std::move(map);
		map = std::move(tmp_map);
		std::swap(map, tmp_map);
		std::swap(map, tmp_map);
		for(size_t i = 0; i < storage_size; i++) {
			find_one(map, i, i);
			remove_one(map, i, i);
		}
		assert(map.size() == 0);
		test_sanity();
	}

	void dump(Map_t& map) noexcept {
		std::cout << "map has " << map.size() << " elements \n";
		for(size_t bucket = 0; bucket < map.buckets(); ++bucket) {
			std::cout << "B[" << bucket << "] ";
			for(auto it = map.cbegin(bucket); it != map.cend(); ++it) {
				std::cout << "[" << &(*it) << "] " << (*it).im_key << ":" << (*it).value.value << " -> ";
			}
			std::cout << "\n";
		}
	}

private:

	void test_sanity() {
		for(unsigned i = 0; i < storage_size; i++) {
			assert(not storage[i].im_linked);
		}
	}

	void put_one(Map_t& map, size_t node_index, const Key_t& key, const Value_t& value) noexcept {
		MapNode_t& node = storage[node_index];
		auto it = map.link(key, node);
		assert(it != map.end());
		bool recycled = (&node != &(*it));
		assert(not recycled);
		it->value = value;
		assert(*it == node);
		assert(it->im_key == key);
		assert(it->im_linked);
	}

	void find_one(Map_t& map, const Key_t& key, const Value_t& value) noexcept {
		auto it = map.find(key);
		assert(it != map.end());
		assert(it->value == value);
		assert(it->im_key == key);
		assert(it->im_linked);
	}

	void find_multi(Map_t& map, const Key_t& key, const Value_t& value) noexcept {
		auto it = map.find(key);
		assert(it != map.end());
		for(; it != map.end(); it.next(key)) {
			assert(it->im_key == key);
			assert(it->im_linked);
			if(it->value == value) {
				return;
			}
		}
		assert(false);
	}

	void miss_one(Map_t& map, const Key_t& key) noexcept {
		auto it = map.find(key);
		assert(it == map.end());
	}

	void remove_one(Map_t& map, const Key_t& key, const Value_t& value) noexcept {
		auto it = map.find(key);
		assert(it != map.end());
		assert(it->value == value);
		assert(it->im_key == key);
		assert(it->im_linked);
		map.remove(it);
		assert(not it->im_linked);
	}

	void remove_multi(Map_t& map, const Key_t& key, const Value_t& value) noexcept {
		auto it = map.find(key);
		assert(it != map.end());
		for(; it != map.end(); it.next(key)) {
			assert(it->im_key == key);
			assert(it->im_linked);
			if(it->value == value) {
				map.remove(it);
				assert(not it->im_linked);
				return;
			}
		}
		assert(false);
	}

};

}; // namespace intrusive

#endif /* INTRUSIVE_TESTS_TESTHASHMAP_H */

