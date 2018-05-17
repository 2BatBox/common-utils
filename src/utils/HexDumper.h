#ifndef UTILS_HEX_DUMPER_H
#define UTILS_HEX_DUMPER_H

#include <cstdint>
#include <cstdio>
#include <ctype.h>

class HexDumper {
    static const unsigned HEX_ASCII_DUMP_WIDTH = 16;
    static const unsigned HEX_ASCII_DUMP_SPLITTER_WIDTH = 8;

public:

    static void memory(const void* mem, unsigned size, FILE* file = stdout)
    {
        fprintf(file, "---- %p ---- %u bytes\n", mem, size);
        const uint8_t* data = (const uint8_t*) mem;
        unsigned offset = 0;
        while (offset < size) {
            fprintf(file, "%p ", data + offset);
            offset += print_hex_ascii(data + offset, size - offset, file);
        }
    }

    static void hex(const void* mem, unsigned size, FILE* file = stdout)
    {
        const uint8_t* data = (const uint8_t*) mem;
        for (unsigned i = 0; i < size; i++) {
            fprintf(file, "%02x", data[i]);
        }
        fprintf(file, "\n");
    }

    static void hex_ascii(const void* mem, unsigned size, FILE* file = stdout)
    {
        fprintf(file, "\n");
        const uint8_t* data = (const uint8_t*) mem;
        unsigned offset = 0;
        while (offset < size) {
            offset += print_hex_ascii(data + offset, size - offset, file);
        }
    }

private:

    static int print_hex_ascii(const uint8_t* mem, unsigned size, FILE* file)
    {
        unsigned width = (size > HEX_ASCII_DUMP_WIDTH) ? HEX_ASCII_DUMP_WIDTH : size;
        for (unsigned i = 0; i < HEX_ASCII_DUMP_WIDTH; i++) {
            if (i % HEX_ASCII_DUMP_SPLITTER_WIDTH == 0 && i > 0)
                fprintf(file, " ");
            if (i < width) {
                fprintf(file, "%02x ", mem[i]);
            } else {
                fprintf(file, "   ");
            }
        }
        fprintf(file, " |");

        for (unsigned i = 0; i < width; i++) {
            char ch = mem[i];
            if (i > 0 && i % HEX_ASCII_DUMP_SPLITTER_WIDTH == 0)
                fprintf(file, " ");
            if (isprint(ch))
                fprintf(file, "%c", ch);
            else
                fprintf(file, ".");
        }

        fprintf(file, "|\n");
        return width;
    }

};

#endif /* UTILS_HEX_DUMPER_H */

