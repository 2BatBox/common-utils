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

	template <typename T>
	MacAddress(const T* ar, size_t ar_nb) noexcept {
		clear();
		const auto size = ADDR_SIZE < ar_nb ? ADDR_SIZE : ar_nb;
		for(size_t i = 0; i < size; ++i) {
			addr[i] = ar[i];
		}
	}

	bool operator<(const MacAddress& rv) const noexcept {
		for(size_t i = 0; i < ADDR_SIZE; ++i) {
			if(addr[i] < rv.addr[i]){
				return true;
			}
		}
		return false;
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
			if(str[offset] == '.' || str[offset] == ':'){
				offset++;
			}
		}
		return offset;
	}

	template <typename T>
	size_t copy_to(T* ar, size_t ar_nb) const noexcept {
		const auto size = ADDR_SIZE < ar_nb ? ADDR_SIZE : ar_nb;
		for(size_t i = 0; i < size; ++i) {
			ar[i] = addr[i];
		}
		return size;
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
