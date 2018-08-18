#ifndef BINIO_PACKET_READER_H
#define BINIO_PACKET_READER_H

#include <cstdlib>
#include <cstring>

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

template <typename T, typename S>
class BasicPacketReader : public BasicPacket<T, S> {
	using Base = BasicPacket<T, S>;

protected:

	BasicPacketReader(T* buf, S len) noexcept :
	Base(buf, len) { }

public:

	/**
	 * Reset the state of the packet.
	 */
	void reset() noexcept {
		S off = Base::offset();
		Base::ptr_head -= off;
		Base::bytes_available += off + Base::bytes_padding;
		Base::bytes_padding = 0;
	}

	/**
	 * Move the head @bytes forward.
	 * @param bytes - bytes to move.
	 */
	inline void head_move(S bytes) noexcept {
		Base::ptr_head += bytes;
		Base::bytes_available -= bytes;
	}

	/**
	 * Move the head @bytes backward.
	 * @param bytes - bytes to move.
	 */
	inline void head_move_back(S bytes) noexcept {
		Base::ptr_head -= bytes;
		Base::bytes_available += bytes;
	}

	/**
	 * Move the tail @bytes forward.
	 * @param bytes - bytes to move.
	 */
	inline void tail_move(S bytes) noexcept {
		Base::bytes_available += bytes;
		Base::bytes_padding -= bytes;
	}

	/**
	 * Move the tail @bytes backward.
	 * @param bytes - bytes to move.
	 */
	inline void tail_move_back(S bytes) noexcept {
		Base::bytes_available -= bytes;
		Base::bytes_padding += bytes;
	}

	/**
	 * Read @value from the packet and set the head to the new position.
	 * @param value - variable to read to.
	 */
	template <typename V>
	inline void read(V& value) noexcept {
		read_unsafe(value);
	}

	/**
	 * Read @value and @args from the packet and set the head to the new position.
	 * @param value - a variable to read to.
	 * @param args - variables to read to.
	 */
	template <typename V, typename... Args>
	inline void read(V& value, Args&... args) noexcept {
		read_unsafe(value, args...);
	}

	/**
	 * Read a memory area from the packet and set the head to the new position.
	 * @param area - an area to read to.
	 */
	void read_marea(MArea area) noexcept {
		S array_len = area.length();
		memcpy(area.begin(), Base::ptr_head, array_len);
		Base::ptr_head += array_len;
		Base::bytes_available -= array_len;
	}

	/**
	 * Assign one pointer to the head and set the head to the new position.
	 * @param pointer - a pointer to assign.
	 */
	template <typename V>
	inline void assign(V*& pointer) noexcept {
		pointer = reinterpret_cast<V*>(Base::ptr_head);
		Base::ptr_head += sizeof (V);
		Base::bytes_available -= sizeof (V);
	}

	/**
	 * Assign an area to the head and set the head to the new position.
	 * @param area - an array to assign.
	 * @param area_len - length of the area in bytes.
	 */
	inline void assign_mcarea(MCArea& area, S area_len) noexcept {
		area = as_mcarea(Base::ptr_head, area_len);
		Base::ptr_head += area_len;
		Base::bytes_available -= area_len;
	}

	/**
	 * Assign one pointer to the head.
	 * @param pointer - a pointer to assign.
	 */
	template <typename V>
	inline void assign_stay(V*& pointer) const noexcept {
		pointer = reinterpret_cast<V*>(Base::ptr_head);
	}


protected:

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
class PacketReader : public BasicPacketReader<const uint8_t, S> {
	using Base = BasicPacketReader<const uint8_t, S>;

public:

	PacketReader(MCArea mem) noexcept :
	Base(mem.begin(), mem.length()) { }

	PacketReader(MArea mem) noexcept :
	Base(mem.begin(), mem.length()) { }

	PacketReader(const uint8_t* data, S bytes) noexcept :
	Base(data, bytes) { }
};

}; // namespace binio

#endif /* BINIO_PACKET_READER_H */