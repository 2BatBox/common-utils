#pragma once

#include <cstdio>
#include <arpa/inet.h>

#include "procotols/Ethernet.h"
#include "procotols/Vlan.h"
#include "procotols/IPv4.h"
#include "procotols/IPv6.h"
#include "procotols/Tcp.h"
#include "procotols/Udp.h"
#include "procotols/Gre.h"

namespace proto {

class Dumper {
public:

	template <typename MFrame>
	static void mframe(FILE* out, const MFrame& mf) noexcept {
		fprintf(out, "| %zu : %zu : %zu |\n", mf.offset(), mf.available(), mf.padding());
	}

	//
	// Header section
	//

	// ethernet
	static void header(FILE* out, const Ethernet::Header* hdr) noexcept {
		fprintf(out, "[ETH]\n");
		uint16_t next_proto = ntohs(hdr->h_proto);
		fprintf(out, "  |-Source      : ");
		print_mac(out, hdr->h_source);
		fprintf(out, "\n");
		fprintf(out, "  |-Destination : ");
		print_mac(out, hdr->h_dest);
		fprintf(out, "\n");
		fprintf(out, "  |-Protocol    : 0x%04X\n", next_proto);
	}

	static void header_line(FILE* out, const Ethernet::Header* hdr) noexcept {
		fprintf(out, "[ETH]");
		print_mac(out, hdr->h_source);
		fprintf(out, "->");
		print_mac(out, hdr->h_dest);
		fprintf(out, "  ");
	}

	// vlan
	static void header(FILE* out, const Vlan::Header* hdr) noexcept {
		Vlan::Header lhdr;
		lhdr.vlan_tci = ntohs(hdr->vlan_tci);
		uint16_t next_proto = ntohs(hdr->nextProto);

		fprintf(out, "[VLAN]\n");
		fprintf(out, "  |-VID      : 0x%03x\n", lhdr.tci_detailed.vid);
		fprintf(out, "  |-DEI      : %u\n", lhdr.tci_detailed.dei);
		fprintf(out, "  |-PCP      : %u\n", lhdr.tci_detailed.pcp);
		fprintf(out, "  |-Protocol : 0x%04X\n", next_proto);
	}

	static void header_line(FILE* out, const Vlan::Header* hdr) noexcept {
		Vlan::Header lhdr;
		lhdr.vlan_tci = ntohs(hdr->vlan_tci);
		fprintf(out, "[VLAN]");
		fprintf(out, "%u(0x%03x)", lhdr.tci_detailed.vid, lhdr.tci_detailed.vid);
		fprintf(out, "  ");
	}

	// ipv4
	static void header(FILE* out, const IPv4::Header* hdr) noexcept {
		u_int16_t packet_nb = ntohs(hdr->tot_len);
		u_int16_t header_nb = hdr->ihl << 2;
		uint16_t frag = ntohs(hdr->frag_off);
		uint32_t src = ntohl(hdr->saddr);
		uint32_t dst = ntohl(hdr->daddr);

		fprintf(out, "[IPv4]\n");
		fprintf(out, "  |-Source      : ");
		print_ip(out, src);
		fprintf(out, "\n");
		fprintf(out, "  |-Destination : ");
		print_ip(out, dst);
		fprintf(out, "\n");

		fprintf(out, "  |-Length      : %u\n", packet_nb);
		fprintf(out, "  |-Hdr Length  : %u\n", header_nb);
		fprintf(out, "  |-ID          : %u\n", ntohs(hdr->id));
		fprintf(out, "  |-Offset      : %u\n", IPv4::offset(hdr));

		fprintf(out, "  |-Flags       :%s%s%s\n",
		        ((frag & IP_RF) ? " IP_RF" : ""),
		        ((frag & IP_DF) ? " IP_DF" : ""),
		        ((frag & IP_MF) ? " IP_MF" : "")
		);

		fprintf(out, "  |-Protocol    : %u\n", hdr->protocol);
		fprintf(out, "  |-Checksum    : 0x%04X\n", ntohs(hdr->check));
	}

	static void header_line(FILE* out, const IPv4::Header* hdr) noexcept {
		uint32_t src = ntohl(hdr->saddr);
		uint32_t dst = ntohl(hdr->daddr);

		fprintf(out, "[IPv4]");
		print_ip(out, src);
		fprintf(out, "->");
		print_ip(out, dst);
		fprintf(out, "  ");
	}

	// ipv6
	static void header(FILE* out, const IPv6::Header* hdr) noexcept {
		fprintf(out, "[IPv6]\n");
		fprintf(out, "  |-Protocol    : %u\n", hdr->next_header);
		fprintf(out, "  |-Payload     : %u\n", ntohs(hdr->payload_len));
		fprintf(out, "  |-Hop limit   : %u\n", hdr->hop_limit);
		fprintf(out, "  |-Source      : ");
		print_ip(out, hdr->src);
		fprintf(out, "\n");
		fprintf(out, "  |-Destination : ");
		print_ip(out, hdr->dst);
		fprintf(out, "\n");
	}

	static void header_line(FILE* out, const IPv6::Header* hdr) noexcept {
		fprintf(out, "[IPv6]");
		print_ip(out, hdr->src);
		fprintf(out, "->");
		print_ip(out, hdr->dst);
		fprintf(out, "  ");
	}

	// tcp
	static void header(FILE* out, const Tcp::Header* hdr) noexcept {
		uint16_t hdr_len = Tcp::hdr_len(hdr);
		fprintf(out, "[TCP]\n");
		fprintf(out, "  |-Source      : %u\n", ntohs(hdr->src));
		fprintf(out, "  |-Destination : %u\n", ntohs(hdr->dst));
		fprintf(out, "  |-Seq. number : %u\n", ntohl(hdr->seq_num));
		fprintf(out, "  |-ACK number  : %u\n", ntohl(hdr->ack_num));
		fprintf(out, "  |-Header len  : %u\n", hdr_len);
		fprintf(out, "  |-Flags       : (0x%02x) ", hdr->flags);
		fprintf(out, "%s", hdr->flag_ns ? "NS " : "");
		fprintf(out, "%s", hdr->flag_cwr ? "CWR " : "");
		fprintf(out, "%s", hdr->flag_ece ? "ECE " : "");
		fprintf(out, "%s", hdr->flag_urg ? "URG " : "");
		fprintf(out, "%s", hdr->flag_ack ? "ACK " : "");
		fprintf(out, "%s", hdr->flag_psh ? "PSH " : "");
		fprintf(out, "%s", hdr->flag_rst ? "RST " : "");
		fprintf(out, "%s", hdr->flag_syn ? "SYN " : "");
		fprintf(out, "%s", hdr->flag_fin ? "FIN " : "");
		fprintf(out, "\n");
		fprintf(out, "  |-Window size : %u\n", ntohs(hdr->win_size));
		fprintf(out, "  |-CRC         : 0x%04x\n", ntohs(hdr->crc));
		if(hdr->flag_urg) {
			fprintf(out, "  |-Urgent ptr. : %u\n", ntohs(hdr->urgent_pointer));
		}
	}

	static void header_line(FILE* out, const Tcp::Header* hdr) noexcept {
		fprintf(out, "[TCP]");
		fprintf(out, "%u->%u  ", ntohs(hdr->src), ntohs(hdr->dst));
	}

	// udp
	static void header(FILE* out, const Udp::Header* hdr) noexcept {
		uint16_t udp_nb = ntohs(hdr->len);
		fprintf(out, "[UDP]\n");
		fprintf(out, "  |-Source      : %u\n", ntohs(hdr->source));
		fprintf(out, "  |-Destination : %u\n", ntohs(hdr->dest));
		fprintf(out, "  |-Length      : %u\n", udp_nb);
		fprintf(out, "  |-Checksum    : 0x%04X\n", ntohs(hdr->check));
	}

	static void header_line(FILE* out, const Udp::Header* hdr) noexcept {
		fprintf(out, "[UDP]");
		fprintf(out, "%u->%u  ", ntohs(hdr->source), ntohs(hdr->dest));
	}

	// gre
	static void header(FILE* out, const Gre::Header* hdr) noexcept {
		fprintf(out, "    |-Bit Checksum        : %u\n", hdr->flag_bits.bit_checksum);
		fprintf(out, "    |-Bit Routung         : %u\n", hdr->flag_bits.bit_routung);
		fprintf(out, "    |-Bit Key             : %u\n", hdr->flag_bits.bit_key);
		fprintf(out, "    |-Bit Sequence Number : %u\n", hdr->flag_bits.bit_seq_num);
		fprintf(out, "    |-Bit SSR             : %u\n", hdr->flag_bits.bit_ssr);
		fprintf(out, "    |-Version             : %u\n", hdr->flag_bits.version);
		fprintf(out, "    |-Next Protocol       : 0x%04X\n", ntohs(hdr->next_proto));
	}

	static void header_short(FILE* out, const Gre::Header* hdr) noexcept {
		fprintf(out, "[GRE]  ");
	}

	static void print_mac(FILE* out, const unsigned char* mac) {
		fprintf(out, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	}

	static inline void print_ip(FILE* file, IPv4::Addr ip) {
		fprintf(file, "%01u.%01u.%01u.%01u", ((ip >> 24) & 0xFF), ((ip >> 16) & 0xFF), ((ip >> 8) & 0xFF),
		        ((ip) & 0xFF));
	}

	static inline void print_ip(FILE* out, IPv6::Addr ip) {
		bool zeros = false;
		for(unsigned i = 0; i < 8; ++i) {
			if(ip.addr16[i]){
				if(i > 0){
					fprintf(out, ":");
				}
				if(zeros){
					fprintf(out, ":");
				}
				fprintf(out, "%x", ntohs(ip.addr16[i]));
				zeros = false;
			} else {
				zeros = true;
			}


		}
	}

};

}; // namespace proto

