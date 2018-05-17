#ifndef NET_PROTOCOLS_UDP_H
#define NET_PROTOCOLS_UDP_H

#include <arpa/inet.h>
#include <netinet/udp.h>

#include "../net.h"

namespace net {

class Udp {
public:

	using Header = udphdr;

	static inline bool validate_packet(Packet& pkt) noexcept {
		const Header* hdr;
		pkt.assign(hdr);
		uint16_t packet_nb = ntohs(hdr->len);
		unsigned available = pkt.available();

		if (
			size_t(packet_nb) < sizeof (Header)
			|| available < packet_nb
			) {
			return false;
		}

		pkt.tail_move_back(available - packet_nb);
		return true;
	}

	static inline bool validate_header(const Packet& pkt) noexcept {
		return pkt.available(sizeof (Header));
	}

	static Protocol next(Packet& pkt) noexcept {
		pkt.head_move(sizeof(Header));
		return Protocol::END;
	}

	static inline constexpr unsigned length_header(const Packet&) noexcept {
		return sizeof (Header);
	}

	static inline unsigned length_payload(const Packet& pkt) noexcept {
		const Header* hdr;
		pkt.assign(hdr);
		return ntohs(hdr->len) - sizeof (Header);
	}

};

}; // namespace net

#endif /* NET_PROTOCOLS_UDP_H */

