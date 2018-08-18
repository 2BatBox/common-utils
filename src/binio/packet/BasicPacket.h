#ifndef BINIO_BASIC_PACKET_H
#define BINIO_BASIC_PACKET_H

#include <cstdlib>

#include "../MArea.h"

namespace binio {

/**
 * The BasicPacket design.
 * The class does not provide bounds checking and does not provide memory management either.
 * 
 *                 head               tail
 *                   |                 |   
 *   | <- offset ->  | <- available -> | <- padding -> |
 *   |R|R|R|R|R|R|R|R|A|A|A|A|A|A|A|A|A|P|P|P|P|P|P|P|P|
 *   | <-------------------- size -------------------> |
 * begin                                              end
 * 
 * R - already read/write.
 * A - available to read/write.
 * P - padding bytes, they're not available to read/write.
 * 
 * The main memory area is divided into three subareas called 'offset', 'available' and 'padding'.
 * 
 * There is no way to move 'begin' and 'end' points but 'head' and 'tail' can be moved.
 * Moving 'head' and 'tail' points affect the subareas they start or end with.
 * 
 **/

template <typename T, typename S>
class BasicPacket {
protected:
	T* ptr_head;
	S bytes_available;
	S bytes_padding;
	S bytes_size;

	BasicPacket(T* buf, S len) noexcept :
	ptr_head(buf),
	bytes_available(len),
	bytes_padding(0),
	bytes_size(len) { }

public:

	/**
	 * @return The distance between 'begin' and 'end'
	 */
	inline S size() const noexcept {
		return bytes_size;
	}

	/**
	 * @return The distance between 'begin' and 'head'
	 */
	inline S offset() const noexcept {
		return bytes_size - bytes_available - bytes_padding;
	}

	/**
	 * @return The distance between 'head' and 'tail'
	 */
	inline S available() const noexcept {
		return bytes_available;
	}

	/**
	 * @return The distance between 'tail' and 'end'
	 */
	inline S padding() const noexcept {
		return bytes_padding;
	}

	/**
	 * @return true - if at least @bytes are available.
	 */
	inline bool available(S bytes) const noexcept {
		return bytes <= bytes_available;
	}

	/**
	 * @return the offset subarea as a MArea object.
	 */
	inline BasicMArea<const T> marea_offset() const noexcept {
		S offset = offset();
		return BasicMArea<const T>(ptr_head - offset, offset);
	}

	/**
	 * @return the offset subarea as a MArea object.
	 */
	inline BasicMArea<T> marea_offset() noexcept {
		S offset = offset();
		return BasicMArea<T>(ptr_head - offset, offset);
	}

	/**
	 * @return the available subarea as a MArea object.
	 */
	inline BasicMArea<const T> marea_available() const noexcept {
		return BasicMArea<const T>(ptr_head, bytes_available);
	}

	/**
	 * @return the available subarea as a MArea object.
	 */
	inline BasicMArea<T> marea_available() noexcept {
		return BasicMArea<T>(ptr_head, bytes_available);
	}

	/**
	 * @return the padding subarea as a MArea object.
	 */
	inline BasicMArea<const T> marea_padding() const noexcept {
		return BasicMArea<const T>(ptr_head + available(), bytes_padding);
	}

	/**
	 * @return the padding subarea as a MArea object.
	 */
	inline BasicMArea<T> marea_padding() noexcept {
		return BasicMArea<T>(ptr_head + available(), bytes_padding);
	}

	/**
	 * @return whole the packet area as a MArea object.
	 */
	inline BasicMArea<const T> marea_packet() const noexcept {
		return BasicMArea<const T>(ptr_head - offset, bytes_size);
	}

	/**
	 * @return whole the packet area as a MArea object.
	 */
	inline BasicMArea<T> marea_packet() noexcept {
		return BasicMArea<T>(ptr_head - offset, bytes_size);
	}

};

}; // namespace binio

#endif /* BINIO_BASIC_PACKET_H */

