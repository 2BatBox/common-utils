#pragma once

#include <arpa/inet.h>
#include <netinet/udp.h>

#include "../proto.h"

namespace proto {

// see Ethernet.h for more details

class Udp {
public:

	using Header = udphdr;

	template <typename MFrame>
	static inline bool validate_packet(MFrame& pkt) noexcept {
		const Header* hdr;
		pkt.assign_stay(hdr);
		uint16_t packet_nb = ntohs(hdr->len);
		unsigned available = pkt.available();

		if(
			size_t(packet_nb) < sizeof(Header)
			|| available < packet_nb
			) {
			return false;
		}

		pkt.tail_move_back(available - packet_nb);
		return true;
	}

	template <typename MFrame>
	static inline bool validate_header(const MFrame& pkt) noexcept {
		return pkt.available(sizeof(Header));
	}

	template <typename MFrame>
	inline static Protocol next(MFrame& pkt) noexcept {
		pkt.head_move(sizeof(Header));
		return Protocol::END;
	}

	template <typename MFrame>
	static inline constexpr unsigned length_header(const MFrame&) noexcept {
		return sizeof(Header);
	}

	template <typename MFrame>
	static inline unsigned length_payload(const MFrame& pkt) noexcept {
		const Header* hdr;
		pkt.assign_stay(hdr);
		return ntohs(hdr->len) - sizeof(Header);
	}

};

}; // namespace proto

