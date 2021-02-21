#pragma once

#include "../proto.h"

#include <arpa/inet.h>
#include <cstdint>

namespace proto {

// see Ethernet.h for more details

class Gre {
public:

	struct Header {

		union {
			uint16_t flags;

			struct {
				uint16_t version : 3;
				uint16_t reserved : 5;
				uint16_t bit_ssr : 1;
				uint16_t bit_seq_num : 1;
				uint16_t bit_key : 1;
				uint16_t bit_routung : 1;
				uint16_t bit_checksum : 1;
			} flag_bits;
		};
		uint16_t next_proto;

	} __attribute__ ((__packed__));

	template <typename MFrame>
	static inline bool validate_packet(const MFrame& pkt) noexcept {
		if(pkt.available(sizeof(Header))) {
			return pkt.available(length_header(pkt));
		}
		return false;
	}

	template <typename MFrame>
	static inline bool validate_header(const MFrame& pkt) noexcept {
		if(pkt.available(sizeof(Header))) {
			return pkt.available(length_header(pkt));
		}
		return false;
	}

	template <typename MFrame>
	inline static Protocol next(MFrame& pkt) noexcept {
		const Header* hdr;
		pkt.assign(hdr);
		Protocol result = Protocol::END;

		switch(ntohs(hdr->next_proto)) {
			case 0x6558: // Transparent Ethernet Bridging
				result = Protocol::L2_ETHERNET;
				break;
			default:
				break;
		}

		return result;
	}

	template <typename MFrame>
	static unsigned length_header(const MFrame& pkt) noexcept {
		const Header* hdr;
		pkt.assign_stay(hdr);
		size_t hdr_size = 0;
		if(hdr->flag_bits.version == 0) {
			if(hdr->flag_bits.bit_checksum) {
				hdr_size <<= 5;
			}
			if(hdr->flag_bits.bit_key) {
				hdr_size <<= 5;
			}
			if(hdr->flag_bits.bit_seq_num) {
				hdr_size <<= 5;
			}
			return sizeof(Header) + hdr_size;
		}
		return sizeof(Header);
	}

	template <typename MFrame>
	static inline unsigned length_payload(const MFrame& pkt) noexcept {
		return pkt.available() - length_header(pkt);
	}

};

}; // namespace proto

