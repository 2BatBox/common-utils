#pragma once

#include <cstdlib>
#include <cstring>
#include <cstdint>

#include "BasicMFrame.h"

namespace proto {

/**
 * The Safe Memory Frame design.
 * The class provides a bounds checking solution for safe reading,writing and heads moving operations.
 * 
 * see BasicMFrame.h for more details.
 * 
 * The head can be moved forward with following methods:
 * read(), read_memory(), write(), write_memory(), assign() and head_move().
 * 
 * The head also can be moved backward with head_move_back().
 * The tail can be moved with tail_move() and tail_move_back().
 * 
 * A SafeMFrame object might be in 'In bounds' or 'Out of bounds' state.
 * 'In bounds' says that there weren't any tries to leave bounds of the object.
 * Any operation which tries to leave the bounds of the SafeMFrame object
 * sets the object to 'Out of bounds' state.
 * For instance reading, writing, assigning more bytes than the available subarea has
 * or moving the head of before 'begin' point sets the object to 'Out of bounds' state.
 * It's an one-way operation, that means there is no way to set the object
 * from 'Out of bounds' back to 'In bounds' state.
 * All the non-const methods return a state of the object they are called with.
 * All the read, write or assign operations work with the 'available' subarea of the Area object only.
 * 
 **/

template<typename T>
class SafeMFrame : public BasicMFrame<T> {
	using Base = BasicMFrame<T>;

protected:
	bool m_in_bounds;

public:

	template <typename Ptr>
	SafeMFrame(Ptr* buffer, size_t buffer_bytes) noexcept :
		Base(buffer, buffer_bytes), m_in_bounds(buffer != nullptr && buffer_bytes) {}

	/**
	 * @return true - if the packet is its bounds.
	 */
	inline bool bounds() const noexcept {
		return m_in_bounds;
	}

	/**
	 * Put the frame in invalid state regardless its current state.
	 */
	inline void invalidate() noexcept {
		m_in_bounds = false;
	}

	/**
	 * Reset the state of the packet.
	 * @return true - if the packet is in its bounds.
	 */
	inline void reset() noexcept {
		size_t off = Base::offset();
		Base::m_head -= off;
		Base::m_available += off + Base::m_padding;
		Base::m_padding = 0;
	}

	/**
	 * Move the head @bytes forward.
	 * @param bytes - bytes to move.
	 * @return true - if the packet is in its bounds after moving.
	 */
	inline bool head_move(size_t bytes) noexcept {
		if(m_in_bounds) {
			if(bytes > Base::m_available) {
				m_in_bounds = false;
			} else {
				Base::m_head += bytes;
				Base::m_available -= bytes;
			}
		}
		return m_in_bounds;
	}

	/**
	 * Move the head @bytes backward.
	 * @param bytes - bytes to move.
	 * @return true - if the packet is in its bounds after moving.
	 */
	inline bool head_move_back(size_t bytes) noexcept {
		if(m_in_bounds) {
			if(bytes > Base::offset()) {
				m_in_bounds = false;
			} else {
				Base::m_head -= bytes;
				Base::m_available += bytes;
			}
		}
		return m_in_bounds;
	}

	/**
	 * Move the tail @bytes forward.
	 * @param bytes - bytes to move.
	 * @return true - if the packet is in its bounds after moving.
	 */
	inline bool tail_move(size_t bytes) noexcept {
		if(m_in_bounds) {
			if(bytes > Base::m_padding) {
				m_in_bounds = false;
			} else {
				Base::m_available += bytes;
				Base::m_padding -= bytes;
			}
		}
		return m_in_bounds;
	}

	/**
	 * Move the tail @bytes backward.
	 * @param bytes - bytes to move.
	 * @return true - if the packet is in its bounds after moving.
	 */
	inline bool tail_move_back(size_t bytes) noexcept {
		if(m_in_bounds) {
			if(bytes > Base::m_available) {
				m_in_bounds = false;
			} else {
				Base::m_available -= bytes;
				Base::m_padding += bytes;
			}
		}
		return m_in_bounds;
	}

	/**
	 * Read @value from the packet.
	 * The head moves to the new position.
	 * @param value - variable to read to.
	 * @return true - if the packet is in its bounds after reading.
	 */
	template<typename V>
	inline bool read(V& value) noexcept {
		if(m_in_bounds) {
			if(sizeof(V) > Base::m_available) {
				m_in_bounds = false;
			} else {
				read_impl(value);
			}
		}
		return m_in_bounds;
	}

	/**
	 * Read @value and @args from the packet.
	 * The head moves to the new position.
	 * @param value - a variable to read to.
	 * @param args - variables to read to.
	 * @return true - if the packet is in its bounds after reading.
	 */
	template<typename V, typename... Args>
	inline bool read(V& value, Args& ... args) noexcept {
		if(m_in_bounds) {
			if(sizeof_args(value, args...) > Base::m_available) {
				m_in_bounds = false;
			} else {
				read_impl(value, args...);
			}
		}
		return m_in_bounds;
	}

	/**
	 * Assign a pointer to the head.
	 * The head moves to the new position.
	 * @param pointer - a pointer to assign.
	 * @return true - if the packet is in its bounds after assigning.
	 */
	template<typename V>
	inline bool assign(V*& pointer) noexcept {
		if(m_in_bounds) {
			if(sizeof(V) > Base::m_available) {
				m_in_bounds = false;
			} else {
				pointer = reinterpret_cast<V*>(Base::m_head);
				Base::m_head += sizeof(V);
				Base::m_available -= sizeof(V);
			}
		}
		return m_in_bounds;
	}

	/**
	 * Assign a pointer to the head.
	 * The head doesn't move.
	 * @param pointer - a pointer to assign.
	 * @return true - if the packet is in its bounds after assigning.
	 */
	template<typename V>
	inline bool assign_stay(V*& pointer) noexcept {
		if(m_in_bounds) {
			if(sizeof(V) > Base::m_available) {
				m_in_bounds = false;
			} else {
				pointer = reinterpret_cast<V*>(Base::m_head);
			}
		}
		return m_in_bounds;
	}

	/**
	 * Write @value to the packet.
	 * The head moves to the new position.
	 * @param value - variable to write from.
	 * @return true - if the buffer is in its bounds after writing.
	 */
	template<typename V>
	inline bool write(const V& value) noexcept {
		if(m_in_bounds) {
			if(sizeof(V) > Base::m_available) {
				m_in_bounds = false;
			} else {
				write_impl(value);
			}
		}
		return m_in_bounds;
	}

	/**
	 * Write @value and @args to the packet.
	 * The head moves to the new position.
	 * @param value - variable to write from.
	 * @param args - variables to write from.
	 * @return true - if the buffer is in its bounds after writing.
	 */
	template<typename V, typename... Args>
	inline bool write(const V& value, const Args& ... args) noexcept {
		if(m_in_bounds) {
			if(sizeof_args(value, args...) > Base::m_available) {
				m_in_bounds = false;
			} else {
				write_impl(value, args...);
			}
		}
		return m_in_bounds;
	}

protected:

	static inline constexpr size_t sizeof_args() noexcept {
		return 0;
	}

	template<typename V, typename... Args>
	static inline constexpr size_t sizeof_args(V& value, Args& ... args) noexcept {
		return sizeof(value) + sizeof_args(args...);
	}

	template<typename V>
	inline void read_impl(V& value) noexcept {
		value = *reinterpret_cast<const V*>(Base::m_head);
		Base::m_head += sizeof(V);
		Base::m_available -= sizeof(V);
	}

	template<typename V, typename... Args>
	inline void read_impl(V& value, Args& ... args) noexcept {
		read_impl(value);
		read_impl(args...);
	}

	template<typename V>
	inline void write_impl(const V& value) noexcept {
		*(reinterpret_cast<V*>(Base::m_head)) = value;
		Base::m_head += sizeof(V);
		Base::m_available -= sizeof(V);
	}

	template<typename V, typename... Args>
	inline void write_impl(const V& value, const Args& ... args) noexcept {
		write_impl(value);
		write_impl(args...);
	}

};

using RoSafeMFrame = SafeMFrame<const uint8_t>;
using RwSafeMFrame = SafeMFrame<uint8_t>;

}; // namespace proto
