#ifndef BINIO_PACKET_READER_H
#define BINIO_PACKET_READER_H

#include <cstdlib>
#include <cstring>

#include "BasicPacket.h"

namespace binio {

/**
 * The PacketReader / PacketWriter design.
 * 
 * see BasicPacket.h for more details.
 * 
 * The head moves forward with following methods:
 * read(), read_memory(), write(), write_memory(), assign() and head_move().
 * 
 * The head also can be moved backward with head_move_back().
 * The tail can be moved with tail_move() and tail_move_back().
 * 
 **/

template <typename RawPtr, typename SizeType>
class BasicPacketReader : public BasicPacket<RawPtr, SizeType> {
    using Base = BasicPacket<RawPtr, SizeType>;

protected:

    BasicPacketReader(RawPtr* buf, SizeType len) noexcept :
    Base(buf, len) { }

public:

    /**
     * Reset the state of the packet.
     */
    void reset() noexcept
    {
        SizeType off = Base::offset();
        Base::ptr_head -= off;
        Base::bytes_available += off + Base::bytes_padding;
        Base::bytes_padding = 0;
    }

    /**
     * Move the head @bytes forward.
     * @param bytes - bytes to move.
     */
    void head_move(SizeType bytes) noexcept
    {
        Base::ptr_head += bytes;
        Base::bytes_available -= bytes;
    }

    /**
     * Move the head @bytes backward.
     * @param bytes - bytes to move.
     */
    void head_move_back(SizeType bytes) noexcept
    {
        Base::ptr_head -= bytes;
        Base::bytes_available += bytes;
    }

    /**
     * Move the tail @bytes forward.
     * @param bytes - bytes to move.
     */
    void tail_move(SizeType bytes) noexcept
    {
        Base::bytes_available += bytes;
        Base::bytes_padding -= bytes;
    }

    /**
     * Move the tail @bytes backward.
     * @param bytes - bytes to move.
     */
    void tail_move_back(SizeType bytes) noexcept
    {
        Base::bytes_available -= bytes;
        Base::bytes_padding += bytes;
    }

    /**
     * Read @value from the packet and set the head to a new position.
     * @param value - variable to read to.
     */
    template <typename T>
    void read(T& value) noexcept
    {
        read_unsafe(value);
    }

    /**
     * Read @value and @args from the packet and set the head to a new position.
     * @param value - a variable to read to.
     * @param args - variables to read to.
     */
    template <typename T, typename... Args>
    void read(T& value, Args&... args) noexcept
    {
        read_unsafe(value, args...);
    }

    /**
     * Read an array from the packet and set the head to a new position.
     * @param array - an array to read to.
     * @param array_len - amount of @array elements.
     */
    template <typename T>
    void read_memory(T* array, SizeType array_len) noexcept
    {
        array_len *= sizeof(T);
        memcpy(array, Base::ptr_head, array_len);
        Base::ptr_head += array_len;
        Base::bytes_available -= array_len;
    }

    /**
     * Assign an array of pointers to the head set the head to a new position.
     * @param array - an array to assign.
     * @param array_len - amount of @array elements.
     */
    template <typename T>
    void assign(T*& array, SizeType array_len) noexcept
    {
        array_len *= sizeof(T);
        array = reinterpret_cast<T*> (Base::ptr_head);
        Base::ptr_head += array_len;
        Base::bytes_available -= array_len;
    }

    /**
     * Assign one pointer to the head and set the head to a new position.
     * @param pointer - a pointer to assign.
     */
    template <typename T>
    void assign(T*& pointer) noexcept
    {
        pointer = reinterpret_cast<T*> (Base::ptr_head);
        Base::ptr_head += sizeof(T);
        Base::bytes_available -= sizeof(T);
    }

    /**
     * Assign one pointer to the head.
     * @param pointer - a pointer to assign.
     */
    template <typename T>
    void assign_stay(T*& pointer) const noexcept
    {
        pointer = reinterpret_cast<T*> (Base::ptr_head);
    }

protected:

    template <typename T>
    inline void read_unsafe(T& value) noexcept
    {
        value = *reinterpret_cast<const T*> (Base::ptr_head);
        Base::ptr_head += sizeof(T);
        Base::bytes_available -= sizeof(T);
    }

    template <typename T, typename... Args>
    inline void read_unsafe(T& value, Args&... args) noexcept
    {
        read_unsafe(value);
        read_unsafe(args...);
    }

};

template <typename SizeType>
class PacketReader : public BasicPacketReader<const uint8_t, SizeType> {
    using Base = BasicPacketReader<const uint8_t, SizeType>;

public:

    PacketReader(ByteConstBuffer mem) noexcept :
    Base(mem.data(), mem.length()) { }

    PacketReader(ByteBuffer mem) noexcept :
    Base(mem.data(), mem.length()) { }

    PacketReader(const uint8_t* data, SizeType bytes) noexcept :
    Base(data, bytes) { }
};

}; // namespace binio

#endif /* BINIO_PACKET_READER_H */