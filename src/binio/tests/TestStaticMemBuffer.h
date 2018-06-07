#ifndef BINIO_TESTS_STATIC_MEM_BUFFER_H
#define BINIO_TESTS_STATIC_MEM_BUFFER_H

#include <assert.h>
#include <memory>
#include <iostream>
#include <cstdio>

#include "../StaticMemBuffer.h"

namespace binio {

class TestStaticMemBuffer {
    using Buffer8 = StaticMemBuffer<8>;
    using Buffer16 = StaticMemBuffer<16>;

    static void test_init() noexcept
    {
        Buffer8 buf8;
        Buffer16 buf16;
        assert(buf8.length() == 0);
        assert(buf16.length() == 0);
        assert(buf8.capacity() == 8);
        assert(buf16.capacity() == 16);
        assert(buf8 == buf16);
    }

    static void test_comparison() noexcept
    {
        Buffer8 buf8;
        Buffer8 buf16;

        for (uint8_t i = 0; i < buf8.capacity() - 1; i++) {
            buf8.append(i);
            buf16.append(i);
        }

        assert(buf8 == buf16);
        buf8.append(0);
        buf16.append(0);
        assert(buf8 == buf16);

        buf16.append(0);
        assert(buf8 != buf16);
    }

    static void test_set() noexcept
    {
        Buffer8 buf8;
        Buffer8 buf16;

        uint8_t buffer[8];

        for (uint8_t i = 0; i < 8; i++) {
            buffer[i] = i;
        }
        MemArea mem_area_null;
        MemConstArea mem_const_area_null;
        MemArea mem_area = as_mem_area(buffer, 8);
        MemConstArea mem_const_area = as_mem_const_area(buffer, 8);

        // raw data
        assert(buf8.set(buffer, 0));
        assert(buf16.set(buffer, 0));
        assert(buf8 == buf16);

        assert(buf8.set(buffer, buf8.capacity() - 1));
        assert(buf16.set(buffer, buf8.capacity() - 1));
        assert(buf8 == buf16);

        assert(buf8.set(buffer, buf8.capacity()));
        assert(buf16.set(buffer, buf8.capacity()));
        assert(buf8 == buf16);

        // mem areas
        assert(not buf8.set(mem_area_null));
        assert(not buf16.set(mem_const_area_null));

        assert(buf8.set(mem_area));
        assert(buf16.set(mem_area));
        assert(buf8 == buf16);

        assert(buf8.set(mem_const_area));
        assert(buf16.set(mem_const_area));
        assert(buf8 == buf16);

        // static buffers
        assert(buf8.set(buf16));
        assert(buf8 == buf16);
    }

    static void test_append() noexcept
    {
        Buffer8 buf8;
        Buffer8 buf16;

        uint8_t buffer[4];

        for (uint8_t i = 0; i < 4; i++) {
            buffer[i] = i;
        }
        MemArea mem_area_null;
        MemConstArea mem_const_area_null;
        MemArea mem_area = as_mem_area(buffer, 4);
        MemConstArea mem_const_area = as_mem_const_area(buffer, 4);

        // raw data
        assert(buf8.append(buffer, 0));
        assert(buf16.append(buffer, 0));
        assert(buf8 == buf16);

        assert(buf8.append(buffer, 4));
        assert(buf8.append(buffer, 4));
        assert(buf16.append(buffer, 4));
        assert(buf16.append(buffer, 4));
        assert(buf8 == buf16);

        buf8.clear();
        buf16.clear();

        // mem areas
        assert(not buf8.append(mem_area_null));
        assert(not buf16.append(mem_const_area_null));

        assert(buf8.append(mem_area));
        assert(buf8.append(mem_area));
        assert(buf16.append(mem_area));
        assert(buf16.append(mem_area));
        assert(buf8 == buf16);
        buf8.clear();
        buf16.clear();

        assert(buf8.append(mem_const_area));
        assert(buf8.append(mem_const_area));
        assert(buf16.append(mem_const_area));
        assert(buf16.append(mem_const_area));
        assert(buf8 == buf16);
        buf8.clear();
        buf16.clear();

        // static buffers
		assert(buf16.append(mem_const_area));
        assert(buf8.append(buf16));
		assert(buf8.append(buf16));
		assert(buf16.append(mem_const_area));
        assert(buf8 == buf16);
    }

public:

    static void test()
    {
        test_init();
        test_comparison();
        test_set();
        test_append();
    }

};

}; // namespace binio

#endif /* BINIO_TESTS_STATIC_MEM_BUFFER_H */