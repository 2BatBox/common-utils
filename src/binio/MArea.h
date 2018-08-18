#ifndef BINIO_MEM_AREA_H
#define BINIO_MEM_AREA_H

#include <cstring>
#include <stdexcept>
#include <cstdio> //TODO: debug

namespace binio {

/**
 * A BasicMArea object represents an area of the memory as a pointer and length of the area.
 * No memory management is provided.
 * Bounds checking is provided for sensitive operations.
 */
template <typename T>
class BasicMArea {
protected:
	T* m_pointer;
	size_t m_length; // The length of the buffer in T elements.

public:

	BasicMArea() noexcept :
	m_pointer(nullptr),
	m_length(0) { }

	BasicMArea(T* data, size_t length) noexcept :
	m_pointer(data),
	m_length(length) { }

	inline operator BasicMArea<const T>() const noexcept {
		return BasicMArea<const T>(m_pointer, m_length);
	}

	inline const T* cbegin() const noexcept {
		return m_pointer;
	}

	inline T* begin() noexcept {
		return m_pointer;
	}

	inline const T* cend() const noexcept {
		return m_pointer + m_length;
	}

	inline T* end() noexcept {
		return m_pointer + m_length;
	}

	inline size_t length() const noexcept {
		return m_length;
	}

	inline operator bool() const noexcept {
		return m_pointer != nullptr;
	}

	BasicMArea subarea(size_t begin) const throw (std::out_of_range) {
		if (m_pointer == nullptr || begin > m_length) {
			fprintf(stderr, "BasicMArea subarea(size_t begin = %zu):"
				"m_pointer=%p, m_length=%zu\n", begin, m_pointer, m_length); //TODO: debug
			throw std::out_of_range("BasicMemArea::subarea(size_t)");
		}
		return BasicMArea(m_pointer + begin, m_length - begin);
	}

	BasicMArea subarea(size_t begin, size_t length) const throw (std::out_of_range) {
		if (m_pointer == nullptr || begin + length > m_length) {
			fprintf(stderr, "BasicMArea subarea(size_t begin = %zu, size_t length=%zu):"
				"m_pointer=%p, m_length=%zu\n", begin, length, m_pointer, m_length); //TODO: debug
			throw std::out_of_range("BasicMemArea::subarea(size_t, size_t)");
		}
		return BasicMArea(m_pointer + begin, length);
	}

	template <typename P>
	bool operator==(const BasicMArea<P>& buf) const noexcept {
		if (m_pointer && buf.cbegin()) {
			// comparing as byte arrays
			size_t bytes = m_length * sizeof (T);
			size_t buf_bytes = buf.length() * sizeof (P);
			if (bytes == buf_bytes) {
				return (memcmp(m_pointer, buf.cbegin(), bytes) == 0);
			}
		}
		return false;
	}

	template <typename P>
	inline bool operator!=(const BasicMArea<P>& buf) const noexcept {
		return not operator==(buf);
	}

	//TODO: intersection checkers

};

using MArea = BasicMArea<uint8_t>;
using MCArea = BasicMArea<const uint8_t>;

// writable areas

template <typename T>
inline MArea as_marea(T* ptr, size_t elements) noexcept {
	return MArea(reinterpret_cast<uint8_t*>(ptr), elements * sizeof (T));
}

inline MArea as_marea(void* ptr, size_t bytes) noexcept {
	return MArea(reinterpret_cast<uint8_t*>(ptr), bytes);
}


// readable areas

template <typename T>
inline MCArea as_mcarea(const MArea& marea) noexcept {
	return MCArea(marea.cbegin(), marea.length());
}

template <typename T>
inline MCArea as_mcarea(T* ptr, size_t elements) noexcept {
	return MCArea(reinterpret_cast<const uint8_t*>(ptr), elements * sizeof (T));
}

inline MCArea as_mcarea(void* ptr, size_t bytes) noexcept {
	return MCArea(reinterpret_cast<const uint8_t*>(ptr), bytes);
}

inline MCArea as_mcarea(const void* ptr, size_t bytes) noexcept {
	return MCArea(reinterpret_cast<const uint8_t*>(ptr), bytes);
}

}; // namespace binio

#endif /* BINIO_MEM_AREA_H */

