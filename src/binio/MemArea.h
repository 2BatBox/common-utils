#ifndef BINIO_MEM_AREA_H
#define BINIO_MEM_AREA_H

#include <cstring>

namespace binio {

/**
 * A BasicMemArea object represents an area of the memory as a type specified array
 * and its length.
 */
template <typename T>
class BasicMemArea {
protected:
	T* m_pointer;
	size_t m_length; // The length of the buffer in T elements.

public:

	BasicMemArea() noexcept :
	m_pointer(nullptr),
	m_length(0) { }

	BasicMemArea(T* data, size_t length) noexcept :
	m_pointer(data),
	m_length(length) { }

	inline operator BasicMemArea<const T>() const noexcept {
		return BasicMemArea<const T>(m_pointer, m_length);
	}

	inline operator const T*() const noexcept {
		return m_pointer;
	}

	inline operator T*() noexcept {
		return m_pointer;
	}

	const T* pointer() const noexcept {
		return m_pointer;
	}

	T* pointer() noexcept {
		return m_pointer;
	}

	inline size_t length() const noexcept {
		return m_length;
	}

	inline operator bool() const noexcept {
		return m_pointer != nullptr;
	}

	template <typename P>
	inline bool operator==(const BasicMemArea<P>& buf) const noexcept {
		if (m_pointer && buf.pointer()) {
			// comparing as byte arrays
			size_t bytes = m_length * sizeof (T);
			size_t buf_bytes = buf.length() * sizeof (P);
			if (bytes == buf_bytes) {
				return (memcmp(m_pointer, buf.pointer(), bytes) == 0);
			}
		}
		return false;
	}

	template <typename P>
	inline bool operator!=(const BasicMemArea<P>& buf) const noexcept {
		return not operator==(buf);
	}

	//TODO: intersection checkers

};

using MemArea = BasicMemArea<uint8_t>;
using MemConstArea = BasicMemArea<const uint8_t>;

// writable areas

template <typename T>
inline MemArea as_mem_area(T* ptr, size_t elements) noexcept {
	return MemArea(reinterpret_cast<uint8_t*>(ptr), elements * sizeof (T));
}

inline MemArea as_mem_area(void* ptr, size_t bytes) noexcept {
	return MemArea(reinterpret_cast<uint8_t*>(ptr), bytes);
}


// readable areas

template <typename T>
inline MemConstArea as_mem_const_area(const MemArea& marea) noexcept {
	return MemConstArea(reinterpret_cast<const uint8_t*>(marea.pointer()), marea.length());
}

template <typename T>
inline MemConstArea as_mem_const_area(T* ptr, size_t elements) noexcept {
	return MemConstArea(reinterpret_cast<const uint8_t*>(ptr), elements * sizeof (T));
}

inline MemConstArea as_mem_const_area(void* ptr, size_t bytes) noexcept {
	return MemConstArea(reinterpret_cast<const uint8_t*>(ptr), bytes);
}

inline MemConstArea as_mem_const_area(const void* ptr, size_t bytes) noexcept {
	return MemConstArea(reinterpret_cast<const uint8_t*>(ptr), bytes);
}

}; // namespace binio

#endif /* BINIO_MEM_AREA_H */

