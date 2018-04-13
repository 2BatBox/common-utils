#ifndef BINIO_RAW_BUFFER_H
#define BINIO_RAW_BUFFER_H

#include <cstdlib>

namespace binio {

/**
 *
 *   | <- offset ->  | <- left ->|
 *   |B|B|B|B|B|B|B|B|B|B|B|B|B|B|
 *                 |
 *             assign point
 */

template <typename RawPtr>
class Buffer {
	friend class RawBuffer;
	friend class ConstRawBuffer;

protected:
	RawPtr* buffer_ptr;
	size_t left_bytes;
	size_t size_bytes;
	bool in_bounds;

	Buffer() noexcept:
	buffer_ptr(nullptr),
	left_bytes(0),
	size_bytes(0),
	in_bounds(true) { }

	Buffer(RawPtr* buf, size_t len) noexcept:
	buffer_ptr(buf),
	left_bytes(len),
	size_bytes(len),
	in_bounds(true) { }

public:

	inline size_t offset() const noexcept {
		return size_bytes - left_bytes;
	}

	inline size_t left() const noexcept {
		return left_bytes;
	}

	inline bool bounds() const noexcept {
		return in_bounds;
	}

	/**
	 * Set the assign point to the beginning of the buffer.
	 * @param bytes - bytes to move
	 * @return true - if the buffer is in bounds after moving
	 */
	inline bool rewind() noexcept {
		if (in_bounds) {
			size_t offset = size_bytes - left_bytes;
			buffer_ptr -= offset;
			left_bytes += offset;
		}
		return in_bounds;
	}

	/**
	 * Set the assign point to a new position.
	 * @param bytes - bytes to move
	 * @return true - if the buffer is in bounds after moving
	 */
	inline bool rewind(size_t bytes) noexcept {
		if (in_bounds) {
			if (bytes > offset()) {
				in_bounds = false;
			} else {
				buffer_ptr -= bytes;
				left_bytes += bytes;
			}
		}
		return in_bounds;
	}

	/**
	 * Set the assign point to a new position.
	 * @param bytes - bytes to move
	 * @return true - if the buffer is in bounds after moving
	 */
	inline bool skip(size_t bytes) noexcept {
		if (in_bounds) {
			if (bytes > left_bytes) {
				in_bounds = false;
			} else {
				buffer_ptr += bytes;
				left_bytes -= bytes;
			}
		}
		return in_bounds;
	}

	/**
	 * Read the variables from the buffer and set it at the new position.
	 * @param value - variable to read to
	 * @param args - variables to read to
	 * @return true - if the buffer is in bounds after reading
	 */
	template <typename T, typename... Args>
	inline bool read(T& value, Args&... args) noexcept {
		return read(value) && read(args...);
	}

	/**
	 * Read one variable from the buffer and set it at the new position.
	 * @param value - variable to read to
	 * @return true - if the buffer is in bounds after reading
	 */
	template <typename T>
	inline bool read(T& value) noexcept {
		if (in_bounds) {
			if (sizeof (T) > left_bytes) {
				in_bounds = false;
			} else {
				value = *reinterpret_cast<const T*>(buffer_ptr);
				buffer_ptr += sizeof (T);
				left_bytes -= sizeof (T);
			}
		}
		return in_bounds;
	}

	/**
	 * Read an array from the buffer and set it at the new position.
	 * @param value - variable to read to
	 * @return true - if the buffer is in bounds after reading
	 */
	template <typename T>
	inline bool read(T* array, const size_t array_len) noexcept {
		if (in_bounds) {
			size_t array_nb = array_len * sizeof (T);
			if (array_nb > left_bytes) {
				in_bounds = false;
			} else {
				for (size_t i = 0; i < array_len; i++) {
					array[i] = *reinterpret_cast<const T*>(buffer_ptr);
					buffer_ptr += sizeof (T);
				}
				left_bytes -= array_nb;
			}
		}
		return in_bounds;
	}

	/**
	 * Assign the pointers to the buffer and set it at the new position.
	 * @param value - variable to assign to
	 * @param args - variables to assign to
	 * @return true - if the buffer is in bounds after assigning
	 */
	template <typename T, typename... Args>
	inline bool assign(T*& value, Args*&... args) noexcept {
		return assign(value) && assign(args...);
	}

	/**
	 * Assign one pointer to the buffer and set it at the new position.
	 * @param value - variable to assign to
	 * @param args - variables to assign to
	 * @return true - if the buffer is in bounds after assigning
	 */
	template <typename T>
	inline bool assign(T*& value) noexcept {
		if (in_bounds) {
			if (sizeof (T) > left_bytes) {
				in_bounds = false;
			} else {
				value = reinterpret_cast<T*>(buffer_ptr);
				buffer_ptr += sizeof (T);
				left_bytes -= sizeof (T);
			}
		}
		return in_bounds;
	}
};

using RawType_t = uint8_t;
using ConstRawType_t = const uint8_t;

class RawBuffer: public Buffer<RawType_t> {
	using Base = Buffer<RawType_t>;

public:

	RawBuffer() noexcept: Base() { }

	template <typename T>
	RawBuffer(T* buf, size_t buf_len) noexcept:
	Base(reinterpret_cast<RawType_t*>(buf), buf_len * sizeof (T)) { }

	//	RawBuffer as_region() noexcept {
	//	}

	/**
	 * Write the variables to the buffer and set it at the new position.
	 * @param value - variable to write from
	 * @return true - if the buffer is in bounds after writing
	 */
	template <typename T, typename... Args>
	inline bool write(const T& value, const Args&... args) noexcept {
		return write(value) && write(args...);
	}

	/**
	 * Write one variable to the buffer and set it at the new position.
	 * @param value - variable to write from
	 * @param args - variables to write from
	 * @return true - if the buffer is in bounds after writing
	 */
	template <typename T>
	inline bool write(const T& value) noexcept {
		if (in_bounds) {
			if (sizeof (T) > left_bytes) {
				in_bounds = false;
			} else {
				*reinterpret_cast<T*>(buffer_ptr) = value;
				buffer_ptr += sizeof (T);
				left_bytes -= sizeof (T);
			}
		}
		return in_bounds;
	}

	/**
	 * Write an array to the buffer and set it at the new position.
	 * @param value - variable to write from
	 * @param args - variables to write from
	 * @return true - if the buffer is in bounds after writing
	 */
	template <typename T>
	inline bool write(const T* array, const size_t array_len) noexcept {
		if (in_bounds) {
			size_t array_nb = array_len * sizeof (T);
			if (array_nb > left_bytes) {
				in_bounds = false;
			} else {
				for (size_t i = 0; i < array_len; i++) {
					*reinterpret_cast<const T*>(buffer_ptr) = array[i];
					buffer_ptr += sizeof (T);
				}
				left_bytes -= array_nb;
			}
		}
		return in_bounds;
	}
};

class ConstRawBuffer: public Buffer<ConstRawType_t> {
	using Base = Buffer<ConstRawType_t>;

public:

	ConstRawBuffer() noexcept: Base() { }

	ConstRawBuffer(const RawBuffer& raw) noexcept: Base() {
		Base::buffer_ptr = raw.buffer_ptr;
		Base::left_bytes = raw.left_bytes;
		Base::size_bytes = raw.size_bytes;
		Base::in_bounds = raw.in_bounds;
	}

	template <typename T>
	ConstRawBuffer(T* buf, size_t buf_len) noexcept:
	Base(reinterpret_cast<ConstRawType_t*>(buf), buf_len * sizeof (T)) { }

};


}; // namespace binio

#endif /* BINIO_RAW_BUFFER_H */