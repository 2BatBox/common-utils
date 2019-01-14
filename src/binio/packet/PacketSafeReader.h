#ifndef BINIO_PACKET_PACKETSAFEREADER_H
#define BINIO_PACKET_PACKETSAFEREADER_H

#include <cstdlib>
#include <cstring>

#include "BasicPacket.h"

namespace binio {

/**
 * The PacketSafeReader / PacketSafeWriter design.
 * The class provides a bounds checking solution for safe reading/writing operations.
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

template<typename T>
class BasicPacketSafeReader : public BasicPacket<T> {
	using Base = BasicPacket<T>;

protected:
	bool in_bounds;

	BasicPacketSafeReader(T* buf, size_t len) noexcept :
		Base(buf, len), in_bounds(buf != nullptr) {}

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
		if(in_bounds) {
			size_t off = Base::offset();
			Base::m_head -= off;
			Base::m_available += off + Base::m_padding;
			Base::m_padding = 0;
		}
		return in_bounds;
	}

	/**
	 * Move the head @bytes forward.
	 * @param bytes - bytes to move.
	 * @return true - if the packet is in its bounds after moving.
	 */
	bool head_move(size_t bytes) noexcept {
		if(in_bounds) {
			if(bytes > Base::m_available) {
				in_bounds = false;
			} else {
				Base::m_head += bytes;
				Base::m_available -= bytes;
			}
		}
		return in_bounds;
	}

	/**
	 * Move the head @bytes backward.
	 * @param bytes - bytes to move.
	 * @return true - if the packet is in its bounds after moving.
	 */
	bool head_move_back(size_t bytes) noexcept {
		if(in_bounds) {
			if(bytes > Base::offset()) {
				in_bounds = false;
			} else {
				Base::m_head -= bytes;
				Base::m_available += bytes;
			}
		}
		return in_bounds;
	}

	/**
	 * Move the tail @bytes forward.
	 * @param bytes - bytes to move.
	 * @return true - if the packet is in its bounds after moving.
	 */
	bool tail_move(size_t bytes) noexcept {
		if(in_bounds) {
			if(bytes > Base::m_padding) {
				in_bounds = false;
			} else {
				Base::m_available += bytes;
				Base::m_padding -= bytes;
			}
		}
		return in_bounds;
	}

	/**
	 * Move the tail @bytes backward.
	 * @param bytes - bytes to move.
	 * @return true - if the packet is in its bounds after moving.
	 */
	bool tail_move_back(size_t bytes) noexcept {
		if(in_bounds) {
			if(bytes > Base::m_available) {
				in_bounds = false;
			} else {
				Base::m_available -= bytes;
				Base::m_padding += bytes;
			}
		}
		return in_bounds;
	}

	/**
	 * Read @value from the packet.
	 * The head moves to the new position.
	 * @param value - variable to read to.
	 * @return true - if the packet is in its bounds after reading.
	 */
	template<typename V>
	bool read(V& value) noexcept {
		if(in_bounds) {
			if(sizeof(V) > Base::m_available) {
				in_bounds = false;
			} else {
				read_impl(value);
			}
		}
		return in_bounds;
	}


	/**
	 * Read @value as a little endian one.
 	 * The head moves to the new position.
 	 * @param value - variable to read to.
 	 * @return true - if the packet is in its bounds after reading.
 	 */
	template<typename V>
	bool read_little_endian(V& value) noexcept {
		if(in_bounds) {
			if(sizeof(V) > Base::m_available) {
				in_bounds = false;
			} else {
				read_little_endian_impl(value);
			}
		}
		return in_bounds;
	}

	/**
	 * Read @bytes wide integer @value as a little endian one.
	 * The head moves to the new position.
	 * @param value - variable to read to.
	 * @return true - if the packet is in its bounds after reading.
	 */
	template<typename V>
	bool read_little_endian(V& value, const uint8_t bytes) noexcept {
		if(in_bounds) {
			if(bytes > Base::m_available) {
				in_bounds = false;
			} else {
				read_little_endian_impl(value, bytes);
			}
		}
		return in_bounds;
	}

	/**
	 * Read @bytes wide integer @value as a big endian one.
 	 * The head moves to the new position.
 	 * @param value - variable to read to.
 	 * @return true - if the packet is in its bounds after reading.
 	 */
	template<typename V>
	bool read_big_endian(V& value) noexcept {
		if(in_bounds) {
			if(sizeof(V) > Base::m_available) {
				in_bounds = false;
			} else {
				read_big_endian_impl(value);
			}
		}
		return in_bounds;
	}

	/**
	* Read @bytes wide integer @value as a big endian one.
 	* The head moves to the new position.
 	* @param value - variable to read to.
	* @return true - if the packet is in its bounds after reading.
 	*/
	template<typename V>
	bool read_big_endian(V& value, const uint8_t bytes) noexcept {
		if(in_bounds) {
			if(bytes > Base::m_available) {
				in_bounds = false;
			} else {
				read_big_endian_impl(value, bytes);
			}
		}
		return in_bounds;
	}

	/**
	 * Read @value and @args from the packet.
	 * The head moves to the new position.
	 * @param value - a variable to read to.
	 * @param args - variables to read to.
	 * @return true - if the packet is in its bounds after reading.
	 */
	template<typename V, typename... Args>
	bool read(V& value, Args& ... args) noexcept {
		if(in_bounds) {
			if(sizeof_args(value, args...) > Base::m_available) {
				in_bounds = false;
			} else {
				read_impl(value, args...);
			}
		}
		return in_bounds;
	}

	/**
	 * Read a memory area from the packet.
	 * The head moves to the new position.
	 * @param area - an array to read to.
	 * @return true - if the packet is in its bounds after reading.
	 */
	bool read_area(MArea area) noexcept {
		if(in_bounds) {
			size_t array_len = area.length();
			if(array_len > Base::m_available) {
				in_bounds = false;
			} else {
				memcpy(area.begin(), Base::m_head, array_len);
				Base::m_head += array_len;
				Base::m_available -= array_len;
			}
		}
		return in_bounds;
	}

	/**
	 * Assign a pointer to the head.
	 * The head moves to the new position.
	 * @param pointer - a pointer to assign.
	 * @return true - if the packet is in its bounds after assigning.
	 */
	template<typename V>
	bool assign(V*& pointer) noexcept {
		if(in_bounds) {
			if(sizeof(V) > Base::m_available) {
				in_bounds = false;
			} else {
				pointer = reinterpret_cast<V*>(Base::m_head);
				Base::m_head += sizeof(V);
				Base::m_available -= sizeof(V);
			}
		}
		return in_bounds;
	}

	/**
	 * Assign a pointer to the head.
	 * The head doesn't move.
	 * @param pointer - a pointer to assign.
	 * @return true - if the packet is in its bounds after assigning.
	 */
	template<typename V>
	bool assign_stay(V*& pointer) noexcept {
		if(in_bounds) {
			if(sizeof(V) > Base::m_available) {
				in_bounds = false;
			} else {
				pointer = reinterpret_cast<V*>(Base::m_head);
			}
		}
		return in_bounds;
	}

	/**
	 * Assign a memory area to the head.
	 * The head moves to the new position.
	 * @param area - an area to assign.
	 * @param area_len - length of the area in bytes.
	 * @return true - if the packet is in its bounds after assigning.
	 */
	bool assign_const_area(MCArea& area, size_t area_len) noexcept {
		if(in_bounds) {
			if(area_len > Base::m_available) {
				in_bounds = false;
			} else {
				area = as_const_area(Base::m_head, area_len);
				Base::m_head += area_len;
				Base::m_available -= area_len;
			}
		}
		return in_bounds;
	}

	/**
	 * Assign a memory area to the head.
	 * The head doesn't move.
	 * @param area - an area to assign.
	 * @param area_len - length of the area in bytes.
	 * @return true - if the packet is in its bounds after assigning.
	 */
	bool assign_const_area_stay(MCArea& area, size_t area_len) noexcept {
		if(in_bounds) {
			if(area_len > Base::m_available) {
				in_bounds = false;
			} else {
				area = as_const_area(Base::m_head, area_len);
			}
		}
		return in_bounds;
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
	inline void read_little_endian_impl(V& value) noexcept {
		value = 0;
		Base::m_head += sizeof(value);
		uint8_t bytes_left = sizeof(value);
		while(bytes_left--) {
			Base::m_head--;
			value <<= 8;
			value |= *Base::m_head & 0xFF;
		}
		Base::m_head += sizeof(value);
		Base::m_available -= sizeof(value);
	}

	template<typename V>
	inline void read_little_endian_impl(V& value, const uint8_t bytes) noexcept {
		value = 0;
		Base::m_head += bytes;
		uint8_t bytes_left = bytes;
		while(bytes_left--) {
			Base::m_head--;
			value <<= 8;
			value |= *Base::m_head & 0xFF;
		}
		Base::m_head += bytes;
		Base::m_available -= bytes;
	}

	template<typename V>
	inline void read_big_endian_impl(V& value) noexcept {
		value = 0;
		uint8_t bytes_left = sizeof(value);
		while(bytes_left--) {
			value <<= 8;
			value |= *Base::m_head & 0xFF;
			Base::m_head++;
		}
		Base::m_available -= sizeof(value);
	}

	template<typename V>
	inline void read_big_endian_impl(V& value, const uint8_t bytes) noexcept {
		value = 0;
		uint8_t bytes_left = bytes;
		while(bytes_left--) {
			value <<= 8;
			value |= *Base::m_head & 0xFF;
			Base::m_head++;
		}
		Base::m_available -= bytes;
	}

};

class PacketSafeReader : public BasicPacketSafeReader<const uint8_t> {
	using Base = BasicPacketSafeReader<const uint8_t>;

public:

	PacketSafeReader(MCArea range) noexcept :
		Base(range.begin(), range.length()) {}

	PacketSafeReader(MArea range) noexcept :
		Base(range.begin(), range.length()) {}

	PacketSafeReader(const uint8_t* data, size_t bytes) noexcept :
		Base(data, bytes) {}
};

}; // namespace binio

#endif /* BINIO_PACKET_PACKETSAFEREADER_H */