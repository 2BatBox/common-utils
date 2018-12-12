#ifndef STACKIP_IPV4UTILS_H
#define STACKIP_IPV4UTILS_H

#include "procotols/IPv4.h"

namespace stack_ip {

class IPv4Utils {
public:

	static inline uint16_t update_check_summ(IPv4::Header* hdr) noexcept {
		u_int16_t len_header = hdr->ihl << 2;
		hdr->check = 0;
		uint16_t check_summ = calc_checksum(hdr, len_header);
		hdr->check = htons(check_summ);
		return check_summ;
	}

	static IPv4::Addr addr_host(unsigned b0, unsigned b1, unsigned b2, unsigned b3) noexcept {
		IPv4::Addr addr = 0;
		addr |= b0 & 0xFF;
		addr <<= 8;
		addr |= b1 & 0xFF;
		addr <<= 8;
		addr |= b2 & 0xFF;
		addr <<= 8;
		addr |= b3 & 0xFF;
		return addr;
	}

	static IPv4::Addr addr_net(unsigned b0, unsigned b1, unsigned b2, unsigned b3) noexcept {
		return htonl(addr_host(b0, b1, b2, b3));
	}

private:

	static uint16_t calc_checksum(void* vdata, size_t length) noexcept {
		// Cast the data pointer to one that can be indexed.
		char* data = (char*) vdata;

		// Initialise the accumulator.
		uint32_t acc = 0xffff;

		// Handle complete 16-bit blocks.
		for(size_t i = 0; i + 1 < length; i += 2) {
			uint16_t word;
			memcpy(&word, data + i, 2);
			acc += ntohs(word);
			if(acc > 0xffff) {
				acc -= 0xffff;
			}
		}

		// Handle any partial block at the end of the data.
		if(length & 1) {
			uint16_t word = 0;
			memcpy(&word, data + length - 1, 1);
			acc += ntohs(word);
			if(acc > 0xffff) {
				acc -= 0xffff;
			}
		}

		// Return the checksum in network byte order.
		return ~acc;
	}

};

}; // namespace stack_ip

#endif /* STACKIP_IPV4UTILS_H */

