#ifndef NET_PROTOCOLS_VLAN_H
#define NET_PROTOCOLS_VLAN_H

#include "../net.h"

namespace net {

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

	static inline bool validate_packet(const Packet& pkt) noexcept {
		return pkt.available(sizeof (Header));
	}

	static inline bool validate_header(const Packet& pkt) noexcept {
		return pkt.available(sizeof (Header));
	}

	static Protocol next(Packet& pkt) noexcept {
		const Header* hdr;
		pkt.assign(hdr);
		Protocol result = Protocol::END;

		switch (ntohs(hdr->nextProto)) {
		case ETH_P_IP:
			result = Protocol::L3_IPv4;
			break;
		case ETH_P_8021Q:
			result = Protocol::L2_VLAN;
			break;
		default:
			break;
		}

		pkt.head_move(sizeof (Header));
		return result;
	}

	static inline constexpr unsigned length_header(const Packet&) noexcept {
		return sizeof (Header);
	}

	static inline unsigned length_payload(const Packet& pkt) noexcept {
		return pkt.available() - sizeof (Header);
	}

};

}; // namespace net

#endif /* NET_PROTOCOLS_VLAN_H */

