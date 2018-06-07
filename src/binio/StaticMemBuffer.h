#ifndef BINIO_STATIC_MEM_BUFFER_H
#define BINIO_STATIC_MEM_BUFFER_H

#include "MemArea.h"

namespace binio {

/**
 * A StaticMemBuffer object represents a variable length static byte buffer with bounds checking.
 */

template <size_t capacity_bytes>
class StaticMemBuffer {
protected:
	size_t m_length;
	uint8_t m_buffer[capacity_bytes];

public:

	StaticMemBuffer() noexcept : m_length(0), m_buffer() { }

	inline operator const uint8_t*() const noexcept {
		return m_buffer;
	}

	inline operator uint8_t*() noexcept {
		return m_buffer;
	}

	inline const uint8_t* pointer() const noexcept {
		return m_buffer;
	}

	inline uint8_t* pointer() noexcept {
		return m_buffer;
	}

	inline size_t length() const noexcept {
		return m_length;
	}

	inline size_t capacity() const noexcept {
		return capacity_bytes;
	}

	inline operator bool() const noexcept {
		return pointer != nullptr;
	}

	// set data

	inline bool set(const StaticMemBuffer& membuf) noexcept {
		return set(membuf.pointer(), membuf.length());
	}

	inline bool set(MemConstArea marea) noexcept {
		return set(marea.pointer(), marea.length());
	}

	inline bool set(MemArea marea) noexcept {
		return set(marea.pointer(), marea.length());
	}

	inline bool set(const uint8_t* data, size_t length) noexcept {
		if (length > capacity_bytes || data == nullptr)
			return false;
		memcpy(m_buffer, data, length);
		m_length = length;
		return true;
	}

	// append data

	inline bool append(const StaticMemBuffer& membuf) noexcept {
		return append(membuf.pointer(), membuf.length());
	}

	inline bool append(MemConstArea array) noexcept {
		return append(array.pointer(), array.length());
	}

	inline bool append(MemArea array) noexcept {
		return append(array.pointer(), array.length());
	}

	inline bool append(uint8_t byte) noexcept {
		return append(&byte, 1);
	}

	inline bool append(const uint8_t* data, size_t length) noexcept {
		size_t bytes_left = capacity_bytes - length;
		if (length > bytes_left || data == nullptr)
			return false;

		memcpy(m_buffer + m_length, data, length);
		m_length += length;
		return true;
	}

	inline size_t clear() noexcept {
		return m_length = 0;
	}

	template <size_t size>
	inline bool operator==(const StaticMemBuffer<size>& lv) const noexcept {
		if (m_length == lv.length()) {
			return (memcmp(m_buffer, lv.pointer(), m_length) == 0);
		}
		return false;
	}

	template <size_t size>
	inline bool operator!=(const StaticMemBuffer<size>& lv) const noexcept {
		return not operator==(lv);
	}

	inline operator MemConstArea() const noexcept {
		return MemConstArea(m_buffer, m_length);
	}

	inline operator MemArea() noexcept {
		return MemArea(m_buffer, m_length);
	}

	//TODO: intersection checkers

};

}; // namespace binio

#endif /* BINIO_STATIC_MEM_BUFFER_H */

