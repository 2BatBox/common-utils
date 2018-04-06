#ifndef BINIO_RAW_BUFFER_H
#define BINIO_RAW_BUFFER_H

#include <cstdlib>

namespace binio {

/**
 *   begin_ptr
 *       | <- offset ->  | <- left ->|
 *       |B|B|B|B|B|B|B|B|B|B|B|B|B|B|
 *                       |
 *                   buffer_ptr
 */

using RawBufferConstDataType_t = const uint8_t;
using RawBufferDataType_t = uint8_t;

template <typename Type>
class Buffer {
	template <typename T>
	friend Buffer<RawBufferDataType_t> make_buffer(T buf, size_t bytes);

	template <typename T>
	friend Buffer<RawBufferConstDataType_t> make_const_buffer(T buf, size_t bytes);

	Type* buffer_ptr;
	Type* begin_ptr;
	size_t left_bytes;
	bool in_bounds;

	Buffer(Type* buf, size_t len) noexcept :
	buffer_ptr(buf),
	begin_ptr(buf),
	left_bytes(len),
	in_bounds(true) { }

public:

	Buffer() noexcept :
	buffer_ptr(nullptr),
	begin_ptr(nullptr),
	left_bytes(0),
	in_bounds(false) { }

	template<typename T>
	Buffer(const Buffer<T>& buf) noexcept :
	buffer_ptr(buf.get()),
	begin_ptr(buf.begin()),
	left_bytes(buf.left()),
	in_bounds(buf.bounds()) { }

	template<typename T>
	Buffer& operator=(const Buffer<T>& buf) noexcept {
		buffer_ptr = buf.get();
		begin_ptr = buf.begin();
		left_bytes = buf.left();
		in_bounds = buf.bounds();
		return *this;
	}

	inline Type* get() const noexcept {
		return buffer_ptr;
	}

	inline const Type* begin() const noexcept {
		return begin_ptr;
	}

	inline size_t offset() const noexcept {
		return buffer_ptr - begin_ptr;
	}

	inline size_t left() const noexcept {
		return left_bytes;
	}

	/**
	 * Check if the buffer is in bounds.
	 * @return
	 */
	inline bool bounds() const noexcept {
		return in_bounds;
	}

	/**
	 * Return the buffer to the start state.
	 */
	inline void reset() noexcept {
		left_bytes += offset();
		buffer_ptr = begin_ptr;
		in_bounds = true;
	}

	/**
	 * Skip 'bytes' bytes and set 'buffer' at the new position.
	 * @param bytes - bytes to skip
	 * @return true - if the buffer is in bounds, false - if the buffer has been smashed during skipping
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
	 * @return true - if the buffer is in bounds, false - if the buffer has been smashed during reading
	 */
	template <typename T, typename... Args>
	inline bool read(T& value, Args&... args) noexcept {
		return read(value) && read(args...);
	}

	/**
	 * Read one variable from the buffer and set it at the new position.
	 * @param value - variable to read to
	 * @return true - if the buffer is in bounds, false - if the buffer has been smashed during reading
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
	 * Write the variables to the buffer and set it at the new position.
	 * @param value - variable to write from
	 * @return true - if the buffer is in bounds, false - if the buffer has been smashed during writing
	 */
	template <typename T, typename... Args>
	inline bool write(const T& value, const Args&... args) noexcept {
		return write(value) && write(args...);
	}

	/**
	 * Write one variable to the buffer and set it at the new position.
	 * @param value - variable to write from
	 * @param args - variables to write from
	 * @return true - if the buffer is in bounds, false - if the buffer has been smashed during writing
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
	 * Assign the pointers to the buffer and set it at the new position.
	 * @param value - variable to assign to
	 * @param args - variables to assign to
	 * @return true - if the buffer is in bounds, false - if the buffer has been smashed during writing
	 */
	template <typename T, typename... Args>
	inline bool assign(T*& value, Args*&... args) noexcept {
		return assign(value) && assign(args...);
	}

	/**
	 * Assign one pointer to the buffer and set it at the new position.
	 * @param value - variable to assign to
	 * @param args - variables to assign to
	 * @return true - if the buffer is in bounds, false - if the buffer has been smashed during writing
	 */
	template <typename T>
	inline bool assign(T*& value) {
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

using ConstRawBuffer = Buffer<RawBufferConstDataType_t>;
using RawBuffer = Buffer<RawBufferDataType_t>;

template <typename T>
inline ConstRawBuffer make_const_buffer(T buf, size_t bytes) {
	return ConstRawBuffer(reinterpret_cast<RawBufferConstDataType_t*>(buf), bytes);
}

template <typename T>
inline RawBuffer make_buffer(T buf, size_t bytes) {
	return RawBuffer(reinterpret_cast<RawBufferDataType_t*>(buf), bytes);
}


}; // namespace binio

#endif /* BINIO_RAW_BUFFER_H */