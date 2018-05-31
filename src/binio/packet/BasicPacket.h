#ifndef BINIO_BASIC_PACKET_H
#define BINIO_BASIC_PACKET_H

#include <cstdlib>

#include "../ByteBuffer.h"

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
 * The main data area is divided into three subareas called 'offset', 'available' and 'padding'.
 * 
 * There is no way to move 'begin' and 'end' points but 'head' and 'tail' can be moved.
 * Moving 'head' and 'tail' points affect the subareas they start or end with.
 * 
 **/

template <typename RawPtr, typename SizeType>
class BasicPacket {
protected:
	RawPtr* ptr_head;
	SizeType bytes_available;
	SizeType bytes_padding;
	SizeType bytes_size;

	BasicPacket(RawPtr* buf, SizeType len) noexcept :
	ptr_head(buf),
	bytes_available(len),
	bytes_padding(0),
	bytes_size(len) { }

public:

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
	 * @return the offset subrange as an ByteBuffer object.
	 */
	inline BasicBuffer<const RawPtr> offset_as_buffer() const noexcept {
		SizeType offset = offset();
		return BasicBuffer<const RawPtr>(ptr_head - offset, offset);
	}

	/**
	 * @return the offset subrange as an ByteBuffer object.
	 */
	inline BasicBuffer<RawPtr> offset_as_buffer() noexcept {
		SizeType offset = offset();
		return BasicBuffer<RawPtr>(ptr_head - offset, offset);
	}

	/**
	 * @return the available subrange as an ByteBuffer object.
	 */
	inline BasicBuffer<const RawPtr> available_as_buffer() const noexcept {
		return BasicBuffer<const RawPtr>(ptr_head, bytes_available);
	}

	/**
	 * @return the available subrange as an ByteBuffer object.
	 */
	inline BasicBuffer<RawPtr> available_as_buffer() noexcept {
		return BasicBuffer<RawPtr>(ptr_head, bytes_available);
	}

	/**
	 * @return the padding subrange as an ByteBuffer object.
	 */
	inline BasicBuffer<const RawPtr> padding_as_buffer() const noexcept {
		return BasicBuffer<const RawPtr>(ptr_head + available(), bytes_padding);
	}

	/**
	 * @return the padding subrange as an ByteBuffer object.
	 */
	inline BasicBuffer<RawPtr> padding_as_buffer() noexcept {
		return BasicBuffer<RawPtr>(ptr_head + available(), bytes_padding);
	}

	/**
	 * @return whole the packet area as an ByteBuffer object.
	 */
	inline BasicBuffer<const RawPtr> as_buffer() const noexcept {
		return BasicBuffer<const RawPtr>(ptr_head - offset, bytes_size);
	}

	/**
	 * @return whole the packet area as an ByteBuffer object.
	 */
	inline BasicBuffer<RawPtr> as_buffer() noexcept {
		return BasicBuffer<RawPtr>(ptr_head - offset, bytes_size);
	}

};

}; // namespace binio

#endif /* BINIO_BASIC_PACKET_H */

