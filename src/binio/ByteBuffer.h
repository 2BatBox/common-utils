#ifndef BINIO_BYTE_BUFFER_H
#define BINIO_BYTE_BUFFER_H

#include <cstring>

namespace binio {

/**
 * A BasicBuffer object represents an area of memory as a pointer with length of the area.
 * No memory management is provided.
 */
template <typename T>
class BasicBuffer {
	T* pointer;
	size_t elements; // The length of the buffer in T elements.

public:

	BasicBuffer() noexcept :
	pointer(nullptr),
	elements(0) { }

	BasicBuffer(T* data, size_t length) noexcept :
	pointer(data),
	elements(length) { }

	inline operator BasicBuffer<const T>() const noexcept {
		return BasicBuffer<const T>(pointer, elements);
	}

	inline operator const T*() const noexcept {
		return pointer;
	}

	inline operator T*() noexcept {
		return pointer;
	}

	inline const T& operator[](unsigned index)const noexcept {
		return pointer[index];
	}

	inline T& operator[](unsigned index)noexcept {
		return pointer[index];
	}

	const T* data() const noexcept {
		return pointer;
	}

	T* data() noexcept {
		return pointer;
	}

	inline size_t length() const noexcept {
		return elements;
	}

	inline operator bool() const noexcept {
		return pointer != nullptr;
	}

	template <typename P>
	inline bool operator==(const BasicBuffer<P>& buf) const noexcept {
		if (pointer && buf.data()) {
			// comparing as byte arrays
			size_t bytes = elements * sizeof (T);
			size_t buf_bytes = buf.length() * sizeof (P);
			if (bytes == buf_bytes) {
				return (memcmp(pointer, buf.data(), bytes) == 0);
			}
		}
		return false;
	}

	template <typename P>
	inline bool operator!=(const BasicBuffer<P>& buf) const noexcept {
		return not operator==(buf);
	}

};

using ByteBuffer = BasicBuffer<uint8_t>;
using ByteConstBuffer = BasicBuffer<const uint8_t>;

// writable buffers

template <typename T>
inline ByteBuffer as_buffer(BasicBuffer<T> buf) noexcept {
	return ByteBuffer(reinterpret_cast<uint8_t*>(buf.data()), buf.length() * sizeof (T));
}

template <typename T>
inline ByteBuffer as_buffer(T* ptr, size_t elements) noexcept {
	return ByteBuffer(reinterpret_cast<uint8_t*>(ptr), elements * sizeof (T));
}

inline ByteBuffer as_buffer(void* ptr, size_t bytes) noexcept {
	return ByteBuffer(reinterpret_cast<uint8_t*>(ptr), bytes);
}


// readable buffers

template <typename T>
inline ByteConstBuffer as_const_buffer(const BasicBuffer<T>& buf) noexcept {
	return ByteConstBuffer(reinterpret_cast<const uint8_t*>(buf.data()), buf.length() * sizeof (T));
}

template <typename T>
inline ByteConstBuffer as_const_buffer(T* ptr, size_t elements) noexcept {
	return ByteConstBuffer(reinterpret_cast<const uint8_t*>(ptr), elements * sizeof (T));
}

inline ByteConstBuffer as_const_buffer(void* ptr, size_t bytes) noexcept {
	return ByteConstBuffer(reinterpret_cast<const uint8_t*>(ptr), bytes);
}

inline ByteConstBuffer as_const_buffer(const void* ptr, size_t bytes) noexcept {
	return ByteConstBuffer(reinterpret_cast<const uint8_t*>(ptr), bytes);
}

}; // namespace binio

#endif /* BINIO_BYTE_BUFFER_H */

