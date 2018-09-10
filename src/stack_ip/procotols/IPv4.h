#ifndef STACK_IP_PROTOCOLS_IPV4_H
#define STACK_IP_PROTOCOLS_IPV4_H

#include <cstdlib>
#include <netinet/ip.h>

#include "../stack_ip.h"

namespace stack_ip {

class IPv4 {
public:

	using Header = iphdr;
	using Addr = uint32_t;

	static constexpr u_int16_t FRAG_MASK = 0x3FFF;
	static constexpr u_int16_t FLAG_MASK_DF = ~IP_DF;

	static constexpr u_int8_t PROTO_UDP = 17;
	static constexpr u_int8_t PROTO_GRE = 47;
	static constexpr u_int8_t PROTO_SCTP = 132;

	static inline bool validate_packet(DefaultPacketReader& pkt) noexcept {
		const unsigned available = pkt.available();
		if (available >= sizeof (Header)) {
			const Header* hdr;
			pkt.assign_stay(hdr);
			u_int16_t header_nb = hdr_len(hdr);
			u_int16_t packet_nb = pkt_len(hdr);
			if (
				available >= header_nb
				&& available >= packet_nb
				&& hdr->version == 4/*IP_V4*/
				&& not flag_rf_get(hdr)
				) {
				// adjust padding if necessary
				pkt.tail_move_back(available - packet_nb);
				return true;
			}
		}
		return false;
	}

	static inline bool validate_header(const DefaultPacketReader& pkt) noexcept {
		const unsigned available = pkt.available();
		if (available >= sizeof (Header)) {
			const Header* hdr;
			pkt.assign_stay(hdr);
			u_int16_t header_nb = hdr_len(hdr);
			return (
				available >= header_nb
				&& hdr->version == 4/*IP_V4*/
				&& not flag_rf_get(hdr)
				);
		}
		return false;
	}

	static Protocol next(DefaultPacketReader& pkt) noexcept {
		const Header* hdr;
		pkt.assign_stay(hdr);
		u_int16_t header_nb = hdr_len(hdr);

		Protocol result = Protocol::END;
		switch (hdr->protocol) {
		case PROTO_UDP:
			result = Protocol::L4_UDP;
			break;
		case PROTO_SCTP:
			result = Protocol::L4_SCTP;
			break;
		case PROTO_GRE:
			result = Protocol::L4_GRE;
			break;

		}

		if (fragmented(hdr)) {
			result = Protocol::END;
		}

		pkt.head_move(header_nb);
		return result;
	}

	static inline unsigned length_header(const DefaultPacketReader& pkt) noexcept {
		const Header* hdr;
		pkt.assign_stay(hdr);
		return hdr_len(hdr);
	}

	static inline unsigned length_payload(const DefaultPacketReader& pkt) noexcept {
		const Header* hdr;
		pkt.assign_stay(hdr);
		return pkt_len(hdr) - hdr_len(hdr);
	}

	static inline uint16_t pkt_len(const Header* hdr) noexcept {
		return ntohs(hdr->tot_len);
	}

	static inline uint16_t hdr_len(const Header* hdr) noexcept {
		return hdr->ihl << 2;
	}

	static inline bool fragmented(const Header* hdr) noexcept {
		return ntohs(hdr->frag_off) & FRAG_MASK;
	}

	inline static bool flag_rf_get(const Header* hdr) noexcept {
		return ntohs(hdr->frag_off) & IP_RF;
	}

	inline static bool flag_df_get(const Header* hdr) noexcept {
		return ntohs(hdr->frag_off) & IP_DF;
	}

	inline static bool flag_mf_get(const Header* hdr) noexcept {
		return ntohs(hdr->frag_off) & IP_MF;
	}

	inline static void flag_df_set(Header* hdr) noexcept {
		uint16_t frag = ntohs(hdr->frag_off);
		frag |= IP_DF;
		hdr->frag_off = htons(frag);
	}

	inline static void flag_df_rst(Header* hdr) noexcept {
		uint16_t frag = ntohs(hdr->frag_off);
		frag &= FLAG_MASK_DF;
		hdr->frag_off = htons(frag);
	}

};

}; // namespace stack_ip

#endif /* STACK_IP_PROTOCOLS_IPV4_H */

