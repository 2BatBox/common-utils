#ifndef BINIO_RAW_BUFFER_H
#define BINIO_RAW_BUFFER_H

#include <cstdlib>
#include <cstring>

namespace binio {

/**
 * The RawBuffer design.
 * 
 *                 head               tail
 *                   |                 |   
 *   | <- offset ->  | <- available -> | <- padding -> |
 *   |B|B|B|B|B|B|B|B|B|B|B|B|B|B|B|B|B|B|B|B|B|B|B|B|B|
 *   | <-------------------- size -------------------> |
 * begin                                              end
 * 
 * Points of 'begin', 'end' and 'size' are constant,
 * that means they cannot be changed with public methods.
 * 
 * Any operations with RawBuffer data can be done only in available area.
 * 
 **/


template <typename RawPtr, typename SizeType>
class Buffer {
	friend class RawBuffer;
	friend class ConstRawBuffer;

protected:
	RawPtr* buffer_ptr;
	SizeType bytes_available;
	SizeType bytes_padding;
	SizeType bytes_size;
	bool in_bounds;

	Buffer() noexcept :
	buffer_ptr(nullptr),
	bytes_available(0),
	bytes_padding(0),
	bytes_size(0),
	in_bounds(true) { }

	Buffer(RawPtr* buf, SizeType len) noexcept :
	buffer_ptr(buf),
	bytes_available(len),
	bytes_padding(0),
	bytes_size(len),
	in_bounds(true) { }

	Buffer& operator =(const Buffer&) noexcept = default;

public:

	/**
	 * @return The distance between 'begin' and 'end'
	 */
	inline SizeType size() const noexcept {
		return bytes_size;
	}

	/**
	 * @return The distance between 'begin' and 'head'
	 */
	inline SizeType offset() const noexcept {
		return bytes_size - bytes_available - bytes_padding;
	}

	/**
	 * @return The distance between 'head' and 'tail'
	 */
	inline SizeType available() const noexcept {
		return bytes_available;
	}

	/**
	 * @return The distance between 'tail' and 'end'
	 */
	inline SizeType padding() const noexcept {
		return bytes_padding;
	}

	/**
	 * @return true - if at least @bytes are available.
	 */
	inline bool available(SizeType bytes) const noexcept {
		return bytes <= bytes_available;
	}

	/**
	 * @return true - if the buffer is its bounds.
	 */
	inline bool bounds() const noexcept {
		return in_bounds;
	}

	/**
	 * Reset the state of the buffer.
	 * @return true - if the buffer is in its bounds.
	 */
	bool reset() noexcept {
		if (in_bounds) {
			SizeType off = offset();
			buffer_ptr -= off;
			bytes_available += off;
		}
		return in_bounds;
	}

	/**
	 * Move the head @bytes forward.
	 * @param bytes - bytes to move.
	 * @return true - if the buffer is in its bounds after moving.
	 */
	bool head_move(SizeType bytes) noexcept {
		if (in_bounds) {
			if (bytes > bytes_available) {
				in_bounds = false;
			} else {
				buffer_ptr += bytes;
				bytes_available -= bytes;
			}
		}
		return in_bounds;
	}

	/**
	 * Move the head @bytes backward.
	 * @param bytes - bytes to move.
	 * @return true - if the buffer is in its bounds after moving.
	 */
	bool head_move_back(SizeType bytes) noexcept {
		if (in_bounds) {
			if (bytes > offset()) {
				in_bounds = false;
			} else {
				buffer_ptr -= bytes;
				bytes_available += bytes;
			}
		}
		return in_bounds;
	}

	/**
	 * Move the tail @bytes forward.
	 * @param bytes - bytes to move.
	 * @return true - if the buffer is in its bounds after moving.
	 */
	bool tail_move(SizeType bytes) noexcept {
		if (in_bounds) {
			if (bytes > bytes_padding) {
				in_bounds = false;
			} else {
				bytes_available += bytes;
				bytes_padding -= bytes;
			}
		}
		return in_bounds;
	}

	/**
	 * Move the tail @bytes backward.
	 * @param bytes - bytes to move.
	 * @return true - if the buffer is in its bounds after moving.
	 */
	bool tail_move_back(SizeType bytes) noexcept {
		if (in_bounds) {
			if (bytes > bytes_available) {
				in_bounds = false;
			} else {
				bytes_available -= bytes;
				bytes_padding += bytes;
			}
		}
		return in_bounds;
	}

	/**
	 * Read @value from the buffer and set the head to a new position.
	 * @param value - variable to read to.
	 * @return true - if the buffer is in its bounds after reading.
	 */
	template <typename T>
	bool read(T& value) noexcept {
		if (in_bounds) {
			if (sizeof (T) > bytes_available) {
				in_bounds = false;
			} else {
				read_no_bounds(value);
			}
		}
		return in_bounds;
	}

	/**
	 * Read @value and @args from the buffer and set the head to a new position.
	 * @param value - a variable to read to.
	 * @param args - variables to read to.
	 * @return true - if the buffer is in its bounds after reading.
	 */
	template <typename T, typename... Args>
	bool read(T& value, Args&... args) noexcept {
		if (in_bounds) {
			if (sizeof_args(value, args...) > bytes_available) {
				in_bounds = false;
			} else {
				read_no_bounds(value, args...);
			}
		}
		return in_bounds;
	}

	/**
	 * Read an array from the buffer and set the head to a new position.
	 * @param array - an array to read to.
	 * @param array_len - amount of @array elements.
	 * @return true - if the buffer is in its bounds after reading.
	 */
	template <typename T>
	bool read_memory(T* array, const SizeType array_len) noexcept {
		if (in_bounds) {
			SizeType array_nb = array_len * sizeof (T);
			if (array_nb > bytes_available) {
				in_bounds = false;
			} else {
				memcpy(array, buffer_ptr, array_nb);
				buffer_ptr += array_nb;
				bytes_available -= array_nb;
			}
		}
		return in_bounds;
	}

	/**
	 * Assign an array of pointers to the buffer set the head to a new position.
	 * @param array - an array to assign.
	 * @param array_len - amount of @array elements.
	 * @return true - if the buffer is in its bounds after assigning.
	 */
	template <typename T>
	bool assign(T*& array, SizeType array_len) noexcept {
		array_len *= sizeof (T);
		if (in_bounds) {
			if (array_len > bytes_available) {
				in_bounds = false;
			} else {
				array = reinterpret_cast<T*> (buffer_ptr);
				buffer_ptr += array_len;
				bytes_available -= array_len;
			}
		}
		return in_bounds;
	}

	/**
	 * Assign one pointer to the buffer and set the head to a new position.
	 * @param pointer - a pointer to assign.
	 * @return true - if the buffer is in its bounds after assigning.
	 */
	template <typename T>
	bool assign(T*& pointer) noexcept {
		if (in_bounds) {
			if (sizeof (T) > bytes_available) {
				in_bounds = false;
			} else {
				pointer = reinterpret_cast<T*> (buffer_ptr);
				buffer_ptr += sizeof (T);
				bytes_available -= sizeof (T);
			}
		}
		return in_bounds;
	}

protected:

	static inline constexpr SizeType sizeof_args() noexcept {
		return 0;
	}

	template <typename T, typename... Args>
	static inline constexpr SizeType sizeof_args(T& value, Args&... args) noexcept {
		return sizeof (value) + sizeof_args(args...);
	}

	template <typename T>
	inline void read_no_bounds(T& value) noexcept {
		value = *reinterpret_cast<const T*> (buffer_ptr);
		buffer_ptr += sizeof (T);
		bytes_available -= sizeof (T);
	}

	template <typename T, typename... Args>
	inline void read_no_bounds(T& value, Args&... args) noexcept {
		read_no_bounds(value);
		read_no_bounds(args...);
	}

};

using RawType_t = uint8_t;
using ConstRawType_t = const uint8_t;
using SizeType_t = size_t;

class RawBuffer : public Buffer<RawType_t, SizeType_t> {
	using Base = Buffer<RawType_t, SizeType_t>;

public:

	RawBuffer() noexcept : Base() { }

	template <typename T>
	RawBuffer(T* buf, SizeType_t buf_len) noexcept :
	Base(reinterpret_cast<RawType_t*> (buf), buf_len * sizeof (T)) { }

	inline RawBuffer region() noexcept {
		return RawBuffer(buffer_ptr, bytes_available);
	}

	/**
	 * Write @value and @args to the buffer and set it at the new position.
	 * @param value - variable to write from.
	 * @param args - variables to write from.
	 * @return true - if the buffer is in its bounds after writing.
	 */
	template <typename T, typename... Args>
	bool write(const T& value, const Args&... args) noexcept {
		if (in_bounds) {
			if (sizeof_args(value, args...) > bytes_available) {
				in_bounds = false;
			} else {
				write_no_bounds(value, args...);
			}
		}
		return in_bounds;
	}

	/**
	 * Write one variable to the buffer and set the head to a new position.
	 * @param value - variable to write from.
	 * @return true - if the buffer is in its bounds after writing.
	 */
	template <typename T>
	bool write(const T& value) noexcept {
		if (in_bounds) {
			if (sizeof (T) > bytes_available) {
				in_bounds = false;
			} else {
				*reinterpret_cast<T*> (buffer_ptr) = value;
				buffer_ptr += sizeof (T);
				bytes_available -= sizeof (T);
			}
		}
		return in_bounds;
	}

	/**
	 * Write an array to the buffer and set the head to a new position.
	 * @param value - variable to write from
	 * @param array_len - amount of @array elements.
	 * @return true - if the buffer is in its bounds after writing
	 */
	template <typename T>
	bool write_memory(const T* array, const SizeType_t array_len) noexcept {
		if (in_bounds) {
			SizeType_t array_nb = array_len * sizeof (T);
			if (array_nb > bytes_available) {
				in_bounds = false;
			} else {
				memcpy(buffer_ptr, array, array_nb);
				buffer_ptr += array_nb;
				bytes_available -= array_nb;
			}
		}
		return in_bounds;
	}

protected:

	template <typename T>
	inline void write_no_bounds(const T& value) noexcept {
		*reinterpret_cast<T*> (buffer_ptr) = value;
		buffer_ptr += sizeof (T);
		bytes_available -= sizeof (T);
	}

	template <typename T, typename... Args>
	inline void write_no_bounds(const T& value, const Args&... args) noexcept {
		write_no_bounds(value);
		write_no_bounds(args...);
	}
};

class ConstRawBuffer : public Buffer<ConstRawType_t, SizeType_t> {
	using Base = Buffer<ConstRawType_t, SizeType_t>;

public:

	ConstRawBuffer() noexcept : Base() { }

	ConstRawBuffer(const RawBuffer& raw) noexcept : Base() {
		Base::buffer_ptr = raw.buffer_ptr;
		Base::bytes_available = raw.bytes_available;
		Base::bytes_padding = raw.bytes_padding;
		Base::bytes_size = raw.bytes_size;
		Base::in_bounds = raw.in_bounds;
	}

	template <typename T>
	ConstRawBuffer(T* buf, SizeType_t buf_len) noexcept :
	Base(reinterpret_cast<ConstRawType_t*> (buf), buf_len * sizeof (T)) { }

	inline ConstRawBuffer region() noexcept {
		return ConstRawBuffer(buffer_ptr, bytes_available);
	}

};


}; // namespace binio

#endif /* BINIO_RAW_BUFFER_H */