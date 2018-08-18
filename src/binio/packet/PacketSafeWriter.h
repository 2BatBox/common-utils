#ifndef BINIO_PACKET_SAFE_WRITER_H
#define BINIO_PACKET_SAFE_WRITER_H

#include <cstdlib>
#include <cstring>

#include "PacketSafeReader.h"

namespace binio {

/**
 * see PacketSafeReader.h for more details.
 **/

template <typename S>
class PacketSafeWriter : public BasicPacketSafeReader<uint8_t, S> {
	using Base = BasicPacketSafeReader<uint8_t, S>;
	friend class PacketSafeReader<S>;

public:

	PacketSafeWriter(MArea range) noexcept :
	Base(range.begin(), range.length()) { }

	PacketSafeWriter(uint8_t* data, S bytes) noexcept :
	Base(data, bytes) { }

	/**
	 * Write one variable to the buffer and set the head to the new position.
	 * @param value - variable to write from.
	 * @return true - if the buffer is in its bounds after writing.
	 */
	template <typename V>
	bool write(const V& value) noexcept {
		if (Base::in_bounds) {
			if (sizeof (V) > Base::bytes_available) {
				Base::in_bounds = false;
			} else {
				write_unsafe(value);
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
	template <typename V, typename... Args>
	bool write(const V& value, const Args&... args) noexcept {
		if (Base::in_bounds) {
			if (Base::sizeof_args(value, args...) > Base::bytes_available) {
				Base::in_bounds = false;
			} else {
				write_unsafe(value, args...);
			}
		}
		return Base::in_bounds;
	}

	/**
	 * Write a memory area to the buffer and set the head to the new position.
	 * @param value - an area to write from
	 * @return true - if the buffer is in its bounds after writing
	 */
	bool write_mcarea(MCArea area) noexcept {
		if (Base::in_bounds) {
			S array_len = area.length();
			if (array_len > Base::bytes_available) {
				Base::in_bounds = false;
			} else {
				memcpy(Base::ptr_head, area.cbegin(), array_len);
				Base::ptr_head += array_len;
				Base::bytes_available -= array_len;
			}
		}
		return Base::in_bounds;
	}

	/**
	 * Assign a memory area to the head and set the head to the new position.
	 * @param area - an area to assign.
	 * @param area_len - length of the area in bytes.
	 * @return true - if the packet is in its bounds after assigning.
	 */
	bool assign_marea(MArea& area, S area_len) noexcept {
		if (Base::in_bounds) {
			if (area_len > Base::bytes_available) {
				Base::in_bounds = false;
			} else {
				area = as_marea(Base::ptr_head, area_len);
				Base::ptr_head += area_len;
				Base::bytes_available -= area_len;
			}
		}
		return Base::in_bounds;
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

#endif /* BINIO_PACKET_SAFE_WRITER_H */