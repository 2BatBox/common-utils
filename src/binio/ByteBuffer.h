#ifndef BINIO_BYTE_BUFFER_H
#define BINIO_BYTE_BUFFER_H

#include <cstdlib>
#include <cstring>

#include "ArrayPointer.h"

namespace binio {

template <typename SizeType>
class BasicReadableBuffer;

template <typename SizeType>
class BasicWritableBuffer;

/**
 * The binio::BasicByteBuffer design.
 * BasicByteBuffer provides bounds checking.
 * No memory management is provided.
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
 * The main data area is divided into three subareas called 'offset', 'available' and 'padding'.
 * 
 * There is no way to move 'begin' and 'end' points but 'head' and 'tail' can be moved.
 * Moving 'head' and 'tail' points affect the subareas they start or end with.
 * 
 * The head moves forward with following methods:
 * read(), read_memory(), write(), write_memory(), assign() and head_move().
 * The head also can be moved backward with head_move_back().
 * The tail can be moved with tail_move() and tail_move_back().
 * 
 * An Area object might be in two states at the same time 'In bounds' and 'Out of bounds'.
 * Any operation which tries to leave the bounds of the Area object
 * sets the Area object to 'Out of bounds' state.
 * For instance reading, writing, assigning more bytes than the available subarea has
 * or moving the head of the area before 'begin' point sets the area to 'Out of bounds' state.
 * It's an one-way operation, that means there is no way to set the Area object back to 'In bounds' state.
 * All the non-const methods return state of the Area object they are called with.
 * All the read, write or assign operations work with the 'available' subarea of the Area object only.
 * 
 **/

template <typename RawPtr, typename SizeType>
class BasicByteBuffer {
protected:
	RawPtr* ptr_head;
	SizeType bytes_available;
	SizeType bytes_padding;
	SizeType bytes_size;
	bool in_bounds;

	BasicByteBuffer() noexcept :
	ptr_head(nullptr),
	bytes_available(0),
	bytes_padding(0),
	bytes_size(0),
	in_bounds(false) { }

	BasicByteBuffer(RawPtr* buf, SizeType len) noexcept :
	ptr_head(buf),
	bytes_available(len),
	bytes_padding(0),
	bytes_size(len),
	in_bounds(buf != nullptr) { }

	BasicByteBuffer(RawPtr* buf, SizeType ava, SizeType pad, SizeType size, bool bounds) noexcept :
	ptr_head(buf),
	bytes_available(ava),
	bytes_padding(pad),
	bytes_size(size),
	in_bounds(bounds && buf != nullptr) { }

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
	 * @return offset subarea as an ArrayPointer object.
	 */
	inline BasicByteArrayPtr<const RawPtr> offset_array() const noexcept {
		SizeType offset = offset();
		return BasicByteArrayPtr<RawPtr>(ptr_head - offset, offset);
	}

	/**
	 * @return offset subarea as an ArrayPointer object.
	 */
	inline BasicByteArrayPtr<RawPtr> offset_array() noexcept {
		SizeType offset = offset();
		return BasicByteArrayPtr<RawPtr>(ptr_head - offset, offset);
	}

	/**
	 * @return available subarea as an ArrayPointer object.
	 */
	inline BasicByteArrayPtr<const RawPtr> available_array() const noexcept {
		return BasicByteArrayPtr<RawPtr>(ptr_head, bytes_available);
	}

	/**
	 * @return available subarea as an ArrayPointer object.
	 */
	inline BasicByteArrayPtr<RawPtr> available_array() noexcept {
		return BasicByteArrayPtr<RawPtr>(ptr_head, bytes_available);
	}

	/**
	 * @return padding subarea as an ArrayPointer object.
	 */
	inline BasicByteArrayPtr<const RawPtr> padding_array() const noexcept {
		return BasicByteArrayPtr<RawPtr>(ptr_head + available(), bytes_padding);
	}

	/**
	 * @return padding subarea as an ArrayPointer object.
	 */
	inline BasicByteArrayPtr<RawPtr> padding_array() noexcept {
		return BasicByteArrayPtr<RawPtr>(ptr_head + available(), bytes_padding);
	}

	/**
	 * @return whole buffer area as an ArrayPointer object.
	 */
	inline BasicByteArrayPtr<const RawPtr> buffer_array() const noexcept {
		return BasicByteArrayPtr<RawPtr>(ptr_head - offset, bytes_size);
	}

	/**
	 * @return whole buffer area as an ArrayPointer object.
	 */
	inline BasicByteArrayPtr<RawPtr> buffer_array() noexcept {
		return BasicByteArrayPtr<RawPtr>(ptr_head - offset, bytes_size);
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
			ptr_head -= off;
			bytes_available += off + bytes_padding;
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
				ptr_head += bytes;
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
				ptr_head -= bytes;
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
				memcpy(array, ptr_head, array_len);
				ptr_head += array_len;
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
				array = reinterpret_cast<T*>(ptr_head);
				ptr_head += array_len;
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
				pointer = reinterpret_cast<T*>(ptr_head);
				ptr_head += sizeof (T);
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
		value = *reinterpret_cast<const T*>(ptr_head);
		ptr_head += sizeof (T);
		bytes_available -= sizeof (T);
	}

	template <typename T, typename... Args>
	inline void read_no_bounds(T& value, Args&... args) noexcept {
		read_no_bounds(value);
		read_no_bounds(args...);
	}

};

template <typename SizeType>
class BasicWritableBuffer : public BasicByteBuffer<uint8_t, SizeType> {
	using Base = BasicByteBuffer<uint8_t, SizeType>;
	friend class BasicReadableBuffer<SizeType>;

public:

	BasicWritableBuffer() noexcept : Base() { }

	BasicWritableBuffer(ByteArrayPtr range) noexcept :
	Base(range.data(), range.length()) { }

	template <typename T>
	BasicWritableBuffer(T* buf, SizeType bytes) noexcept :
	Base(reinterpret_cast<uint8_t*>(buf), bytes) { }

	BasicWritableBuffer(const BasicWritableBuffer<SizeType>& raw) noexcept :
	Base(raw.ptr_head, raw.available(), raw.padding(), raw.size(), raw.bounds()) { }

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
				*reinterpret_cast<T*>(Base::ptr_head) = value;
				Base::ptr_head += sizeof (T);
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
				memcpy(Base::ptr_head, array, array_len);
				Base::ptr_head += array_len;
				Base::bytes_available -= array_len;
			}
		}
		return Base::in_bounds;
	}

protected:

	template <typename T>
	inline void write_no_bounds(const T& value) noexcept {
		*reinterpret_cast<T*>(Base::ptr_head) = value;
		Base::ptr_head += sizeof (T);
		Base::bytes_available -= sizeof (T);
	}

	template <typename T, typename... Args>
	inline void write_no_bounds(const T& value, const Args&... args) noexcept {
		write_no_bounds(value);
		write_no_bounds(args...);
	}
};

template <typename SizeType>
class BasicReadableBuffer : public BasicByteBuffer<const uint8_t, SizeType> {
	using Base = BasicByteBuffer<const uint8_t, SizeType>;

public:

	BasicReadableBuffer() noexcept : Base() { }

	BasicReadableBuffer(ByteArrayPtr range) noexcept :
	Base(range.data(), range.length()) { }

	BasicReadableBuffer(ByteArrayConstPtr range) noexcept :
	Base(range.data(), range.length()) { }

	template <typename T>
	BasicReadableBuffer(T* buf, SizeType bytes) noexcept :
	Base(reinterpret_cast<const uint8_t*>(buf), bytes) { }

	BasicReadableBuffer(const BasicWritableBuffer<SizeType>& raw) noexcept :
	Base(raw.ptr_head, raw.available(), raw.padding(), raw.size(), raw.bounds()) { }

};

/**
 * Public aliases;
 */
using ReadableByteBuffer = BasicReadableBuffer<size_t>;
using WritableByteBuffer = BasicWritableBuffer<size_t>;

using ReadableByteBuffer8 = BasicReadableBuffer<uint8_t>;
using WritableByteBuffer8 = BasicWritableBuffer<uint8_t>;

using ReadableByteBuffer16 = BasicReadableBuffer<uint16_t>;
using WritableByteBuffer16 = BasicWritableBuffer<uint16_t>;

using ReadableByteBuffer32 = BasicReadableBuffer<uint32_t>;
using WritableByteBuffer32 = BasicWritableBuffer<uint32_t>;

using ReadableByteBuffer64 = BasicReadableBuffer<uint64_t>;
using WritableByteBuffer64 = BasicWritableBuffer<uint64_t>;

}; // namespace binio

#endif /* BINIO_BYTE_BUFFER_H */