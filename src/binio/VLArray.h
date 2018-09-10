#ifndef BINIO_STATIC_ARRAY_H
#define BINIO_STATIC_ARRAY_H

#include "MArea.h"

#include <stdexcept>
#include <cstdio> //TODO: debug

namespace binio {

/**
 * A VLArray object represents an array with variable length.
 */

template <typename T, size_t Capacity>
class VLArray {
protected:
	size_t m_length;
	T m_array[Capacity];

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

	inline constexpr size_t capacity() const noexcept {
		return Capacity;
	}

	inline MArea assign(size_t length) throw (std::out_of_range) {
		if (length > Capacity) {
			fprintf(stderr, "MArea assign(size_t length = %zu):"
				"m_array=%p, m_length=%zu, capacity_bytes=%zu\n", length, m_array, m_length, Capacity); //TODO: debug
			throw std::out_of_range("VLArray::assign(size_t)");
		}
		m_length = length;
		return binio::as_area(m_array, m_length);
	}

	template <typename Array, size_t S>
	inline bool operator==(const VLArray<Array, S>& lv) const noexcept {
		auto self = as_const_area();
		auto other = lv.as_const_area();
		return self == other;
	}

	template <typename Array, size_t S>
	inline bool operator!=(const VLArray<Array, S>& lv) const noexcept {
		return not operator==(lv);
	}

	inline MCArea as_const_area() const noexcept {
		return binio::as_const_area(m_array, m_length);
	}

	inline MArea as_area() noexcept {
		return binio::as_area(m_array, m_length);
	}

	//TODO: intersection checkers

};

}; // namespace binio

#endif /* BINIO_STATIC_ARRAY_H */

