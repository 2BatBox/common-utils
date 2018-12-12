#ifndef UTILS_HEX_DUMPER_H
#define UTILS_HEX_DUMPER_H

#include "../binio/MArea.h"

#include <cstdint>
#include <cstdio>
#include <ctype.h>

namespace utils {

class HexDumper {
	static const unsigned HEX_ASCII_DUMP_WIDTH = 16;
	static const unsigned HEX_ASCII_DUMP_SPLITTER_WIDTH = 8;

public:

	static void memory(FILE* out, binio::MCArea area) {
		const uint8_t* data = reinterpret_cast<const uint8_t*>(area.cbegin());
		fprintf(out, "---- %p ---- %lu bytes\n", data, area.length());
		unsigned offset = 0;
		while(offset < area.length()) {
			fprintf(out, "%p ", data + offset);
			offset += print_hex_ascii(out, data + offset, area.length() - offset);
		}
	}

	static void hex(FILE* out, binio::MCArea area) {
		const uint8_t* data = reinterpret_cast<const uint8_t*>(area.cbegin());
		size_t size = area.length();
		for(unsigned i = 0; i < size; i++) {
			fprintf(out, "%02x", data[i]);
		}
		fprintf(out, "\n");
	}

	static void hex_ascii(FILE* out, binio::MCArea area) {
		const uint8_t* data = reinterpret_cast<const uint8_t*>(area.cbegin());
		size_t size = area.length();
		fprintf(out, "\n");
		unsigned offset = 0;
		while(offset < size) {
			offset += print_hex_ascii(out, data + offset, size - offset);
		}
	}

	// aliases for raw pointers

	template<typename T>
	static void memory(FILE* out, const T* ptr, size_t size) {
		memory(out, binio::as_const_area(ptr, size));
	}

	template<typename T>
	static void hex(FILE* out, const T* ptr, size_t size) {
		hex(out, binio::as_const_area(ptr, size));
	}

	template<typename T>
	static void hex_ascii(FILE* out, const T* ptr, size_t size) {
		hex_ascii(out, binio::as_const_area(ptr, size));
	}


private:

	static int print_hex_ascii(FILE* out, const uint8_t* mem, unsigned size) {
		unsigned width = (size > HEX_ASCII_DUMP_WIDTH) ? HEX_ASCII_DUMP_WIDTH : size;
		for(unsigned i = 0; i < HEX_ASCII_DUMP_WIDTH; i++) {
			if(i % HEX_ASCII_DUMP_SPLITTER_WIDTH == 0 && i > 0)
				fprintf(out, " ");
			if(i < width) {
				fprintf(out, "%02x ", mem[i]);
			} else {
				fprintf(out, "   ");
			}
		}
		fprintf(out, " |");

		for(unsigned i = 0; i < width; i++) {
			char ch = mem[i];
			if(i > 0 && i % HEX_ASCII_DUMP_SPLITTER_WIDTH == 0)
				fprintf(out, " ");
			if(isprint(ch))
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

