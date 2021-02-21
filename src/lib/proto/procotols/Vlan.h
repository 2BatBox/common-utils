#pragma once

#include <arpa/inet.h>
#include <linux/if_ether.h>

#include "../proto.h"

namespace proto {

// see Ethernet.h for more details

class Vlan {
public:

	struct Header {

		union {
			uint16_t vlan_tci;

			struct {
				uint16_t vid : 12;
				uint16_t dei : 1;
				uint16_t pcp : 3;
			} tci_detailed;
		};
		uint16_t nextProto;
	} __attribute__ ((__packed__));

	template <typename MFrame>
	static inline bool validate_packet(const MFrame& pkt) noexcept {
		return pkt.available(sizeof(Header));
	}

	template <typename MFrame>
	static inline bool validate_header(const MFrame& pkt) noexcept {
		return pkt.available(sizeof(Header));
	}

	template <typename MFrame>
	inline static Protocol next(MFrame& pkt) noexcept {
		const Header* hdr;
		pkt.assign(hdr);
		Protocol result = Protocol::END;

		switch(ntohs(hdr->nextProto)) {
			case ETH_P_IP:
				result = Protocol::L3_IPv4;
				break;
			case ETH_P_IPV6:
				result = Protocol::L3_IPv6;
				break;
			case ETH_P_8021Q:
				result = Protocol::L2_VLAN;
				break;
			default:
				break;
		}

		return result;
	}

	template <typename MFrame>
	static inline constexpr unsigned length_header(const MFrame&) noexcept {
		return sizeof(Header);
	}

	template <typename MFrame>
	static inline unsigned length_payload(const MFrame& pkt) noexcept {
		return pkt.available() - sizeof(Header);
	}

};

}; // namespace proto

