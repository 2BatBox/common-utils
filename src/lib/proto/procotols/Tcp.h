#pragma once

#include <arpa/inet.h>
#include <netinet/udp.h>

#include "../proto.h"

namespace proto {

// see Ethernet.h for more details

class Tcp {
public:

	struct Header {
		uint16_t src;
		uint16_t dst;
		uint32_t seq_num;
		uint32_t ack_num;

		union {
			struct {
				uint8_t flag_ns : 1;
				uint8_t reserved : 3;
				uint8_t data_offset : 4;
			};
			uint8_t drf;
		};

		union {
			struct {
				uint8_t flag_fin : 1;
				uint8_t flag_syn : 1;
				uint8_t flag_rst : 1;
				uint8_t flag_psh : 1;
				uint8_t flag_ack : 1;
				uint8_t flag_urg : 1;
				uint8_t flag_ece : 1;
				uint8_t flag_cwr : 1;
			};
			uint8_t flags;
		};

		uint16_t win_size;
		uint16_t crc;
		uint16_t urgent_pointer;
	} __attribute__ ((__packed__));

	template<typename MFrame>
	static inline bool validate_packet(MFrame& pkt) noexcept {
		// TODO:
		return false;
	}

	template<typename MFrame>
	static inline bool validate_header(const MFrame& pkt) noexcept {
		if(pkt.available(sizeof(Header))){
			const Header* hdr;
			pkt.assign_stay(hdr);
			return pkt.available(hdr_len(hdr));
		}
		return false;
	}

	template<typename MFrame>
	inline static Protocol next(MFrame& pkt) noexcept {
		const Header* hdr;
		pkt.assign_stay(hdr);
		pkt.head_move(hdr_len(hdr));
		return Protocol::END;
	}

	template<typename MFrame>
	static inline constexpr unsigned length_header(const MFrame&) noexcept {
		return sizeof(Header);
	}

//	template<typename MFrame>
//	static inline unsigned length_payload(const MFrame& pkt) noexcept {
//		const Header* hdr;
//		pkt.assign_stay(hdr);
//		return ntohs(hdr->len) - sizeof(Header);
//	}

	// header manipulation
	static inline uint16_t hdr_len(const Header* hdr) noexcept {
		return uint16_t(hdr->data_offset << 2u);
	}

};

}; // namespace proto

