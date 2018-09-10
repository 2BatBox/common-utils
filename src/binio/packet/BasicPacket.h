#ifndef BINIO_PACKET_BASICPACKET_H
#define BINIO_PACKET_BASICPACKET_H

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
 * Notes:
 * Type T MUST be a 8-bit unsigned integer (constant or not constant) type.
 * 
 **/

template <typename T>
class BasicPacket {
protected:
	T* m_head;
	size_t m_available; // bytes available to read/write
	size_t m_padding; // padding bytes
	size_t m_size; // size of the packet in bytes

	BasicPacket(T* buf, size_t len) noexcept :
	m_head(buf),
	m_available(len),
	m_padding(0),
	m_size(len) { }

public:

	/**
	 * @return The distance between 'begin' and 'end'
	 */
	inline size_t size() const noexcept {
		return m_size;
	}

	/**
	 * @return The distance between 'begin' and 'head'
	 */
	inline size_t offset() const noexcept {
		return m_size - m_available - m_padding;
	}

	/**
	 * @return The distance between 'head' and 'tail'
	 */
	inline size_t available() const noexcept {
		return m_available;
	}

	/**
	 * @return The distance between 'tail' and 'end'
	 */
	inline size_t padding() const noexcept {
		return m_padding;
	}

	/**
	 * @return true - if at least @bytes are available.
	 */
	inline bool available(size_t bytes) const noexcept {
		return bytes <= m_available;
	}

	/**
	 * @return the offset subarea as a memory area.
	 */
	inline BasicMArea<const T> offset_area() const noexcept {
		size_t off = offset();
		return BasicMArea<const T>(m_head - off, off);
	}

	/**
	 * @return the offset subarea as a memory area.
	 */
	inline BasicMArea<T> offset_area() noexcept {
		size_t off = offset();
		return BasicMArea<T>(m_head - off, off);
	}

	/**
	 * @return the available subarea as a memory area.
	 */
	inline BasicMArea<const T> available_area() const noexcept {
		return BasicMArea<const T>(m_head, m_available);
	}

	/**
	 * @return the available subarea as a memory area.
	 */
	inline BasicMArea<T> available_area() noexcept {
		return BasicMArea<T>(m_head, m_available);
	}

	/**
	 * @return the padding subarea as a memory area.
	 */
	inline BasicMArea<const T> padding_area() const noexcept {
		return BasicMArea<const T>(m_head + available(), m_padding);
	}

	/**
	 * @return the padding subarea as a memory area.
	 */
	inline BasicMArea<T> padding_area() noexcept {
		return BasicMArea<T>(m_head + available(), m_padding);
	}

	/**
	 * @return whole the packet memory area except the offset area.
	 */
	inline BasicMArea<const T> packet_area() const noexcept {
		return BasicMArea<const T>(m_head - offset, m_size);
	}

	/**
	 * @return whole the packet area except the offset area.
	 */
	inline BasicMArea<T> packet_area() noexcept {
		return BasicMArea<T>(m_head - offset, m_size);
	}

};

}; // namespace binio

#endif /* BINIO_PACKET_BASICPACKET_H */

