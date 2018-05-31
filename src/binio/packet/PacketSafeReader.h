#ifndef BINIO_PACKET_SAFE_READER_H
#define BINIO_PACKET_SAFE_READER_H

#include <cstdlib>
#include <cstring>

#include "BasicPacket.h"

namespace binio {

/**
 * The PacketSafeReader / PacketSafeWriter design.
 * 
 * see BasicPacket.h for more details.
 * 
 * The head can be moved forward with following methods:
 * read(), read_memory(), write(), write_memory(), assign() and head_move().
 * 
 * The head also can be moved backward with head_move_back().
 * The tail can be moved with tail_move() and tail_move_back().
 * 
 * A PacketSafeReader/Writer object might be in 'In bounds' or 'Out of bounds' state.
 * 'In bounds' says that there weren't any tries to leave bounds of the object.
 * Any operation which tries to leave the bounds of the PacketSafeReader/Writer object
 * sets the object to 'Out of bounds' state.
 * For instance reading, writing, assigning more bytes than the available subarea has
 * or moving the head of before 'begin' point sets the object to 'Out of bounds' state.
 * It's an one-way operation, that means there is no way to set the object
 * from 'Out of bounds' back to 'In bounds' state.
 * All the non-const methods return a state of the object they are called with.
 * All the read, write or assign operations work with the 'available' subarea of the Area object only.
 * 
 **/

template <typename RawPtr, typename SizeType>
class BasicPacketSafeReader : public BasicPacket<RawPtr, SizeType> {
	using Base = BasicPacket<RawPtr, SizeType>;
protected:
	bool in_bounds;

	BasicPacketSafeReader(RawPtr* buf, SizeType len) noexcept :
	Base(buf, len),
	in_bounds(buf != nullptr) { }

public:

	/**
	 * @return true - if the packet is its bounds.
	 */
	inline bool bounds() const noexcept {
		return in_bounds;
	}

	/**
	 * Reset the state of the packet.
	 * @return true - if the packet is in its bounds.
	 */
	bool reset() noexcept {
		if (in_bounds) {
			SizeType off = Base::offset();
			Base::ptr_head -= off;
			Base::bytes_available += off + Base::bytes_padding;
			Base::bytes_padding = 0;
		}
		return in_bounds;
	}

	/**
	 * Move the head @bytes forward.
	 * @param bytes - bytes to move.
	 * @return true - if the packet is in its bounds after moving.
	 */
	bool head_move(SizeType bytes) noexcept {
		if (in_bounds) {
			if (bytes > Base::bytes_available) {
				in_bounds = false;
			} else {
				Base::ptr_head += bytes;
				Base::bytes_available -= bytes;
			}
		}
		return in_bounds;
	}

	/**
	 * Move the head @bytes backward.
	 * @param bytes - bytes to move.
	 * @return true - if the packet is in its bounds after moving.
	 */
	bool head_move_back(SizeType bytes) noexcept {
		if (in_bounds) {
			if (bytes > Base::offset()) {
				in_bounds = false;
			} else {
				Base::ptr_head -= bytes;
				Base::bytes_available += bytes;
			}
		}
		return in_bounds;
	}

	/**
	 * Move the tail @bytes forward.
	 * @param bytes - bytes to move.
	 * @return true - if the packet is in its bounds after moving.
	 */
	bool tail_move(SizeType bytes) noexcept {
		if (in_bounds) {
			if (bytes > Base::bytes_padding) {
				in_bounds = false;
			} else {
				Base::bytes_available += bytes;
				Base::bytes_padding -= bytes;
			}
		}
		return in_bounds;
	}

	/**
	 * Move the tail @bytes backward.
	 * @param bytes - bytes to move.
	 * @return true - if the packet is in its bounds after moving.
	 */
	bool tail_move_back(SizeType bytes) noexcept {
		if (in_bounds) {
			if (bytes > Base::bytes_available) {
				in_bounds = false;
			} else {
				Base::bytes_available -= bytes;
				Base::bytes_padding += bytes;
			}
		}
		return in_bounds;
	}

	/**
	 * Read @value from the packet and set the head to a new position.
	 * @param value - variable to read to.
	 * @return true - if the packet is in its bounds after reading.
	 */
	template <typename T>
	bool read(T& value) noexcept {
		if (in_bounds) {
			if (sizeof (T) > Base::bytes_available) {
				in_bounds = false;
			} else {
				read_unsafe(value);
			}
		}
		return in_bounds;
	}

	/**
	 * Read @value and @args from the packet and set the head to a new position.
	 * @param value - a variable to read to.
	 * @param args - variables to read to.
	 * @return true - if the packet is in its bounds after reading.
	 */
	template <typename T, typename... Args>
	bool read(T& value, Args&... args) noexcept {
		if (in_bounds) {
			if (sizeof_args(value, args...) > Base::bytes_available) {
				in_bounds = false;
			} else {
				read_unsafe(value, args...);
			}
		}
		return in_bounds;
	}

	/**
	 * Read an array from the packet and set the head to a new position.
	 * @param array - an array to read to.
	 * @param array_len - amount of @array elements.
	 * @return true - if the packet is in its bounds after reading.
	 */
	template <typename T>
	bool read_memory(T* array, SizeType array_len) noexcept {
		if (in_bounds) {
			array_len *= sizeof (T);
			if (array_len > Base::bytes_available) {
				in_bounds = false;
			} else {
				memcpy(array, Base::ptr_head, array_len);
				Base::ptr_head += array_len;
				Base::bytes_available -= array_len;
			}
		}
		return in_bounds;
	}

	/**
	 * Assign an array of pointers to the head and set the head to a new position.
	 * @param array - an array to assign.
	 * @param array_len - amount of @array elements.
	 * @return true - if the packet is in its bounds after assigning.
	 */
	template <typename T>
	bool assign(T*& array, SizeType array_len) noexcept {
		if (in_bounds) {
			array_len *= sizeof (T);
			if (array_len > Base::bytes_available) {
				in_bounds = false;
			} else {
				array = reinterpret_cast<T*>(Base::ptr_head);
				Base::ptr_head += array_len;
				Base::bytes_available -= array_len;
			}
		}
		return in_bounds;
	}

	/**
	 * Assign one pointer to the head and set the head to a new position.
	 * @param pointer - a pointer to assign.
	 * @return true - if the packet is in its bounds after assigning.
	 */
	template <typename T>
	bool assign(T*& pointer) noexcept {
		if (in_bounds) {
			if (sizeof (T) > Base::bytes_available) {
				in_bounds = false;
			} else {
				pointer = reinterpret_cast<T*>(Base::ptr_head);
				Base::ptr_head += sizeof (T);
				Base::bytes_available -= sizeof (T);
			}
		}
		return in_bounds;
	}

	/**
	 * Assign an array of pointers to the head.
	 * @param array - an array to assign.
	 * @param array_len - amount of @array elements.
	 * @return true - if the packet is in its bounds after assigning.
	 */
	template <typename T>
	bool assign_stay(T*& array, SizeType array_len) noexcept {
		if (in_bounds) {
			array_len *= sizeof (T);
			if (array_len > Base::bytes_available) {
				in_bounds = false;
			} else {
				array = reinterpret_cast<T*>(Base::ptr_head);
			}
		}
		return in_bounds;
	}

	/**
	 * Assign one pointer to the head.
	 * @param pointer - a pointer to assign.
	 * @return true - if the packet is in its bounds after assigning.
	 */
	template <typename T>
	bool assign_stay(T*& pointer) noexcept {
		if (in_bounds) {
			if (sizeof (T) > Base::bytes_available) {
				in_bounds = false;
			} else {
				pointer = reinterpret_cast<T*>(Base::ptr_head);
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
	inline void read_unsafe(T& value) noexcept {
		value = *reinterpret_cast<const T*>(Base::ptr_head);
		Base::ptr_head += sizeof (T);
		Base::bytes_available -= sizeof (T);
	}

	template <typename T, typename... Args>
	inline void read_unsafe(T& value, Args&... args) noexcept {
		read_unsafe(value);
		read_unsafe(args...);
	}

};

template <typename SizeType>
class PacketSafeReader : public BasicPacketSafeReader<const uint8_t, SizeType> {
	using Base = BasicPacketSafeReader<const uint8_t, SizeType>;

public:

	PacketSafeReader(ByteConstBuffer range) noexcept :
	Base(range.data(), range.length()) { }

	PacketSafeReader(ByteBuffer range) noexcept :
	Base(range.data(), range.length()) { }

	PacketSafeReader(const uint8_t* data, SizeType bytes) noexcept :
	Base(data, bytes) { }
};

}; // namespace binio

#endif /* BINIO_PACKET_SAFE_READER_H */