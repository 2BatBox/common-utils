#pragma once

#include <cstdlib>

namespace proto {

/**
 * The Basic Memory Frame design.
 * An instance of BasicMFrame class represents a memory area divided into three subareas.
 * The class does not provide bounds checking and does not provide memory management either.
 * 
 *                 head               tail
 *                   |                 |   
 *   | <- offset ->  | <- available -> | <- padding -> |
 *   |R|R|R|R|R|R|R|R|A|A|A|A|A|A|A|A|A|P|P|P|P|P|P|P|P|
 *   | <-------------------- size -------------------> |
 * begin                                              end
 * 
 * R - already read/written.
 * A - available to read/write.
 * P - padding bytes, they're not available to read/write.
 * 
 * The frame is divided into three subareas called 'offset', 'available' and 'padding'.
 * 
 * There is no way to move 'begin' and 'end' points but 'head' and 'tail' can be moved.
 * Moving 'head' and 'tail' points affects the subareas they start or end with.
 * 
 * Notes:
 * Type T MUST be a 8-bit unsigned integer (constant or not constant) type.
 * 
 **/

template<typename T>
class BasicMFrame {
protected:
	T* m_head;
	size_t m_available; // bytes available to read/write
	size_t m_padding; // padding bytes
	size_t m_size; // size of the whole memory frame in bytes

	template <typename Ptr>
	BasicMFrame(Ptr* buffer, size_t buffer_bytes) noexcept
		: m_head(reinterpret_cast<T*>(buffer))
		, m_available(buffer_bytes)
		, m_padding(0)
		, m_size(buffer_bytes) {}

public:

	/**
	 * @return The 'begin' pointer.
	 */
	inline T* begin() const noexcept {
		return m_head;
	}

	/**
	 * @return The 'head' pointer.
	 */
	inline T* head() const noexcept {
		return m_head + offset();
	}

	/**
	 * @return The 'tail' pointer.
	 */
	inline T* tail() const noexcept {
		return head() + available();
	}

	/**
	 * @return The 'end' pointer.
	 */
	inline T* end() const noexcept {
		return m_head + m_size;
	}

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

};

}; // namespace proto

