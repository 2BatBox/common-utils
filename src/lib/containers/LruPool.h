#pragma once

#include <lib/intrusive/LinkedList.h>
#include <lib/intrusive/HashMap.h>

#include <memory>

class TestUtilsLruPool;

namespace utils {

template<
	typename K,
	typename V,
	typename H = std::hash<K>
>
class LruPool {

	friend class ::TestUtilsLruPool;

public:

	struct Item {
		intrusive::HashMapHook<K, Item> __ihm[1u];
		intrusive::LinkedListHook<Item> __ill[2u];
		V __value;
		bool __is_acquired;

		bool operator==(const Item& data) const {
			return __value == data.__value;
		}

		const K& key() const {
			return __ihm[0].key;
		}

		const V& value() const {
			return __value;
		}

		V& value() {
			return __value;
		}

		bool is_acquired() const {
			return __is_acquired;
		}
	};

private:

	using Allocator_t = std::allocator<Item>;

	using List_t = intrusive::LinkedList<Item, 1u>;
	using Map_t = intrusive::HashMap<K, Item, 0, 0, H>;

	size_t _capacity;
	Allocator_t _allocator;
	Item* _storage;
	Map_t _map;
	List_t _list_used;
	List_t _list_freed;

public:
	using Iterator_t = typename List_t::Iterator_t;
	using ConstIterator_t = typename List_t::ConstIterator_t;

	LruPool(size_t capacity, float load_factor) :
		_capacity(capacity),
		_allocator(),
		_storage(_allocator.allocate(_capacity)),
		_map(size_t(capacity / load_factor) + 1u),
		_list_used(),
		_list_freed()
	{
		for(size_t i = 0; i < _capacity; i++) {
			_allocator.construct(_storage + i);
		}
		reset();
	}

	LruPool(const LruPool&) = delete;
	LruPool& operator=(const LruPool&) = delete;

	LruPool(LruPool&& rv) = delete;
	LruPool& operator=(LruPool&&) = delete;

	virtual ~LruPool() {
		destroy();
	}

	Item* acquire(const K& key) {
		Item* result = nullptr;

		auto it = _map.find(key);
		if(it != _map.end()) {
			result = it.get();
			_list_used.remove(result);
		} else {
			if(_list_freed.size() > 0) {
				result = _list_freed.pop_front();
			} else {
				result = _list_used.pop_front();
				_map.remove(result);
			}
			_map.insert(key, result);
			result->__is_acquired = true;
		}
		_list_used.push_back(result);
		return result;
	}

	/**
	 * @param value - MUST NOT BE nullptr;
	*/
	void release(Item* item) {
		_map.remove(item);
		_list_used.remove(item);
		_list_freed.push_front(item);
		item->__is_acquired = false;
	}

	Iterator_t begin() {
		return _list_used.begin();
	}

	ConstIterator_t cbegin() const {
		return _list_used.cbegin();
	}

	Iterator_t end() {
		return _list_used.end();
	}

	ConstIterator_t cend() const {
		return _list_used.cend();
	}

	ConstIterator_t find(const K& key) const {
		return ConstIterator_t(_map.find(key).get());
	}

	Iterator_t find(const K& key) {
		return Iterator_t(_map.find(key).get());
	}

	void reset() {
		_map.clear();
		_list_used.clear();
		_list_freed.clear();
		for(size_t i = 0; i < _capacity; i++) {
			const auto item = _storage + i;
			_list_freed.push_back(item);
			item->__is_acquired = false;
		}
	}

	size_t available() const {
		return _list_freed.size();
	}

	size_t capacity() const {
		return _capacity;
	}

	size_t size() const {
		return _list_used.size();
	}

private:

	void destroy() {
		_list_freed.clear();
		_list_used.clear();
		_map.clear();
		for(size_t i = 0; i < _capacity; i++) {
			_allocator.destroy(_storage + i);
		}
		_allocator.deallocate(_storage, _capacity);
		_storage = nullptr;
	}

};

}; // namespace utils
