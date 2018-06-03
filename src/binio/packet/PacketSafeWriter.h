#ifndef BINIO_PACKET_SAFE_WRITER_H
#define BINIO_PACKET_SAFE_WRITER_H

#include <cstdlib>
#include <cstring>

#include "PacketSafeReader.h"

// gcc 4.8.2's -Wnon-virtual-dtor is broken and turned on by -Weffc++
#if __GNUC__ < 3 || (__GNUC__ == 4 && __GNUC_MINOR__ <= 8)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnon-virtual-dtor"
#pragma GCC diagnostic ignored "-Weffc++"
#define GCC_DIAG_POP_NEEDED
#endif

namespace binio {

/**
 * see PacketSafeReader.h for more details.
 **/

template <typename S>
class PacketSafeWriter : public BasicPacketSafeReader<uint8_t, S> {
	using Base = BasicPacketSafeReader<uint8_t, S>;
	friend class PacketSafeReader<S>;

public:

	PacketSafeWriter(MemArea range) noexcept :
	Base(range.pointer(), range.length()) { }

	PacketSafeWriter(uint8_t* data, S bytes) noexcept :
	Base(data, bytes) { }

	/**
	 * Write one variable to the buffer and set the head to a new position.
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
	 * Write an array to the buffer and set the head to a new position.
	 * @param value - variable to write from
	 * @param array_len - amount of @array elements.
	 * @return true - if the buffer is in its bounds after writing
	 */
	template <typename V>
	bool write_memory(const V* array, S array_len) noexcept {
		if (Base::in_bounds) {
			array_len *= sizeof (V);
			if (array_len > Base::bytes_available) {
				Base::in_bounds = false;
			} else {
				memcpy(Base::ptr_head, array, array_len);
				Base::ptr_head += array_len;
				Base::bytes_available -= array_len;
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

#if defined(GCC_DIAG_POP_NEEDED)
#pragma GCC diagnostic pop
#undef GCC_DIAG_POP_NEEDED
#endif

#endif /* BINIO_PACKET_SAFE_WRITER_H */