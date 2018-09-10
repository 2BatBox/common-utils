#ifndef BINIO_PACKET_PACKETREADER_H
#define BINIO_PACKET_PACKETREADER_H

#include <cstdint>

#include "BasicPacket.h"

namespace binio {

/**
 * The PacketReader / PacketWriter design.
 * 
 * see BasicPacket.h for more details.
 * 
 * The head can be moved forward with following methods:
 * read(), read_memory(), write(), write_memory(), assign() and head_move().
 * 
 * The head also can be moved backward with head_move_back().
 * The tail can be moved with tail_move() and tail_move_back().
 * 
 **/

template <typename T>
class BasicPacketReader : public BasicPacket<T> {
	using Base = BasicPacket<T>;

protected:

	BasicPacketReader(T* buf, size_t len) noexcept :
	Base(buf, len) { }

public:

	/**
	 * Reset the state of the packet.
	 */
	void reset() noexcept {
		size_t off = Base::offset();
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
	template <typename V>
	inline void read(V& value) noexcept {
		read_impl(value);
	}

	/**
	 * Read @value and @args from the packet.
	 * The head moves to the new position.
	 * @param value - a variable to read to.
	 * @param args - variables to read to.
	 */
	template <typename V, typename... Args>
	inline void read(V& value, Args&... args) noexcept {
		read_impl(value, args...);
	}

	/**
	 * Read a memory area from the packet.
	 * The head moves to the new position.
	 * @param area - an area to read to.
	 */
	void read_area(MArea area) noexcept {
		size_t array_len = area.length();
		memcpy(area.begin(), Base::m_head, array_len);
		Base::m_head += array_len;
		Base::m_available -= array_len;
	}

	/**
	 * Assign a pointer to the head.
	 * The head moves to the new position.
	 * @param pointer - a pointer to assign.
	 */
	template <typename V>
	inline void assign(V*& pointer) noexcept {
		pointer = reinterpret_cast<V*>(Base::m_head);
		Base::m_head += sizeof (V);
		Base::m_available -= sizeof (V);
	}

	/**
	 * Assign a pointer to the head.
	 * The head doesn't move.
	 * @param pointer - a pointer to assign.
	 */
	template <typename V>
	inline void assign_stay(V*& pointer) const noexcept {
		pointer = reinterpret_cast<V*>(Base::m_head);
	}

	/**
	 * Assign a memory area to the head.
	 * The head moves to the new position.
	 * @param area - an array to assign.
	 * @param area_len - length of the area in bytes.
	 */
	inline void assign_const_area(MCArea& area, size_t length) noexcept {
		area = as_const_area(Base::m_head, length);
		Base::m_head += length;
		Base::m_available -= length;
	}

	/**
	 * Assign a memory area to the head.
	 * The head doesn't move.
	 * @param area - an array to assign.
	 * @param area_len - length of the area in bytes.
	 */
	inline void assign_const_area_stay(MCArea& area, size_t area_len) const noexcept {
		area = as_const_area(Base::m_head, area_len);
	}

protected:

	template <typename V>
	inline void read_impl(V& value) noexcept {
		value = *reinterpret_cast<const V*>(Base::m_head);
		Base::m_head += sizeof (V);
		Base::m_available -= sizeof (V);
	}

	template <typename V, typename... Args>
	inline void read_impl(V& value, Args&... args) noexcept {
		read_impl(value);
		read_impl(args...);
	}

};

class PacketReader : public BasicPacketReader<const uint8_t> {
	using Base = BasicPacketReader<const uint8_t>;

public:

	PacketReader(MCArea mem) noexcept :
	Base(mem.begin(), mem.length()) { }

	PacketReader(MArea mem) noexcept :
	Base(mem.begin(), mem.length()) { }

	PacketReader(const uint8_t* data, size_t bytes) noexcept :
	Base(data, bytes) { }
};

}; // namespace binio

#endif /* BINIO_PACKET_PACKETREADER_H */