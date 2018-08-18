#ifndef BINIO_PACKET_WRITER_H
#define BINIO_PACKET_WRITER_H

#include <cstdlib>
#include <cstring>

#include "PacketReader.h"

namespace binio {

/**
 * see PacketReader.h for more details.
 **/

template <typename S>
class PacketWriter : public BasicPacketReader<uint8_t, S> {
	using Base = BasicPacketReader<uint8_t, S>;

public:

	PacketWriter(MArea range) noexcept :
	Base(range.begin(), range.length()) { }

	PacketWriter(uint8_t* data, S bytes) noexcept :
	Base(data, bytes) { }

	/**
	 * Write one variable to the buffer and set the head to the new position.
	 * @param value - variable to write from.
	 */
	template <typename V>
	inline void write(const V& value) noexcept {
		write_unsafe(value);
	}

	/**
	 * Write @value and @args to the buffer and set it at the new position.
	 * @param value - variable to write from.
	 * @param args - variables to write from.
	 */
	template <typename V, typename... Args>
	inline void write(const V& value, const Args&... args) noexcept {
		write_unsafe(value, args...);
	}

	/**
	 * Write an array to the buffer and set the head to the new position.
	 * @param area - an area to write from
	 */
	void write_mcarea(MCArea area) noexcept {
		S array_len = area.length();
		memcpy(Base::ptr_head, area.begin(), array_len);
		Base::ptr_head += array_len;
		Base::bytes_available -= array_len;
	}

	/**
	 * Assign an area to the head and set the head to the new position.
	 * @param area - an array to assign.
	 * @param area_len - length of the area in bytes.
	 */
	inline void assign_marea(MArea& area, S area_len) noexcept {
		area = as_marea(Base::ptr_head, area_len);
		Base::ptr_head += area_len;
		Base::bytes_available -= area_len;
	}

protected:

	template <typename V>
	inline void write_unsafe(const V& value) noexcept {
		*reinterpret_cast<V*>(Base::ptr_head) = value;
		Base::ptr_head += sizeof (V);
		Base::bytes_available -= sizeof (V);
	}

	template <typename V, typename... Args>
	inline void write_unsafe(const V& value, const Args&... args) noexcept {
		write_unsafe(value);
		write_unsafe(args...);
	}
};

}; // namespace binio

#endif /* BINIO_PACKET_WRITER_H */