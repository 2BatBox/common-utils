#ifndef DPDK_ALLOCATOR_H
#define DPDK_ALLOCATOR_H

#include <rte_malloc.h>

namespace dpdk {

template<typename T>
class Allocator {
public:

	T* allocate(size_t size) noexcept {
		return reinterpret_cast<T*>(rte_malloc(nullptr, size * sizeof(T), 0));
	}

	void deallocate(T* ptr, size_t) noexcept {
		rte_free(ptr);
	}

	void construct(T* ptr) noexcept {
		new(ptr)T();
	}

	void construct(T* ptr, const T& t) noexcept {
		new(ptr)T(t);
	}

	void destroy(T* ptr) noexcept {
		ptr->~T();
	}

};

}; // namespace dpdk

#endif /* DPDK_ALLOCATOR_H */

