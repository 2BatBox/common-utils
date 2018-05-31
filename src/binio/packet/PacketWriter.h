#ifndef BINIO_PACKET_WRITER_H
#define BINIO_PACKET_WRITER_H

#include <cstdlib>
#include <cstring>

#include "PacketReader.h"

namespace binio {

/**
 * see PacketReader.h for more details.
 **/

template <typename SizeType>
class PacketWriter : public BasicPacketReader<uint8_t, SizeType> {
	using Base = BasicPacketReader<uint8_t, SizeType>;

public:

	PacketWriter(ByteBuffer range) noexcept :
	Base(range.data(), range.length()) { }

	PacketWriter(uint8_t* data, SizeType bytes) noexcept :
	Base(data, bytes) { }

	/**
	 * Write one variable to the buffer and set the head to a new position.
	 * @param value - variable to write from.
	 * @return true - if the buffer is in its bounds after writing.
	 */
	template <typename T>
	void write(const T& value) noexcept {
		write_unsafe(value);
	}

	/**
	 * Write @value and @args to the buffer and set it at the new position.
	 * @param value - variable to write from.
	 * @param args - variables to write from.
	 * @return true - if the buffer is in its bounds after writing.
	 */
	template <typename T, typename... Args>
	void write(const T& value, const Args&... args) noexcept {
		write_unsafe(value, args...);
	}

	/**
	 * Write an array to the buffer and set the head to a new position.
	 * @param value - variable to write from
	 * @param array_len - amount of @array elements.
	 * @return true - if the buffer is in its bounds after writing
	 */
	template <typename T>
	void write_memory(const T* array, SizeType array_len) noexcept {
		array_len *= sizeof (T);
		memcpy(Base::ptr_head, array, array_len);
		Base::ptr_head += array_len;
		Base::bytes_available -= array_len;
	}

protected:

	template <typename T>
	inline void write_unsafe(const T& value) noexcept {
		*reinterpret_cast<T*>(Base::ptr_head) = value;
		Base::ptr_head += sizeof (T);
		Base::bytes_available -= sizeof (T);
	}

	template <typename T, typename... Args>
	inline void write_unsafe(const T& value, const Args&... args) noexcept {
		write_unsafe(value);
		write_unsafe(args...);
	}
};

}; // namespace binio

#endif /* BINIO_PACKET_WRITER_H */