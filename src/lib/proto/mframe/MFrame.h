#pragma once

#include <cstdint>

#include "BasicMFrame.h"

namespace proto {

/**
 * The Memory Frame design.
 * see BasicMFrame.h for more details.
 * 
 * The head can be moved forward with following methods:
 * read*, write*, assign() and head_move() methods.
 * 
 * The head also can be moved backward with head_move_back().
 * The tail can be moved with tail_move() and tail_move_back().
 * 
 **/

template<typename T>
class MFrame : public BasicMFrame<T> {
	using Base = BasicMFrame<T>;

public:

	template <typename Ptr>
	MFrame(Ptr* buffer, size_t buffer_bytes) noexcept :
		Base(buffer, buffer_bytes) {}

	/**
	 * Reset the state of the packet.
	 */
	inline void reset() noexcept {
		const auto off = Base::offset();
		Base::m_head -= off;
		Base::m_available += off + Base::m_padding;
		Base::m_padding = 0;
	}

	/**
	 * Move the head @bytes forward.
	 * @param bytes - bytes to move.
	 */
	inline void head_move(size_t bytes) noexcept {
		Base::m_head += bytes;
		Base::m_available -= bytes;
	}

	/**
	 * Move the head @bytes backward.
	 * @param bytes - bytes to move.
	 */
	inline void head_move_back(size_t bytes) noexcept {
		Base::m_head -= bytes;
		Base::m_available += bytes;
	}

	/**
	 * Move the tail @bytes forward.
	 * @param bytes - bytes to move.
	 */
	inline void tail_move(size_t bytes) noexcept {
		Base::m_available += bytes;
		Base::m_padding -= bytes;
	}

	/**
	 * Move the tail @bytes backward.
	 * @param bytes - bytes to move.
	 */
	inline void tail_move_back(size_t bytes) noexcept {
		Base::m_available -= bytes;
		Base::m_padding += bytes;
	}

	/**
	 * Read @value from the packet.
	 * The head moves to the new position.
	 * @param value - variable to read to.
	 */
	template<typename V>
	inline void read(V& value) noexcept {
		read_impl(value);
	}

	/**
 	 * Read @value and @args from the packet.
 	 * The head moves to the new position.
 	 * @param value - a variable to read to.
 	 * @param args - variables to read to.
	 */
	template<typename V, typename... Args>
	inline void read(V& value, Args& ... args) noexcept {
		read_impl(value, args...);
	}

	/**
	 * Assign a pointer to the head.
	 * The head moves to the new position.
	 * @param pointer - a pointer to assign.
	 */
	template<typename V>
	inline void assign(V*& pointer) noexcept {
		pointer = reinterpret_cast<V*>(Base::m_head);
		Base::m_head += sizeof(V);
		Base::m_available -= sizeof(V);
	}

	/**
	 * Assign a pointer to the head.
	 * The head doesn't move.
	 * @param pointer - a pointer to assign.
	 */
	template<typename V>
	inline void assign_stay(V*& pointer) const noexcept {
		pointer = reinterpret_cast<V*>(Base::m_head);
	}

	/**
	 * Write @value to the packet.
	 * The head moves to the new position.
	 * @param value - variable to write from.
	 */
	template<typename V>
	inline void write(const V& value) noexcept {
		write_impl(value);
	}

	/**
	 * Write @value and @args to the packet.
	 * The head moves to the new position.
	 * @param value - variable to write from.
	 * @param args - variables to write from.
	 */
	template<typename V, typename... Args>
	inline void write(const V& value, const Args& ... args) noexcept {
		write_impl(value, args...);
	}

protected:

	template<typename V>
	inline void read_impl(V& value) noexcept {
		value = *(reinterpret_cast<const V*>(Base::m_head));
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
		*reinterpret_cast<V*>(Base::m_head) = value;
		Base::m_head += sizeof(V);
		Base::m_available -= sizeof(V);
	}

	template<typename V, typename... Args>
	inline void write_impl(const V& value, const Args& ... args) noexcept {
		write_impl(value);
		write_impl(args...);
	}

};

using RoMFrame = MFrame<const uint8_t>;
using RwMFrame = MFrame<uint8_t>;

}; // namespace proto
