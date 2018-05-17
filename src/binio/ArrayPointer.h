#ifndef BINIO_ARRAY_POINTER_H
#define BINIO_ARRAY_POINTER_H

namespace binio {

/**
 * Class binio::BasicByteArrayPtr represents a byte array pointer with length of the array.
 * The class also provides some utility functions like 'empty', 'assign' and etc.
 * No memory management is provided.
 */
template <typename T>
class BasicByteArrayPtr {
	T* pointer;
	size_t length_bytes;

public:

	BasicByteArrayPtr() noexcept :
	pointer(nullptr),
	length_bytes(0) { }

	BasicByteArrayPtr(T* data, size_t length) noexcept :
	pointer(data),
	length_bytes(length) { }

	template <typename P>
	BasicByteArrayPtr(const BasicByteArrayPtr<P>& array) noexcept :
	pointer(array.data()),
	length_bytes(array.length()) { }

	template <typename P>
	BasicByteArrayPtr& operator=(const BasicByteArrayPtr<P>& array) noexcept {
		if (this != reinterpret_cast<const BasicByteArrayPtr*>(&array)) {
			pointer = array.data();
			length_bytes = array.length();
		}
		return *this;
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
		return length_bytes;
	}

	inline bool empty() const noexcept {
		return pointer == nullptr || length_bytes == 0;
	}

	template <typename P>
	bool assign(const P*& pointer) const noexcept {
		if (sizeof (P) > length_bytes) {
			return false;
		} else {
			pointer = reinterpret_cast<const P*>(pointer);
			return true;
		}
	}

	template <typename P>
	bool assign(P*& pointer) noexcept {
		if (sizeof (P) > length_bytes) {
			return false;
		} else {
			pointer = reinterpret_cast<P*>(pointer);
			return true;
		}
	}

	template <typename P>
	bool assign(const P*& pointer, unsigned index) noexcept {
		if (sizeof (P) + index > length_bytes) {
			return false;
		} else {
			pointer = reinterpret_cast<const P*>(pointer + index);
			return true;
		}
	}

	template <typename P>
	bool assign(P*& pointer, unsigned index) noexcept {
		if (sizeof (P) + index > length_bytes) {
			return false;
		} else {
			pointer = reinterpret_cast<P*>(pointer + index);
			return true;
		}
	}

};

using ByteArrayPtr = BasicByteArrayPtr<uint8_t>;
using ByteArrayConstPtr = BasicByteArrayPtr<const uint8_t>;

template <typename T>
inline ByteArrayPtr make_byte_array(T* ptr, size_t bytes) noexcept {
	return ByteArrayPtr(reinterpret_cast<uint8_t*>(ptr), bytes);
}

template <typename T>
inline ByteArrayConstPtr make_const_byte_array(T* ptr, size_t bytes) noexcept {
	return ByteArrayConstPtr(reinterpret_cast<const uint8_t*>(ptr), bytes);
}


}; // namespace binio

#endif /* BINIO_ARRAY_POINTER_H */

