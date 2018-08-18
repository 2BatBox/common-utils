#ifndef BINIO_STATIC_ARRAY_H
#define BINIO_STATIC_ARRAY_H

#include "MArea.h"

#include <stdexcept>
#include <cstdio> //TODO: debug

namespace binio {

/**
 * A VLArray object represents a static array with variable length.
 */

template <typename T, size_t capacity_bytes>
class VLArray {
protected:
	size_t m_length;
	T m_array[capacity_bytes];

public:

	VLArray() noexcept : m_length(0), m_array() { }

	inline const T& operator[](unsigned index)const noexcept {
		return m_array[index];
	}

	inline T& operator[](unsigned index)noexcept {
		return m_array[index];
	}

	inline const T* operator+(unsigned index)const noexcept {
		return m_array + index;
	}

	inline T* operator+(unsigned index)noexcept {
		return m_array + index;
	}

	inline size_t length() const noexcept {
		return m_length;
	}

	inline size_t capacity() const noexcept {
		return capacity_bytes;
	}

	inline MArea assign(size_t length) throw (std::out_of_range) {
		if (length > capacity_bytes) {
			fprintf(stderr, "MArea assign(size_t length = %zu):"
				"m_array=%p, m_length=%zu, capacity_bytes=%zu\n", length, m_array, m_length, capacity_bytes); //TODO: debug
			throw std::out_of_range("VLArray::assign(size_t)");
		}
		m_length = length;
		return as_marea(m_array, m_length);
	}

	template <typename Array, size_t S>
	inline bool operator==(const VLArray<Array, S>& lv) const noexcept {
		auto self = mcarea();
		auto other = lv.mcarea();
		return self == other;
	}

	template <typename Array, size_t S>
	inline bool operator!=(const VLArray<Array, S>& lv) const noexcept {
		return not operator==(lv);
	}

	inline MCArea mcarea() const noexcept {
		return as_mcarea(m_array, m_length);
	}

	inline MArea marea() noexcept {
		return as_marea(m_array, m_length);
	}

	//TODO: intersection checkers

};

}; // namespace binio

#endif /* BINIO_STATIC_ARRAY_H */

