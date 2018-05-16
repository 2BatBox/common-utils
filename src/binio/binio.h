#ifndef BINIO_BINIO_H
#define BINIO_BINIO_H

namespace binio {

template <typename T>
class BasicByteRange {
	T* ptr_data;
	size_t bytes_length;

public:

	BasicByteRange() noexcept :
	ptr_data(nullptr),
	bytes_length(0) { }

	BasicByteRange(T* data, size_t bytes) noexcept :
	ptr_data(data),
	bytes_length(bytes) { }

	inline operator T*() const noexcept {
		return ptr_data;
	}

	T* data() const noexcept {
		return ptr_data;
	}

	inline size_t bytes() const noexcept {
		return bytes_length;
	}

	inline bool empty() const noexcept {
		return ptr_data == nullptr || bytes_length == 0;
	}

};

using ReadableByteRange = BasicByteRange<const uint8_t>;
using WritableByteRange = BasicByteRange<uint8_t>;

template <typename T>
inline ReadableByteRange make_readable_byte_range(T* ptr, size_t bytes) noexcept {
	return ReadableByteRange(reinterpret_cast<const uint8_t*>(ptr), bytes);
}

template <typename T>
inline WritableByteRange make_writable_byte_range(T* ptr, size_t bytes) noexcept {
	return WritableByteRange(reinterpret_cast<uint8_t*>(ptr), bytes);
}

}; // namespace binio

#endif /* BINIO_BINIO_H */

