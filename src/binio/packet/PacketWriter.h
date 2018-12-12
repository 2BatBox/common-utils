#ifndef BINIO_PACKET_PACKETWRITER_H
#define BINIO_PACKET_PACKETWRITER_H

#include <cstdlib>
#include <cstring>

#include "PacketReader.h"

namespace binio {

/**
 * see PacketReader.h for more details.
 **/

class PacketWriter : public BasicPacketReader<uint8_t> {
	using Base = BasicPacketReader<uint8_t>;

public:
	PacketWriter(MArea range) noexcept :
		Base(range.begin(), range.length()) {}

	PacketWriter(uint8_t* data, size_t bytes) noexcept :
		Base(data, bytes) {}

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

	/**
	 * Write a memory array to the packet.
	 * The head moves to the new position.
	 * @param area - an area to write from
	 */
	void write_area(MCArea area) noexcept {
		size_t array_len = area.length();
		memcpy(Base::m_head, area.begin(), array_len);
		Base::m_head += array_len;
		Base::m_available -= array_len;
	}

	/**
	 * Assign a memory area to the head.
	 * The head moves to the new position.
	 * @param area - an array to assign.
	 * @param area_len - length of the area in bytes.
	 */
	inline void assign_area(MArea& area, size_t area_len) noexcept {
		area = as_area(Base::m_head, area_len);
		Base::m_head += area_len;
		Base::m_available -= area_len;
	}

	/**
	 * Assign a memory area to the head.
	 * The head doesn't move.
	 * @param area - an array to assign.
	 * @param area_len - length of the area in bytes.
	 */
	inline void assign_area_stay(MArea& area, size_t area_len) const noexcept {
		area = as_area(Base::m_head, area_len);
	}

protected:

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

}; // namespace binio

#endif /* BINIO_PACKET_PACKETWRITER_H */