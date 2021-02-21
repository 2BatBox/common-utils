#pragma once

#include <cstdlib>
#include <netinet/ip.h>
#include <cstring>

#include "../proto.h"

namespace proto {

// see Ethernet.h for more details

class IPv4 {
public:

	using Header = iphdr;
	using Addr = uint32_t;
	struct Net {
		Addr addr;
		Addr mask;
	};

	static constexpr uint16_t FRAG_MASK = 0x3FFF;
	static constexpr uint16_t FLAG_MASK_RF = uint16_t(~uint16_t(IP_RF));
	static constexpr uint16_t FLAG_MASK_MF = uint16_t(~uint16_t(IP_MF));
	static constexpr uint16_t FLAG_MASK_DF = uint16_t(~uint16_t(IP_DF));

	static constexpr uint8_t PROTO_TCP = 6;
	static constexpr uint8_t PROTO_UDP = 17;
	static constexpr uint8_t PROTO_GRE = 47;
	static constexpr uint8_t PROTO_SCTP = 132;

	template <typename MFrame>
	static inline bool validate_packet(MFrame& pkt) noexcept {
		const unsigned available = pkt.available();
		if(available >= sizeof(Header)) {
			const Header* hdr;
			pkt.assign_stay(hdr);
			u_int16_t header_nb = hdr_len(hdr);
			u_int16_t packet_nb = pkt_len(hdr);
			if(
				available >= header_nb
				&& available >= packet_nb
				&& hdr->version == 4/*IP_V4*/
				&& not flag_rf(hdr)
				) {
				// adjust padding if necessary
				pkt.tail_move_back(available - packet_nb);
				return true;
			}
		}
		return false;
	}

	template <typename MFrame>
	static inline bool validate_header(const MFrame& pkt) noexcept {
		if(pkt.available(sizeof(Header))) {
			const Header* hdr;
			pkt.assign_stay(hdr);
			return hdr->version == 4/*IP_V4*/;
		}
		return false;
	}

	template <typename MFrame>
	inline static Protocol next(MFrame& pkt) noexcept {
		const Header* hdr;
		pkt.assign_stay(hdr);
		uint16_t header_nb = hdr_len(hdr);

		Protocol result = Protocol::END;
		switch(hdr->protocol) {
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

		if(fragmented(hdr)) {
			result = Protocol::END;
		}

		pkt.head_move(header_nb);
		return result;
	}

	template <typename MFrame>
	static inline unsigned length_header(const MFrame& pkt) noexcept {
		const Header* hdr;
		pkt.assign_stay(hdr);
		return hdr_len(hdr);
	}

	template <typename MFrame>
	static inline unsigned length_payload(const MFrame& pkt) noexcept {
		const Header* hdr;
		pkt.assign_stay(hdr);
		return pkt_len(hdr) - hdr_len(hdr);
	}

	// header manipulation

	static inline uint16_t pkt_len(const Header* hdr) noexcept {
		return ntohs(hdr->tot_len);
	}

	static inline uint16_t hdr_len(const Header* hdr) noexcept {
		return uint16_t(hdr->ihl << 2u);
	}

	static inline uint16_t payload_len(const Header* hdr) noexcept {
		return ntohs(hdr->tot_len) - uint16_t(hdr->ihl << 2u);
	}

	static inline bool fragmented(const Header* hdr) noexcept {
		return (ntohs(hdr->frag_off) & FRAG_MASK) > 0;
	}

	static inline uint16_t offset(const Header* hdr) noexcept {
		return (uint16_t(ntohs(hdr->frag_off) & uint16_t(IP_OFFMASK)) << 3u);
	}

	// bit 0: Evil Bit. see rfc-3514
	inline static bool flag_rf(const Header* hdr) noexcept {
		return (ntohs(hdr->frag_off) & uint16_t(IP_RF)) > 0u;
	}

	inline static void flag_rf_set(Header* hdr) noexcept {
		uint16_t frag = ntohs(hdr->frag_off);
		frag |= IP_RF;
		hdr->frag_off = htons(frag);
	}

	inline static void flag_rf_rst(Header* hdr) noexcept {
		uint16_t frag = ntohs(hdr->frag_off);
		frag &= FLAG_MASK_RF;
		hdr->frag_off = htons(frag);
	}

	// bit 1: Don't Fragment (DF)
	inline static bool flag_df(const Header* hdr) noexcept {
		return (ntohs(hdr->frag_off) & uint16_t(IP_DF)) > 0u;
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

	// bit 2: More Fragments (MF)
	inline static bool flag_mf(const Header* hdr) noexcept {
		return (ntohs(hdr->frag_off) & uint16_t(IP_MF)) > 0u;
	}

	inline static void flag_mf_set(Header* hdr) noexcept {
		uint16_t frag = ntohs(hdr->frag_off);
		frag |= IP_MF;
		hdr->frag_off = htons(frag);
	}

	inline static void flag_mf_rst(Header* hdr) noexcept {
		uint16_t frag = ntohs(hdr->frag_off);
		frag &= FLAG_MASK_MF;
		hdr->frag_off = htons(frag);
	}

	static IPv4::Addr addr_host(unsigned b0, unsigned b1, unsigned b2, unsigned b3) noexcept {
		IPv4::Addr addr = 0;
		addr |= b0 & 0xFF;
		addr <<= 8;
		addr |= b1 & 0xFF;
		addr <<= 8;
		addr |= b2 & 0xFF;
		addr <<= 8;
		addr |= b3 & 0xFF;
		return addr;
	}

	inline static IPv4::Addr addr_net(unsigned b0, unsigned b1, unsigned b2, unsigned b3) noexcept {
		return htonl(addr_host(b0, b1, b2, b3));
	}

	static inline uint16_t update_checksum(Header* hdr) noexcept {
		const auto len_header = u_int16_t(hdr->ihl << 2u);
		hdr->check = 0;
		const auto check_summ = calc_checksum(hdr, len_header);
		hdr->check = htons(check_summ);
		return check_summ;
	}

private:

	static uint16_t calc_checksum(const void* vdata, size_t length) noexcept {
		// Cast the data pointer to one that can be indexed.
		auto data = (const char*) vdata;

		// Initialise the accumulator.
		uint32_t acc = 0xffff;

		// Handle complete 16-bit blocks.
		for(size_t i = 0; i + 1 < length; i += 2) {
			uint16_t word;
			memcpy(&word, data + i, 2);
			acc += ntohs(word);
			if(acc > 0xffff) {
				acc -= 0xffff;
			}
		}

		// Handle any partial block at the end of the data.
		if(length & 1) {
			uint16_t word = 0;
			memcpy(&word, data + length - 1, 1);
			acc += ntohs(word);
			if(acc > 0xffff) {
				acc -= 0xffff;
			}
		}

		// Return the checksum in addr byte order.
		return uint16_t(~acc);
	}

};

}; // namespace proto

