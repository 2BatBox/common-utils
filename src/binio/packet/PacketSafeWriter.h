#ifndef BINIO_PACKET_SAFE_WRITER_H
#define BINIO_PACKET_SAFE_WRITER_H

#include <cstdlib>
#include <cstring>

#include "PacketSafeReader.h"

namespace binio {

/**
 * see PacketSafeReader.h for more details.
 **/

template <typename SizeType>
class PacketSafeWriter : public BasicPacketSafeReader<uint8_t, SizeType> {
    using Base = BasicPacketSafeReader<uint8_t, SizeType>;
    friend class PacketSafeReader<SizeType>;

public:

    PacketSafeWriter(ByteBuffer range) noexcept :
    Base(range.data(), range.length()) { }

    PacketSafeWriter(uint8_t* data, SizeType bytes) noexcept :
    Base(data, bytes) { }

    /**
     * Write one variable to the buffer and set the head to a new position.
     * @param value - variable to write from.
     * @return true - if the buffer is in its bounds after writing.
     */
    template <typename T>
    bool write(const T& value) noexcept
    {
        if (Base::in_bounds) {
            if (sizeof(T) > Base::bytes_available) {
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
    template <typename T, typename... Args>
    bool write(const T& value, const Args&... args) noexcept
    {
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
    template <typename T>
    bool write_memory(const T* array, SizeType array_len) noexcept
    {
        if (Base::in_bounds) {
            array_len *= sizeof(T);
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

    template <typename T>
    inline void write_unsafe(const T& value) noexcept
    {
        *reinterpret_cast<T*> (Base::ptr_head) = value;
        Base::ptr_head += sizeof(T);
        Base::bytes_available -= sizeof(T);
    }

    template <typename T, typename... Args>
    inline void write_unsafe(const T& value, const Args&... args) noexcept
    {
        write_unsafe(value);
        write_unsafe(args...);
    }
};

}; // namespace binio

#endif /* BINIO_PACKET_SAFE_WRITER_H */