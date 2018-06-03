#ifndef BINIO_PACKET_WRITER_H
#define BINIO_PACKET_WRITER_H

#include <cstdlib>
#include <cstring>

#include "PacketReader.h"

// gcc 4.8.2's -Wnon-virtual-dtor is broken and turned on by -Weffc++
#if __GNUC__ < 3 || (__GNUC__ == 4 && __GNUC_MINOR__ <= 8)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnon-virtual-dtor"
#pragma GCC diagnostic ignored "-Weffc++"
#define GCC_DIAG_POP_NEEDED
#endif

namespace binio {

/**
 * see PacketReader.h for more details.
 **/

template <typename S>
class PacketWriter : public BasicPacketReader<uint8_t, S> {
	using Base = BasicPacketReader<uint8_t, S>;

public:

	PacketWriter(MemArea range) noexcept :
	Base(range.pointer(), range.length()) { }

	PacketWriter(uint8_t* data, S bytes) noexcept :
	Base(data, bytes) { }

	/**
	 * Write one variable to the buffer and set the head to a new position.
	 * @param value - variable to write from.
	 * @return true - if the buffer is in its bounds after writing.
	 */
	template <typename V>
	void write(const V& value) noexcept {
		write_unsafe(value);
	}

	/**
	 * Write @value and @args to the buffer and set it at the new position.
	 * @param value - variable to write from.
	 * @param args - variables to write from.
	 * @return true - if the buffer is in its bounds after writing.
	 */
	template <typename V, typename... Args>
	void write(const V& value, const Args&... args) noexcept {
		write_unsafe(value, args...);
	}

	/**
	 * Write an array to the buffer and set the head to a new position.
	 * @param value - variable to write from
	 * @param array_len - amount of @array elements.
	 * @return true - if the buffer is in its bounds after writing
	 */
	template <typename V>
	void write_memory(const V* array, S array_len) noexcept {
		array_len *= sizeof (V);
		memcpy(Base::ptr_head, array, array_len);
		Base::ptr_head += array_len;
		Base::bytes_available -= array_len;
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

#if defined(GCC_DIAG_POP_NEEDED)
#pragma GCC diagnostic pop
#undef GCC_DIAG_POP_NEEDED
#endif

#endif /* BINIO_PACKET_WRITER_H */