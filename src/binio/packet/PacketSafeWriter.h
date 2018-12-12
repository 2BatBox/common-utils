#ifndef BINIO_PACKET_PACKETSAFEWRITER_H
#define BINIO_PACKET_PACKETSAFEWRITER_H

#include <cstdlib>
#include <cstring>

#include "PacketSafeReader.h"

namespace binio {

/**
 * see PacketSafeReader.h for more details.
 **/

class PacketSafeWriter : public BasicPacketSafeReader<uint8_t> {
	using Base = BasicPacketSafeReader<uint8_t>;

	friend class PacketSafeReader;

public:

	PacketSafeWriter(MArea range) noexcept :
		Base(range.begin(), range.length()) {}

	PacketSafeWriter(uint8_t* data, size_t bytes) noexcept :
		Base(data, bytes) {}

	/**
	 * Write @value to the packet.
	 * The head moves to the new position.
	 * @param value - variable to write from.
	 * @return true - if the buffer is in its bounds after writing.
	 */
	template<typename V>
	bool write(const V& value) noexcept {
		if(Base::in_bounds) {
			if(sizeof(V) > Base::m_available) {
				Base::in_bounds = false;
			} else {
				write_impl(value);
			}
		}
		return Base::in_bounds;
	}

	/**
	 * Write @value and @args to the packet.
	 * The head moves to the new position.
	 * @param value - variable to write from.
	 * @param args - variables to write from.
	 * @return true - if the buffer is in its bounds after writing.
	 */
	template<typename V, typename... Args>
	bool write(const V& value, const Args& ... args) noexcept {
		if(Base::in_bounds) {
			if(Base::sizeof_args(value, args...) > Base::m_available) {
				Base::in_bounds = false;
			} else {
				write_impl(value, args...);
			}
		}
		return Base::in_bounds;
	}

	/**
	 * Write a memory area to the packet.
	 * The head moves to the new position.
	 * @param value - an area to write from
	 * @return true - if the buffer is in its bounds after writing
	 */
	bool write_mcarea(MCArea area) noexcept {
		if(Base::in_bounds) {
			size_t array_len = area.length();
			if(array_len > Base::m_available) {
				Base::in_bounds = false;
			} else {
				memcpy(Base::m_head, area.cbegin(), array_len);
				Base::m_head += array_len;
				Base::m_available -= array_len;
			}
		}
		return Base::in_bounds;
	}

	/**
	 * Assign a pointer to the head.
	 * The head moves to the new position.
	 * @param area - an area to assign.
	 * @param area_len - length of the area in bytes.
	 * @return true - if the packet is in its bounds after assigning.
	 */
	bool assign_area(MArea& area, size_t area_len) noexcept {
		if(Base::in_bounds) {
			if(area_len > Base::m_available) {
				Base::in_bounds = false;
			} else {
				area = as_area(Base::m_head, area_len);
				Base::m_head += area_len;
				Base::m_available -= area_len;
			}
		}
		return Base::in_bounds;
	}

	/**
	 * Assign a pointer to the head.
	 * The head doesn't move.
	 * @param area - an area to assign.
	 * @param area_len - length of the area in bytes.
	 * @return true - if the packet is in its bounds after assigning.
	 */
	bool assign_area_stay(MArea& area, size_t area_len) noexcept {
		if(Base::in_bounds) {
			if(area_len > Base::m_available) {
				Base::in_bounds = false;
			} else {
				area = as_area(Base::m_head, area_len);
			}
		}
		return Base::in_bounds;
	}

protected:

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

}; // namespace binio

#endif /* BINIO_PACKET_PACKETSAFEWRITER_H */