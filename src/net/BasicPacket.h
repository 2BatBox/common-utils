#ifndef NET_BASIC_PACKET_H
#define NET_BASIC_PACKET_H

#include <cstdlib>

#include "../binio/ArrayPointer.h"

namespace net {

/**
 * The BasicPacket design.
 * 
 *                 head               tail
 *                   |                 |   
 *   | <- offset ->  | <- available -> | <- padding -> |
 *   |R|R|R|R|R|R|R|R|A|A|A|A|A|A|A|A|A|P|P|P|P|P|P|P|P|
 *   | <-------------------- size -------------------> |
 * begin                                              end
 * 
 * R - already read.
 * A - available to read.
 * P - padding bytes, they're not available to read.
 **/

template <typename SizeType>
class BasicPacket {
protected:
	const uint8_t* ptr_head;
	SizeType bytes_available;
	SizeType bytes_padding;
	SizeType bytes_size;

public:

	BasicPacket() :
	ptr_head(nullptr),
	bytes_available(0),
	bytes_padding(0),
	bytes_size(0) { }

	BasicPacket(binio::ByteArrayConstPtr range) :
	ptr_head(range.data()),
	bytes_available(range.length()),
	bytes_padding(0),
	bytes_size(bytes_available) { }

	template <typename T>
	BasicPacket(const T* raw_data, unsigned bytes) :
	ptr_head(reinterpret_cast<const uint8_t*>(raw_data)),
	bytes_available(bytes),
	bytes_padding(0),
	bytes_size(bytes_available) { }

	/**
	 * @return The distance between 'begin' and 'end'
	 */
	inline SizeType size() const noexcept {
		return bytes_size;
	}

	/**
	 * @return The distance between 'begin' and 'head'
	 */
	inline SizeType offset() const noexcept {
		return bytes_size - bytes_available - bytes_padding;
	}

	/**
	 * @return The distance between 'head' and 'tail'
	 */
	inline SizeType available() const noexcept {
		return bytes_available;
	}

	/**
	 * @return The distance between 'tail' and 'end'
	 */
	inline SizeType padding() const noexcept {
		return bytes_padding;
	}

	/**
	 * @return true - if at least @bytes are available.
	 */
	inline bool available(SizeType bytes) const noexcept {
		return bytes <= bytes_available;
	}

	/**
	 * @return offset subarea as a Range object.
	 */
	inline binio::ByteArrayConstPtr offset_range() const noexcept {
		SizeType offset = offset();
		return binio::ByteArrayConstPtr(ptr_head - offset, offset);
	}

	/**
	 * @return available subarea as a Range object.
	 */
	inline binio::ByteArrayConstPtr available_range() const noexcept {
		return binio::ByteArrayConstPtr(ptr_head, bytes_available);
	}

	/**
	 * @return padding subarea as a Range object.
	 */
	inline binio::ByteArrayConstPtr padding_range() const noexcept {
		return binio::ByteArrayConstPtr(ptr_head + available(), bytes_padding);
	}

	/**
	 * Move the head @bytes forward.
	 * @param bytes - bytes to move.
	 */
	inline void head_move(SizeType bytes) noexcept {
		ptr_head += bytes;
		bytes_available -= bytes;
	}

	/**
	 * Move the tail @bytes backward.
	 * @param bytes - bytes to move.
	 */
	inline void tail_move_back(SizeType bytes) noexcept {
		bytes_available -= bytes;
		bytes_padding += bytes;
	}

	/**
	 * Assign one pointer to the buffer head of the packet.
	 * @param pointer - a pointer to assign.
	 */
	template <typename T>
	void assign(T*& pointer) const noexcept {
		pointer = reinterpret_cast<T*>(ptr_head);
	}

	/**
	 * Assign one pointer to the buffer head of the packet with bounds checking.
	 * @param pointer - a pointer to assign.
	 * @return true - if the buffer has enough bytes in the available space to assign the pointer.
	 */
	template <typename T>
	bool assign_try(T*& pointer) const noexcept {
		if (bytes_available >= sizeof (T)) {
			pointer = reinterpret_cast<T*>(ptr_head);
			return true;
		}
		return false;
	}
};

}; // namespace net

#endif /* NET_BASIC_PACKET_H */

