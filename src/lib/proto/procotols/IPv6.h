#pragma once

#include <cstdlib>
#include <cstring>

#include "../proto.h"

namespace proto {

// see Ethernet.h for more details

class IPv6 {
public:

	struct Addr {
		union {
			uint8_t addr8[16];
			uint16_t addr16[8];
			uint32_t addr32[4];
			uint64_t addr64[2];
		};
	} __attribute__ ((__packed__));

	struct Header {
		union {
			struct {
#if __BYTE_ORDER == __LITTLE_ENDIAN
				uint8_t traffic_class0 : 4;
				uint8_t version : 4;
#elif __BYTE_ORDER == __BIG_ENDIAN
				uint8_t version : 4;
				uint8_t traffic_class0 : 4;
#else
# error	"qlib::proto::IPv6"
#endif
			};
			uint8_t vtc;
		};
		union {
			struct {
#if __BYTE_ORDER == __LITTLE_ENDIAN
				uint8_t flow_label0 : 4;
				uint8_t traffic_class1 : 4;
#elif __BYTE_ORDER == __BIG_ENDIAN
				uint8_t traffic_class1 : 4;
				uint8_t flow_label0 : 4;
#else
# error	"qlib::proto::IPv6"
#endif
			};
			uint8_t tcfl;
		};
		uint16_t flow_label1;
		uint16_t payload_len;
		uint8_t next_header;
		uint8_t hop_limit;
		Addr src;
		Addr dst;

	} __attribute__ ((__packed__));


	static constexpr uint8_t PROTO_TCP = 6;
	static constexpr uint8_t PROTO_UDP = 17;
	static constexpr uint8_t PROTO_GRE = 47;
	static constexpr uint8_t PROTO_SCTP = 132;

	template<typename MFrame>
	static inline bool validate_packet(MFrame& frame) noexcept {
		const unsigned available = frame.available();
		if(available >= sizeof(Header)) {
			const Header* hdr;
			frame.assign_stay(hdr);
			const auto pkt_size = ntohs(hdr->payload_len) + sizeof(Header);
			if(hdr->version == 6/*IP_V4*/ && available >= pkt_size) {

				// adjust padding if necessary
				frame.tail_move_back(available - pkt_size);
				return true;
			}
		}
		return false;
	}

	template<typename MFrame>
	static inline bool validate_header(const MFrame& pkt) noexcept {
		if(pkt.available(sizeof(Header))) {
			const Header* hdr;
			pkt.assign_stay(hdr);
			return hdr->version == 6/*IP_V6*/;
		}
		return false;
	}

	template<typename MFrame>
	inline static Protocol next(MFrame& pkt) noexcept {
		const Header* hdr;
		pkt.assign_stay(hdr);

		Protocol result = Protocol::END;
		switch(hdr->next_header) {
			case PROTO_TCP:
				result = Protocol::L4_TCP;
				break;
			case PROTO_UDP:
				result = Protocol::L4_UDP;
				break;
			case PROTO_GRE:
				result = Protocol::L4_GRE;
				break;

		}
		pkt.head_move(sizeof(Header));
		return result;
	}

	template<typename MFrame>
	static inline constexpr unsigned length_header(const MFrame&) noexcept {
		return sizeof(Header);
	}

	template<typename MFrame>
	static inline unsigned length_payload(const MFrame& pkt) noexcept {
		const Header* hdr;
		pkt.assign_stay(hdr);
		return ntohs(hdr->payload_len);
	}

};

}; // namespace proto

