#ifndef STACK_IP_PROTOCOLS_ETHERNET_H
#define STACK_IP_PROTOCOLS_ETHERNET_H

#include <linux/if_ether.h>
#include <arpa/inet.h>

#include "../stack_ip.h"

namespace stack_ip {

class Ethernet {
public:

	using Header = ethhdr;

	/**
	 * @param pkt
	 * @return true if 'pkt' paints to a valid Ethernet protocol packet
	 */
	static inline bool validate_packet(const DefaultPacketReader& pkt) noexcept {
		return pkt.available(64);
	}

	/**
	 * @param pkt
	 * @return true if 'pkt' paints to a valid Ethernet protocol header
	 */
	static inline bool validate_header(const DefaultPacketReader& pkt) noexcept {
		return pkt.available(sizeof (Header));
	}

	/**
	 * The method MUST NOT be called for the certain packet without validating it with
	 * validate_packet() and/or validate_header().
	 * Skip the header of the protocol and return the next protocol identifier.
	 * @param pkt
	 * @return the next protocol identifier or Protocol::END in case of error.
	 */
	static Protocol next(DefaultPacketReader& pkt) noexcept {
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

		return result;
	}

	static inline constexpr unsigned length_header(const DefaultPacketReader&) noexcept {
		return sizeof (Header);
	}

	static inline unsigned length_payload(const DefaultPacketReader& pkt) noexcept {
		return pkt.available() - sizeof (Header);
	}

};

}; // namespace stack_ip

#endif /* STACK_IP_PROTOCOLS_ETHERNET_H */

