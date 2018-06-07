#ifndef CACHE_LRU_ASSEMBLER_H
#define CACHE_LRU_ASSEMBLER_H

#include "LruCache.h"
#include "../binio/StaticMemBuffer.h"

// gcc 4.8.2's -Wnon-virtual-dtor is broken and turned on by -Weffc++
#if __GNUC__ < 3 || (__GNUC__ == 4 && __GNUC_MINOR__ <= 8)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnon-virtual-dtor"
#pragma GCC diagnostic ignored "-Weffc++"
#define GCC_DIAG_POP_NEEDED
#endif

namespace cache {

template <typename K, size_t capacity>
struct Stream {
	K dst;
	binio::StaticMemBuffer<capacity> buffer;
};

template <typename K, size_t pkt_max_size, typename H = std::hash<K> >
class LruAssembler {
	// gb-probe
	//typedef cache::LruCache<Data_t, H, dpdk::Allocator<Data_t>, dpdk::Allocator<intrusive::MapBucket<Data_t> > > LruCache_t;

	using Key_t = K;
	using Value_t = Stream<K, pkt_max_size>;

	using Node = LruCacheNode<Key_t, Value_t>;
	using LruCache_t = LruCache<Node>;

	LruCache_t m_cache;

public:

	LruAssembler(unsigned capacity, float load_factor) noexcept : m_cache(capacity, load_factor) { }
	
	bool allocate() noexcept {
		return m_cache.allocate();
	}

	Value_t* connect(const Key_t& src, const Key_t& dst) noexcept {
		auto it = m_cache.put(src);
		it->value.dst = dst;
		it->value.buffer.clear();
		return &(*it);
	}

	Value_t* disconnect(const Key_t& key) noexcept {
		auto it = m_cache.find();
		if (it != m_cache.end()) {
			m_cache.remove(it);
			return &(*it);
		}
		return nullptr;
	}

	Value_t* find(const Key_t& key) noexcept {
		auto it = m_cache.find(key);
		if (it != m_cache.end()) {
			return &(*it);
		}
		return nullptr;
	};

};

}; // namespace cache

#if defined(GCC_DIAG_POP_NEEDED)
#pragma GCC diagnostic pop
#undef GCC_DIAG_POP_NEEDED
#endif


#endif /* CACHE_LRU_ASSEMBLER_H */

