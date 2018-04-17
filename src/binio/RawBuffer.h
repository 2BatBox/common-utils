#ifndef BINIO_RAW_BUFFER_H
#define BINIO_RAW_BUFFER_H

#include <cstdlib>
#include <cstring>

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

	Buffer& operator=(const Buffer&)noexcept = default;

public:

	inline decltype(size_bytes)offset() const noexcept {
		return size_bytes - left_bytes;
	}

	inline decltype(left_bytes)left() const noexcept {
		return left_bytes;
	}

	inline decltype(in_bounds)bounds() const noexcept {
		return in_bounds;
	}

	inline decltype(size_bytes)size() const noexcept {
		return size_bytes;
	}

	/**
	 * Set the assign point to the beginning of the buffer.
	 * @param bytes - bytes to move
	 * @return true - if the buffer is in bounds after moving
	 */
	bool rewind() noexcept {
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
	bool rewind(size_t bytes) noexcept {
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
	bool skip(size_t bytes) noexcept {
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
	 * Read one variable from the buffer and set it at the new position.
	 * An assignment operator of the type will be used to read a value.
	 * @param value - variable to read to
	 * @return true - if the buffer is in bounds after reading
	 */
	template <typename T>
	bool read(T& value) noexcept {
		if (in_bounds) {
			if (sizeof (T) > left_bytes) {
				in_bounds = false;
			} else {
				read_no_bounds(value);
			}
		}
		return in_bounds;
	}

	/**
	 * Read the variables from the buffer and set it at the new position.
	 * Assignment operators of the types will be used to read values.
	 * @param value - variable to read to
	 * @param args - variables to read to
	 * @return true - if the buffer is in bounds after reading
	 */
	template <typename T, typename... Args>
	bool read(T& value, Args&... args) noexcept {
		if (in_bounds) {
			if (args_size(value, args...) > left_bytes) {
				in_bounds = false;
			} else {
				read_no_bounds(value, args...);
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
	bool read_memory(T* array, const size_t array_len) noexcept {
		if (in_bounds) {
			size_t array_nb = array_len * sizeof (T);
			if (array_nb > left_bytes) {
				in_bounds = false;
			} else {
				memcpy(array, buffer_ptr, array_nb);
				buffer_ptr += array_nb;
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
	bool assign(T*& value, Args*&... args) noexcept {
		return assign(value) && assign(args...);
	}

	/**
	 * Assign one pointer to the buffer and set it at the new position.
	 * @param value - variable to assign to
	 * @param args - variables to assign to
	 * @return true - if the buffer is in bounds after assigning
	 */
	template <typename T>
	bool assign(T*& value) noexcept {
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

protected:

	static inline constexpr size_t args_size() noexcept {
		return 0;
	}

	template <typename T, typename... Args>
	static inline constexpr size_t args_size(T& value, Args&... args) noexcept {
		return sizeof (value) + args_size(args...);
	}

	template <typename T>
	inline void read_no_bounds(T& value) noexcept {
		value = *reinterpret_cast<const T*>(buffer_ptr);
		buffer_ptr += sizeof (T);
		left_bytes -= sizeof (T);
	}

	template <typename T, typename... Args>
	inline void read_no_bounds(T& value, Args&... args) noexcept {
		read_no_bounds(value);
		read_no_bounds(args...);
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

	inline RawBuffer region() noexcept {
		return RawBuffer(buffer_ptr, left_bytes);
	}

	/**
	 * Write the variables to the buffer and set it at the new position.
	 * Assignment operators of the types will be used to write values.
	 * @param value - variable to write from
	 * @return true - if the buffer is in bounds after writing
	 */
	template <typename T, typename... Args>
	bool write(const T& value, const Args&... args) noexcept {
		if (in_bounds) {
			if (args_size(value, args...) > left_bytes) {
				in_bounds = false;
			} else {
				write_no_bounds(value, args...);
			}
		}
		return in_bounds;
	}

	/**
	 * Write one variable to the buffer and set it at the new position.
	 * An assignment operator of the type will be used to write a value.
	 * @param value - variable to write from
	 * @param args - variables to write from
	 * @return true - if the buffer is in bounds after writing
	 */
	template <typename T>
	bool write(const T& value) noexcept {
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
	bool write_memory(const T* array, const size_t array_len) noexcept {
		if (in_bounds) {
			size_t array_nb = array_len * sizeof (T);
			if (array_nb > left_bytes) {
				in_bounds = false;
			} else {
				memcpy(buffer_ptr, array, array_nb);
				buffer_ptr += array_nb;
				left_bytes -= array_nb;
			}
		}
		return in_bounds;
	}

protected:

	template <typename T>
	inline void write_no_bounds(const T& value) noexcept {
		*reinterpret_cast<T*>(buffer_ptr) = value;
		buffer_ptr += sizeof (T);
		left_bytes -= sizeof (T);
	}

	template <typename T, typename... Args>
	inline void write_no_bounds(const T& value, const Args&... args) noexcept {
		write_no_bounds(value);
		write_no_bounds(args...);
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

	inline ConstRawBuffer region() noexcept {
		return ConstRawBuffer(buffer_ptr, left_bytes);
	}

};


}; // namespace binio

#endif /* BINIO_RAW_BUFFER_H */