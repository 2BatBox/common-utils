#ifndef NET_PROTOCOLS_IPV4_H
#define NET_PROTOCOLS_IPV4_H

#include <cstdlib>
#include <netinet/ip.h>

#include "../net.h"

namespace net {

class IPv4 {
public:

	using Header = iphdr;

	static constexpr u_int16_t FRAG_MASK = 0x3FFF;
	static constexpr u_int16_t FLAG_MASK_DF = ~IP_DF;

	static constexpr u_int8_t PROTO_UDP = 17;
	static constexpr u_int8_t PROTO_SCTP = 132;

	static inline bool validate_packet(Packet& pkt) noexcept {
		const Header* hdr;
		pkt.assign(hdr);
		u_int16_t header_nb = hdr_len(hdr);
		u_int16_t packet_nb = pkt_len(hdr);
		unsigned available = pkt.available();

		if (
			available < sizeof (Header)
			|| packet_nb < header_nb
			|| available < packet_nb
			|| hdr->version != 4/*IP_V4*/
			|| flag_rf_get(hdr)
			) {
			return false;
		}

		// adjust padding if necessary
		pkt.tail_move_back(available - packet_nb);
		return true;
	}

	static inline bool validate_header(const Packet& pkt) noexcept {
		const Header* hdr;
		pkt.assign(hdr);
		u_int16_t header_nb = hdr_len(hdr);
		u_int16_t packet_nb = pkt_len(hdr);
		unsigned available = pkt.available();

		if (
			available < sizeof (Header)
			|| packet_nb < header_nb
			|| available < packet_nb
			|| hdr->version != 4/*IP_V4*/
			|| flag_rf_get(hdr)
			) {
			return false;
		}
		return true;
	}

	static Protocol next(Packet& pkt) noexcept {
		const Header* hdr;
		pkt.assign(hdr);
		u_int16_t header_nb = hdr_len(hdr);

		Protocol result = Protocol::END;
		switch (hdr->protocol) {
		case PROTO_UDP:
			result = Protocol::L4_UDP;
			break;
		case PROTO_SCTP:
			result = Protocol::L4_SCTP;
			break;
		}

		if (fragmented(hdr)) {
			result = Protocol::END;
		}

		pkt.head_move(header_nb);
		return result;
	}

	static inline unsigned length_header(const Packet& pkt) noexcept {
		const Header* hdr;
		pkt.assign(hdr);
		return hdr_len(hdr);
	}

	static inline unsigned length_payload(const Packet& pkt) noexcept {
		const Header* hdr;
		pkt.assign(hdr);
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

	static inline uint16_t update_check_summ(Header* hdr) noexcept {
		u_int16_t len_header = hdr->ihl << 2;
		hdr->check = 0;
		uint16_t check_summ = calc_checksum(hdr, len_header);
		hdr->check = htons(check_summ);
		return check_summ;
	}

private:

	static uint16_t calc_checksum(void* vdata, size_t length) noexcept {
		// Cast the data pointer to one that can be indexed.
		char* data = (char*)vdata;

		// Initialise the accumulator.
		uint32_t acc = 0xffff;

		// Handle complete 16-bit blocks.
		for (size_t i = 0; i + 1 < length; i += 2) {
			uint16_t word;
			memcpy(&word, data + i, 2);
			acc += ntohs(word);
			if (acc > 0xffff) {
				acc -= 0xffff;
			}
		}

		// Handle any partial block at the end of the data.
		if (length & 1) {
			uint16_t word = 0;
			memcpy(&word, data + length - 1, 1);
			acc += ntohs(word);
			if (acc > 0xffff) {
				acc -= 0xffff;
			}
		}

		// Return the checksum in network byte order.
		return ~acc;
	}

};

}; // namespace net

#endif /* NET_PROTOCOLS_IPV4_H */

