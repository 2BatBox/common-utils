#ifndef BINIO_BASIC_PACKET_H
#define BINIO_BASIC_PACKET_H

#include <cstdlib>

#include "../MemArea.h"

// gcc 4.8.2's -Wnon-virtual-dtor is broken and turned on by -Weffc++
#if __GNUC__ < 3 || (__GNUC__ == 4 && __GNUC_MINOR__ <= 8)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnon-virtual-dtor"
#pragma GCC diagnostic ignored "-Weffc++"
#define GCC_DIAG_POP_NEEDED
#endif

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
	 * @return the offset subrange as a MemArea object.
	 */
	inline BasicMemArea<const T> offset_mem_area() const noexcept {
		S offset = offset();
		return BasicMemArea<const T>(ptr_head - offset, offset);
	}

	/**
	 * @return the offset subrange as a MemArea object.
	 */
	inline BasicMemArea<T> offset_mem_area() noexcept {
		S offset = offset();
		return BasicMemArea<T>(ptr_head - offset, offset);
	}

	/**
	 * @return the available subrange as a MemArea object.
	 */
	inline BasicMemArea<const T> available_mem_area() const noexcept {
		return BasicMemArea<const T>(ptr_head, bytes_available);
	}

	/**
	 * @return the available subrange as a MemArea object.
	 */
	inline BasicMemArea<T> available_mem_area() noexcept {
		return BasicMemArea<T>(ptr_head, bytes_available);
	}

	/**
	 * @return the padding subrange as a MemArea object.
	 */
	inline BasicMemArea<const T> padding_mem_area() const noexcept {
		return BasicMemArea<const T>(ptr_head + available(), bytes_padding);
	}

	/**
	 * @return the padding subrange as a MemArea object.
	 */
	inline BasicMemArea<T> padding_mem_area() noexcept {
		return BasicMemArea<T>(ptr_head + available(), bytes_padding);
	}

	/**
	 * @return whole the packet area as a MemArea object.
	 */
	inline BasicMemArea<const T> as_mem_area() const noexcept {
		return BasicMemArea<const T>(ptr_head - offset, bytes_size);
	}

	/**
	 * @return whole the packet area as a MemArea object.
	 */
	inline BasicMemArea<T> as_mem_area() noexcept {
		return BasicMemArea<T>(ptr_head - offset, bytes_size);
	}

};

}; // namespace binio

#if defined(GCC_DIAG_POP_NEEDED)
#pragma GCC diagnostic pop
#undef GCC_DIAG_POP_NEEDED
#endif

#endif /* BINIO_BASIC_PACKET_H */

