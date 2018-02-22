//#ifndef LRU_CACHE_TEST_H
//#define LRU_CACHE_TEST_H
//
//#include <assert.h>
//#include <cstdio>
//
//#include "LruCache.h"
//
//class LruCacheTest {
//
//	typedef unsigned Key_t;
//	typedef unsigned Value_t;
//	
//	typedef Data<Key_t, Value_t> Data_t;
//	
//	typedef LruCache<Data_t> LruCache_t;
//	const size_t storage_size;
//	
//	LruCache_t cache;
//	
//public:
//	
//	LruCacheTest(unsigned storage_size, float load_factor)
//		: storage_size(storage_size),
//			cache(storage_size, load_factor)
//		{
//	}
//	
//	LruCacheTest(const LruCacheTest&) = delete;
//	LruCacheTest(LruCacheTest&&) = delete;
//	
//	LruCacheTest operator=(const LruCacheTest&) = delete;
//	LruCacheTest operator=(LruCacheTest&&) = delete;
//	
//	~LruCacheTest(){
//	}
//	
//	size_t mem_used(){
//		return cache.mem_used();
//	}
//	
//	void test(){
//		printf("<LruCacheTest>...\n");
//		printf("sizeof(Data_t)=%zu\n", sizeof(Data_t));
//		printf("sizeof(Bucket_t)=%zu\n", sizeof(LruCache_t::Bucket_t));
//		printf("storage_size=%zu\n", storage_size);
//		printf("bucket_list_size=%u\n", cache.bucket_list_size);
//		printf("memory used %zu Kb\n", cache.mem_used() / (1024));
//		test_put();
//		test_get();
//		test_get_refresh();
//		test_remove();
//		test_cycle();
//		test_update_value();
//	}
//	
//	void test_sanity(){
//		for (Key_t i = 0; i < storage_size; i++) {
//			assert(not cache.storage[i].im_linked);
//			assert(cache.storage[i].il_linked);
//		}
//	}
//
//	void test_put() { 
//		assert(cache.size() == 0);
//		
//		for (Key_t i = 0; i < storage_size; i++) {
//			cache.put(i, i);
//		}
//		assert(cache.size() == storage_size);
//		
//		Value_t value;
//		for (Key_t i = 0; i < storage_size; i++) {
//			assert(cache.get(i, value));
//			assert(value == i);
//		}
//		
//		cache.reset();
//		assert(cache.size() == 0);
//		test_sanity();
//	}
//	
//	void test_get() { 
//		assert(cache.size() == 0);
//		
//		Key_t half = storage_size / 2;
//		for (Key_t i = 0; i < half; i++) {
//			cache.put(i, i);
//		}
//		assert(cache.size() == half);
//		
//		Value_t value;
//		for (Key_t i = 0; i < storage_size; i++) {
//			if(i < half){
//				assert(cache.get(i, value));
//				assert(value == i);
//			} else {
//				assert(not cache.get(i, value));
//			}
//		}
//		
//		cache.reset();
//		assert(cache.size() == 0);
//		test_sanity();
//	}
//	
//	void test_get_refresh() { 
//		assert(cache.size() == 0);
//		
//		Value_t value;
//		cache.put(0xabcd, 0xbcde);
//		
//		for (Key_t i = 1; i < storage_size * 2; i++) {
//			cache.put(i, i);
//			assert(cache.get_refresh(0xabcd, value));
//			assert(value = 0xbcde);
//		}
//		
//		assert(cache.size() == storage_size);
//		
//		cache.reset();
//		assert(cache.size() == 0);
//		test_sanity();
//	}
//	
//	void test_remove() { 
//		assert(cache.size() == 0);
//		
//		for (Key_t i = 0; i < storage_size; i++) {
//			cache.put(i, i);
//		}
//		assert(cache.size() == storage_size);
//		
//		Key_t half = storage_size / 2;
//		for (Key_t i = half; i < storage_size; i++) {
//			assert(cache.remove(i));
//			assert(not cache.remove(i));
//		}
//		
//		for (Key_t i = 0; i < half; i++) {
//			assert(cache.remove(i));
//			assert(not cache.remove(i));
//		}
//		
//		assert(cache.size() == 0);
//		test_sanity();
//	}
//	
//	void test_cycle() { 
//		assert(cache.size() == 0);
//		
//		for (Key_t i = 0; i < storage_size * 2; i++) {
//			cache.put(i, i);
//		}
//		assert(cache.size() == storage_size);
//		
//		Value_t value;
//		for (Key_t i = 0; i < storage_size * 2; i++) {
//			if(i < storage_size){
//				assert(not cache.get(i, value));
//			} else {
//				assert(cache.get(i, value));
//				assert(value == i);
//			}
//		}
//		
//		cache.reset();
//		assert(cache.size() == 0);
//		test_sanity();
//	}
//	
//	void test_update_value() { 
//		assert(cache.size() == 0);
//		
//		for (Key_t i = 0; i < storage_size; i++) {
//			cache.put(i, i);
//			cache.put(i, i * 2);
//		}
//		assert(cache.size() == storage_size);
//		
//		Value_t value;
//		for (Key_t i = 0; i < storage_size; i++) {
//			assert(cache.get(i, value));
//			assert(value == i * 2);
//		}
//		
//		cache.reset();
//		assert(cache.size() == 0);
//		test_sanity();
//	}
//	
//	void dump()
//	{
//		std::cout << "map has " << cache.map.size() << " elements \n";
//		for (size_t bucket = 0; bucket < cache.map.bucket_list_size; ++bucket) {
//			std::cout << "B[" << bucket << "] ";
//			for (auto it = cache.map.cbegin(bucket); it != cache.map.cend(); ++it) {
//				std::cout << (*it).value << " (" << (*it).im_key << "), ";
//			}
//			std::cout << "\n";
//		}
//		std::cout << "cached list has " << cache.list_cached.size() << " elements \n";
//		for (auto it = cache.list_cached.cbegin(); it != cache.list_cached.cend(); ++it) {
//			std::cout << (*it).value << ", ";
//		}
//		std::cout << "\n";
//		std::cout << "freed list has " << cache.list_freed.size() << " elements \n";
//		for (auto it = cache.list_freed.cbegin(); it != cache.list_freed.cend(); ++it) {
//			std::cout << (*it).value << ", ";
//		}
//		std::cout << "\n";
//	}
//	
//};
//
//#endif /* LRU_CACHE_TEST_H */
//
