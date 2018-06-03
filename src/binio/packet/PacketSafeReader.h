#ifndef BINIO_PACKET_SAFE_READER_H
#define BINIO_PACKET_SAFE_READER_H

#include <cstdlib>
#include <cstring>

#include "BasicPacket.h"

// gcc 4.8.2's -Wnon-virtual-dtor is broken and turned on by -Weffc++
#if __GNUC__ < 3 || (__GNUC__ == 4 && __GNUC_MINOR__ <= 8)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnon-virtual-dtor"
#pragma GCC diagnostic ignored "-Weffc++"
#define GCC_DIAG_POP_NEEDED
#endif

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

template <typename T, typename S>
class BasicPacketSafeReader : public BasicPacket<T, S> {
	using Base = BasicPacket<T, S>;
protected:
	bool in_bounds;

	BasicPacketSafeReader(T* buf, S len) noexcept :
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
			S off = Base::offset();
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
	bool head_move(S bytes) noexcept {
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
	bool head_move_back(S bytes) noexcept {
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
	bool tail_move(S bytes) noexcept {
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
	bool tail_move_back(S bytes) noexcept {
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
	template <typename V>
	bool read(V& value) noexcept {
		if (in_bounds) {
			if (sizeof (V) > Base::bytes_available) {
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
	template <typename V, typename... Args>
	bool read(V& value, Args&... args) noexcept {
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
	template <typename V>
	bool read_memory(V* array, S array_len) noexcept {
		if (in_bounds) {
			array_len *= sizeof (V);
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
	template <typename V>
	bool assign(V*& array, S array_len) noexcept {
		if (in_bounds) {
			array_len *= sizeof (V);
			if (array_len > Base::bytes_available) {
				in_bounds = false;
			} else {
				array = reinterpret_cast<V*>(Base::ptr_head);
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
	template <typename V>
	bool assign(V*& pointer) noexcept {
		if (in_bounds) {
			if (sizeof (V) > Base::bytes_available) {
				in_bounds = false;
			} else {
				pointer = reinterpret_cast<V*>(Base::ptr_head);
				Base::ptr_head += sizeof (V);
				Base::bytes_available -= sizeof (V);
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
	template <typename V>
	bool assign_stay(V*& array, S array_len) noexcept {
		if (in_bounds) {
			array_len *= sizeof (V);
			if (array_len > Base::bytes_available) {
				in_bounds = false;
			} else {
				array = reinterpret_cast<V*>(Base::ptr_head);
			}
		}
		return in_bounds;
	}

	/**
	 * Assign one pointer to the head.
	 * @param pointer - a pointer to assign.
	 * @return true - if the packet is in its bounds after assigning.
	 */
	template <typename V>
	bool assign_stay(V*& pointer) noexcept {
		if (in_bounds) {
			if (sizeof (V) > Base::bytes_available) {
				in_bounds = false;
			} else {
				pointer = reinterpret_cast<V*>(Base::ptr_head);
			}
		}
		return in_bounds;
	}

protected:

	static inline constexpr S sizeof_args() noexcept {
		return 0;
	}

	template <typename V, typename... Args>
	static inline constexpr S sizeof_args(V& value, Args&... args) noexcept {
		return sizeof (value) + sizeof_args(args...);
	}

	template <typename V>
	inline void read_unsafe(V& value) noexcept {
		value = *reinterpret_cast<const V*>(Base::ptr_head);
		Base::ptr_head += sizeof (V);
		Base::bytes_available -= sizeof (V);
	}

	template <typename V, typename... Args>
	inline void read_unsafe(V& value, Args&... args) noexcept {
		read_unsafe(value);
		read_unsafe(args...);
	}

};

template <typename S>
class PacketSafeReader : public BasicPacketSafeReader<const uint8_t, S> {
	using Base = BasicPacketSafeReader<const uint8_t, S>;

public:

	PacketSafeReader(MemConstArea range) noexcept :
	Base(range.pointer(), range.length()) { }

	PacketSafeReader(MemArea range) noexcept :
	Base(range.pointer(), range.length()) { }

	PacketSafeReader(const uint8_t* data, S bytes) noexcept :
	Base(data, bytes) { }
};

}; // namespace binio

#if defined(GCC_DIAG_POP_NEEDED)
#pragma GCC diagnostic pop
#undef GCC_DIAG_POP_NEEDED
#endif

#endif /* BINIO_PACKET_SAFE_READER_H */