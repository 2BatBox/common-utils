#ifndef BINIO_STATIC_ARRAY_H
#define BINIO_STATIC_ARRAY_H

#include "MemArea.h"

namespace binio {

/**
 * A StaticArray object represents a static byte buffer.
 */

template <typename T, size_t capacity_bytes>
class StaticArray {
protected:
	size_t m_length;
	T m_array[capacity_bytes];

public:

	StaticArray() noexcept : m_length(0), m_array() { }

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

	inline void length(size_t length) noexcept {
		m_length = length;
	}

	inline size_t length() const noexcept {
		return m_length;
	}

	inline size_t capacity() const noexcept {
		return capacity_bytes;
	}

	template <typename Array, size_t S>
	inline bool operator==(const StaticArray<Array, S>& lv) const noexcept {
		MemConstArea self = *this;
		MemConstArea other = lv;
		return self == other;
	}

	template <typename Array, size_t S>
	inline bool operator!=(const StaticArray<Array, S>& lv) const noexcept {
		return not operator==(lv);
	}

	inline operator MemConstArea() const noexcept {
		return as_mem_const_area(m_array, m_length);
	}

	inline operator MemArea() noexcept {
		return as_mem_area(m_array, m_length);
	}

	//TODO: intersection checkers

};

}; // namespace binio

#endif /* BINIO_STATIC_ARRAY_H */

