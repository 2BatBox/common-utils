#ifndef BINIO_VL_BYTE_ARRAY_H
#define BINIO_VL_BYTE_ARRAY_H

#include "ByteBuffer.h"

namespace binio {

/**
 * Class binio::VLByteArray represents a variable length byte array with bounds checking.
 * No memory management is provided.
 */
template <typename SizeType, SizeType max_length>
class VLByteArray {
protected:
    uint8_t array[max_length];
    SizeType length_bytes;

public:

    VLByteArray() noexcept : array(), length_bytes(0) { }

    inline const uint8_t* data() const noexcept
    {
        return array;
    }

    inline uint8_t* data() noexcept
    {
        return array;
    }

    inline SizeType length() const noexcept
    {
        return length_bytes;
    }

    inline bool set_value(ByteConstBuffer array) noexcept
    {
        return set_value(array.data(), array.length());
    }

    inline bool set_value(ByteBuffer array) noexcept
    {
        return set_value(array.data(), array.length());
    }

    inline bool set_value(const uint8_t* data, size_t length) noexcept
    {
        if (length > max_length || data == nullptr)
            return false;
        memcpy(array, data, length);
        length_bytes = length;
        return true;
    }

    inline bool operator==(const VLByteArray& lv) const noexcept
    {
        if (length_bytes == lv.length_bytes && length_bytes <= max_length) {
            return(memcmp(array, lv.array, length_bytes) == 0);
        }
        return false;
    }

    inline bool operator!=(const VLByteArray& lv) const noexcept
    {
        return not operator==(lv);
    }

    template <typename P>
    bool assign(const P*& pointer) const noexcept
    {
        if (sizeof(P) > length_bytes) {
            return false;
        } else {
            pointer = reinterpret_cast<const P*> (array);
            return true;
        }
    }

    template <typename P>
    bool assign(P*& pointer) noexcept
    {
        if (sizeof(P) > length_bytes) {
            return false;
        } else {
            pointer = reinterpret_cast<P*> (array);
            return true;
        }
    }

    template <typename P>
    bool assign(const P*& pointer, unsigned index) const noexcept
    {
        if (sizeof(P) + index > length_bytes) {
            return false;
        } else {
            pointer = reinterpret_cast<const P*> (array + index);
            return true;
        }
    }

    template <typename P>
    bool assign(P*& pointer, unsigned index) noexcept
    {
        if (sizeof(P) + index > length_bytes) {
            return false;
        } else {
            pointer = reinterpret_cast<P*> (array + index);
            return true;
        }
    }
};

}; // namespace binio

#endif /* BINIO_VL_BYTE_ARRAY_H */

