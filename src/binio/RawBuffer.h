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
 *   |R|R|R|R|R|R|R|R|A|A|A|A|A|A|A|A|A|P|P|P|P|P|P|P|P|
 *   | <-------------------- size -------------------> |
 * begin                                              end
 * 
 * R - already read/written.
 * A - available to read/write.
 * P - padding bytes, they're not available to read/write.
 * 
 * The head moves forward with following methods:
 * read(), read_memory(), write(), write_memory(), assign() and head_move().
 * The head also can be moved backward with head_move_back().
 * The tail can be moved with tail_move() and tail_move_back().
 * 
 * RawBuffer has two state, 'In bounds' and 'Out of bounds'.
 * Any operation which tries to leave the bounds of the buffer
 * will set the buffer to 'Out of bounds' state.
 * For instance reading, writing, assigning more bytes than the available area has
 * or moving the head of a buffer before 'begin' point will set the buffer to 'Out of bounds' state.
 * It's an one-way operation, that means there is no way to set the buffer back to 'In bounds' state.
 * All the non-const methods return state of the buffer they are called with.
 * All the read, write or assign operations work with the available area of the buffer only.
 * 
 **/

template <typename SizeType>
class ROBuffer;

template <typename SizeType>
class RWBuffer;

template <typename RawPtr, typename SizeType>
class BaseBuffer {
protected:
	RawPtr* buffer_ptr;
	SizeType bytes_available;
	SizeType bytes_padding;
	SizeType bytes_size;
	bool in_bounds;

	BaseBuffer() noexcept :
	buffer_ptr(nullptr),
	bytes_available(0),
	bytes_padding(0),
	bytes_size(0),
	in_bounds(true) { }

	BaseBuffer(RawPtr* buf, SizeType len) noexcept :
	buffer_ptr(buf),
	bytes_available(len),
	bytes_padding(0),
	bytes_size(len),
	in_bounds(true) { }

	BaseBuffer(RawPtr* buf, SizeType ava, SizeType pad, SizeType size, bool bounds) noexcept :
	buffer_ptr(buf),
	bytes_available(ava),
	bytes_padding(pad),
	bytes_size(size),
	in_bounds(bounds) { }

	BaseBuffer(const BaseBuffer&) noexcept = default;

	BaseBuffer& operator =(const BaseBuffer&) noexcept = default;

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
	 * Shrink the buffer to the available area.
	 * @return true - if the buffer is in its bounds.
	 */
	bool trim() noexcept {
		if (in_bounds) {
			bytes_size = bytes_available;
			bytes_padding = 0;
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
	bool read_memory(T* array, SizeType array_len) noexcept {
		if (in_bounds) {
			array_len *= sizeof (T);
			if (array_len > bytes_available) {
				in_bounds = false;
			} else {
				memcpy(array, buffer_ptr, array_len);
				buffer_ptr += array_len;
				bytes_available -= array_len;
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
		if (in_bounds) {
			array_len *= sizeof (T);
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

template <typename SizeType>
class RWBuffer : public BaseBuffer<uint8_t, SizeType> {
	using Base = BaseBuffer<uint8_t, SizeType>;
	friend class ROBuffer<SizeType>;

public:

	RWBuffer() noexcept : Base() { }

	template <typename T>
	RWBuffer(T* buf, SizeType buf_len) noexcept :
	Base(reinterpret_cast<uint8_t*> (buf), buf_len * sizeof (T)) { }

	RWBuffer region() const noexcept {
		RWBuffer result(*this);
		result.trim();
		return result;
	}

	/**
	 * Write one variable to the buffer and set the head to a new position.
	 * @param value - variable to write from.
	 * @return true - if the buffer is in its bounds after writing.
	 */
	template <typename T>
	bool write(const T& value) noexcept {
		if (Base::in_bounds) {
			if (sizeof (T) > Base::bytes_available) {
				Base::in_bounds = false;
			} else {
				*reinterpret_cast<T*> (Base::buffer_ptr) = value;
				Base::buffer_ptr += sizeof (T);
				Base::bytes_available -= sizeof (T);
			}
		}
		return Base::in_bounds;
	}

	/**
	 * Write @value and @args to the buffer and set it at the new position.
	 * @param value - variable to write from.
	 * @param args - variables to write from.
	 * @return true - if the buffer is in its bounds after writing.
	 */
	template <typename T, typename... Args>
	bool write(const T& value, const Args&... args) noexcept {
		if (Base::in_bounds) {
			if (Base::sizeof_args(value, args...) > Base::bytes_available) {
				Base::in_bounds = false;
			} else {
				write_no_bounds(value, args...);
			}
		}
		return Base::in_bounds;
	}

	/**
	 * Write an array to the buffer and set the head to a new position.
	 * @param value - variable to write from
	 * @param array_len - amount of @array elements.
	 * @return true - if the buffer is in its bounds after writing
	 */
	template <typename T>
	bool write_memory(const T* array, SizeType array_len) noexcept {
		if (Base::in_bounds) {
			array_len *= sizeof (T);
			if (array_len > Base::bytes_available) {
				Base::in_bounds = false;
			} else {
				memcpy(Base::buffer_ptr, array, array_len);
				Base::buffer_ptr += array_len;
				Base::bytes_available -= array_len;
			}
		}
		return Base::in_bounds;
	}

protected:

	template <typename T>
	inline void write_no_bounds(const T& value) noexcept {
		*reinterpret_cast<T*> (Base::buffer_ptr) = value;
		Base::buffer_ptr += sizeof (T);
		Base::bytes_available -= sizeof (T);
	}

	template <typename T, typename... Args>
	inline void write_no_bounds(const T& value, const Args&... args) noexcept {
		write_no_bounds(value);
		write_no_bounds(args...);
	}
};

template <typename SizeType>
class ROBuffer : public BaseBuffer<const uint8_t, SizeType> {
	using Base = BaseBuffer<const uint8_t, SizeType>;

public:

	ROBuffer() noexcept : Base() { }

	ROBuffer(const RWBuffer<SizeType>& raw) noexcept :
	Base(raw.buffer_ptr, raw.available(), raw.padding(), raw.size(), raw.bounds()) { }

	template <typename T>
	ROBuffer(T* buf, SizeType buf_len) noexcept :
	Base(reinterpret_cast<const uint8_t*> (buf), buf_len * sizeof (T)) { }

	ROBuffer region() const noexcept {
		ROBuffer result(*this);
		result.trim();
		return result;
	}

};

/**
 * Public aliases;
 */
using RawBuffer = RWBuffer<size_t>;
using RawBufferConst = ROBuffer<size_t>;

using RawBuffer8 = RWBuffer<uint8_t>;
using RawBufferConst8 = ROBuffer<uint8_t>;

using RawBuffer16 = RWBuffer<uint16_t>;
using RawBufferConst16 = ROBuffer<uint16_t>;

using RawBuffer32 = RWBuffer<uint32_t>;
using RawBufferConst32 = ROBuffer<uint32_t>;

using RawBuffer64 = RWBuffer<uint64_t>;
using RawBufferConst64 = ROBuffer<uint64_t>;

}; // namespace binio

#endif /* BINIO_RAW_BUFFER_H */