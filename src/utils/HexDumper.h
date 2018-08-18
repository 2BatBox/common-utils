#ifndef UTILS_HEX_DUMPER_H
#define UTILS_HEX_DUMPER_H

#include <cstdint>
#include <cstdio>
#include <ctype.h>

namespace utils {

class HexDumper {
	static const unsigned HEX_ASCII_DUMP_WIDTH = 16;
	static const unsigned HEX_ASCII_DUMP_SPLITTER_WIDTH = 8;

public:

	template <typename T>
	static void memory(FILE* out, const T* ptr, unsigned size) {
		const uint8_t* data = reinterpret_cast<const uint8_t*>(ptr);
		fprintf(out, "---- %p ---- %u bytes\n", data, size);
		unsigned offset = 0;
		while (offset < size) {
			fprintf(out, "%p ", data + offset);
			offset += print_hex_ascii(out, data + offset, size - offset);
		}
	}

	template <typename T>
	static void hex(FILE* out, const T* ptr, unsigned size) {
		const uint8_t* data = reinterpret_cast<const uint8_t*>(ptr);
		for (unsigned i = 0; i < size; i++) {
			fprintf(out, "%02x", data[i]);
		}
		fprintf(out, "\n");
	}

	template <typename T>
	static void hex_ascii(FILE* out, const T* ptr, unsigned size) {
		const uint8_t* data = reinterpret_cast<const uint8_t*>(ptr);
		fprintf(out, "\n");
		unsigned offset = 0;
		while (offset < size) {
			offset += print_hex_ascii(out, data + offset, size - offset);
		}
	}

private:

	static int print_hex_ascii(FILE* out, const uint8_t* mem, unsigned size) {
		unsigned width = (size > HEX_ASCII_DUMP_WIDTH) ? HEX_ASCII_DUMP_WIDTH : size;
		for (unsigned i = 0; i < HEX_ASCII_DUMP_WIDTH; i++) {
			if (i % HEX_ASCII_DUMP_SPLITTER_WIDTH == 0 && i > 0)
				fprintf(out, " ");
			if (i < width) {
				fprintf(out, "%02x ", mem[i]);
			} else {
				fprintf(out, "   ");
			}
		}
		fprintf(out, " |");

		for (unsigned i = 0; i < width; i++) {
			char ch = mem[i];
			if (i > 0 && i % HEX_ASCII_DUMP_SPLITTER_WIDTH == 0)
				fprintf(out, " ");
			if (isprint(ch))
				fprintf(out, "%c", ch);
			else
				fprintf(out, ".");
		}

		fprintf(out, "|\n");
		return width;
	}

};

}; // namespace utils

#endif /* UTILS_HEX_DUMPER_H */

