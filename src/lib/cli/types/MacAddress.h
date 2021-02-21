#pragma once

#include <cstdlib>
#include <cstdint>
#include <cstdio>
#include <cctype>

namespace cli {

struct MacAddress {
	static constexpr size_t ADDR_SIZE = 6;
	using ADDR_TYPE = uint8_t;

	ADDR_TYPE addr[ADDR_SIZE];

	MacAddress() noexcept {
		clear();
	}

	bool operator==(const MacAddress& rv) const noexcept {
		for(size_t i = 0; i < ADDR_SIZE; ++i) {
			if(addr[i] != rv.addr[i]){
				return false;
			}
		}
		return true;
	}

	bool operator!=(const MacAddress& rv) const noexcept {
		return not operator==(rv);
	}

	void clear() noexcept {
		for(size_t i = 0; i < ADDR_SIZE; ++i) {
			addr[i] = 0;
		}
	}

	bool empty() const noexcept {
		size_t sum = 0;
		for(size_t i = 0; i < ADDR_SIZE; ++i) {
			sum += addr[i];
		}
		return sum == 0;
	}

	void print(FILE* out = stdout) const noexcept {
		for(size_t i = 0; i < ADDR_SIZE; ++i) {
			if(i){
				fprintf(out, ".");
			}
			fprintf(out, "%02X", addr[i]);
		}
	}

	ssize_t parse(const char* str) noexcept {
		size_t offset = 0;
		for(size_t i = 0; i < ADDR_SIZE; ++i) {
			uint64_t acc = 0;
			size_t read = read_hex_byte(str + offset, acc);
			if(read == 0){
				return -1;
			}
			offset += read;
			addr[i] = acc;
			if(str[offset] == '.'){
				offset++;
			}
		}
		return offset;
	}

private:

	static size_t read_hex(const char* str, uint64_t& acc) noexcept {
		int ch = tolower(str[0]);
		if(isxdigit(ch)){
			if(ch >= '0' && ch <= '9'){
				acc <<= 4;
				acc |= (ch - '0');
				return 1;
			}
			if(ch >= 'a' && ch <= 'f'){
				acc <<= 4;
				acc |= ch - 'a' + 10;
				return 1;
			}
		}
		return 0;
	}

	static size_t read_hex_byte(const char* str, uint64_t& acc) noexcept {
		size_t offset = read_hex(str, acc);
		return offset + read_hex(str + offset, acc);
	}

};

}; // namespace cli
