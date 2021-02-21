#pragma once

#include <linux/if_ether.h>
#include <arpa/inet.h>

#include "../proto.h"

namespace proto {

class Ethernet {
public:

	using Header = ethhdr;

	/**
	 * @param pkt
	 * @return true if 'pkt' has a valid Ethernet protocol packet
	 */
	template <typename MFrame>
	static inline bool validate_packet(const MFrame& pkt) noexcept {
		// Ethernet IEEE 802.3 standard defines the minimum Ethernet frame size_addr as 64 bytes.
		// However, it's possible to get an Ethernet frame with size less than 64 bytes from a PCAP dump for example.
		// return pkt.available_addr(64); // Should be used in case of following the standard accurately.
		return pkt.available(sizeof(Header));
	}

	/**
	 * @param pkt
	 * @return true if 'pkt' has a valid Ethernet protocol header
	 */
	template <typename MFrame>
	static inline bool validate_header(const MFrame& pkt) noexcept {
		return pkt.available(sizeof(Header));
	}

	/**
	 * The method MUST NOT be called for the certain packet without validating it with
	 * validate_packet() and/or validate_header().
	 * Skip the header of the protocol and return the next protocol identifier.
	 * @param pkt
	 * @return the next protocol identifier or Protocol::END in case of unknown payload type.
	 */
	template <typename MFrame>
	inline static Protocol next(MFrame& pkt) noexcept {
		const Header* hdr;
		pkt.assign(hdr);
		Protocol result = Protocol::END;

		switch(ntohs(hdr->h_proto)) {
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

