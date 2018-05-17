#ifndef NET_PROTOCOLS_ETHERNET_H
#define NET_PROTOCOLS_ETHERNET_H

#include <linux/if_ether.h>
#include <arpa/inet.h>

#include "../net.h"
#include "../BasicPacket.h"

namespace net {

class Ethernet {
public:

	using Header = ethhdr;

	static inline bool validate_packet(const Packet& pkt) noexcept {
		return pkt.available(64);
	}

	static inline bool validate_header(const Packet& pkt) noexcept {
		return pkt.available(sizeof (Header));
	}

	static Protocol next(Packet& pkt) noexcept {
		const Header* hdr;
		pkt.assign(hdr);
		Protocol result = Protocol::END;

		switch (ntohs(hdr->h_proto)) {
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

#endif /* NET_PROTOCOLS_ETHERNET_H */

