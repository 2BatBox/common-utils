#ifndef STACK_IP_DUMPER_H
#define STACK_IP_DUMPER_H

#include <cstdio>
#include <arpa/inet.h>

#include "parsers/StackParser.h"
#include "parsers/SctpParser.h"
#include "../utils/HexDumper.h"

namespace stack_ip {

class Dumper {
	static constexpr bool DUMP_PAYLOAD_ETHERNET = false;
	static constexpr bool DUMP_PAYLOAD_VLAN = false;
	static constexpr bool DUMP_PAYLOAD_IPv4 = false;
	static constexpr bool DUMP_PAYLOAD_UDP = false;
	static constexpr bool DUMP_PAYLOAD_GRE = false;
	static constexpr bool DUMP_PAYLOAD_SCTP = false;

public:

	static void
	dump_stack(FILE* out, bool hex_dump, binio::MCArea data, Protocol proto = Protocol::L2_ETHERNET) noexcept {
		StackParser packet(data);
		if(packet.parse(proto)) {
			while(packet.protocol() != Protocol::END) {
				dump_proto(out, packet, hex_dump);
				packet.next();
			}
			if(packet.padding()) {
				fprintf(out, "    |-Padding [%zu bytes]\n", packet.padding());
				//		fprintf(out, "[ %u : %u : %u]\n", packet.offset(), packet.available(), packet.padding());
			}
		}
	}

	static void dump_proto(FILE* out, const StackParser& packet, bool hex_dump) {
		bool hex_dump_proto = false;

		switch(packet.protocol()) {

			case Protocol::L2_ETHERNET:
				hex_dump_proto = hex_dump & DUMP_PAYLOAD_ETHERNET;
				dump_ethernet(out, packet);
				break;

			case Protocol::L2_VLAN:
				hex_dump_proto = hex_dump & DUMP_PAYLOAD_VLAN;
				dump_vlan(out, packet);
				break;

			case Protocol::L3_IPv4:
				hex_dump_proto = hex_dump & DUMP_PAYLOAD_IPv4;
				dump_ipv4(out, packet);
				break;

			case Protocol::L4_UDP:
				hex_dump_proto = hex_dump & DUMP_PAYLOAD_UDP;
				dump_udp(out, packet);
				break;

			case Protocol::L4_GRE:
				hex_dump_proto = hex_dump & DUMP_PAYLOAD_GRE;
				dump_gre(out, packet);
				break;

			case Protocol::L4_SCTP:
				hex_dump_proto = hex_dump & DUMP_PAYLOAD_SCTP;
				dump_sctp(out, packet, hex_dump_proto);
				break;

			default:
				fprintf(out, "[UNKNOWN]\n");
				hex_dump = false;
				break;
		}

		if(hex_dump_proto) {
			binio::MCArea header = packet.header();
			binio::MCArea payload = packet.payload();

			if(header) {
				fprintf(out, "    |-Header [%zu] : ", header.length());
				utils::HexDumper::hex_ascii(out, header);
			}

			if(payload) {
				fprintf(out, "    |-Payload [%zu] : ", payload.length());
				utils::HexDumper::hex_ascii(out, payload);
			}
			printf("\n");
		}
	}

	static void dump_ethernet(FILE* out, const StackParser& pkt) noexcept {
		const Ethernet::Header* hdr;
		pkt.assign_stay(hdr);
		uint16_t next_proto = ntohs(hdr->h_proto);
		print_header(out, "Ethernet", pkt);
		fprintf(out, "    |-Source      : ");
		print_mac(out, hdr->h_source);
		fprintf(out, "\n");
		fprintf(out, "    |-Destination : ");
		print_mac(out, hdr->h_dest);
		fprintf(out, "\n");
		fprintf(out, "    |-Protocol    : 0x%04X\n", next_proto);
	}

	static void dump_vlan(FILE* out, const StackParser& pkt) noexcept {
		const Vlan::Header* hdr;
		pkt.assign_stay(hdr);
		uint16_t next_proto = ntohs(hdr->nextProto);
		print_header(out, "Vlan", pkt);
		fprintf(out, "    |-VID      : 0x%03x\n", hdr->tci_detailed.vid);
		fprintf(out, "    |-DEI      : %u\n", hdr->tci_detailed.dei);
		fprintf(out, "    |-PCP      : %u\n", hdr->tci_detailed.pcp);
		fprintf(out, "    |-Protocol : 0x%04X\n", next_proto);
	}

	static void dump_ipv4(FILE* out, const StackParser& pkt) noexcept {
		const IPv4::Header* hdr;
		pkt.assign_stay(hdr);
		u_int16_t packet_nb = ntohs(hdr->tot_len);
		u_int16_t header_nb = hdr->ihl << 2;
		uint16_t frag = ntohs(hdr->frag_off);
		uint32_t src = ntohl(hdr->saddr);
		uint32_t dst = ntohl(hdr->daddr);

		print_header(out, "IPv4", pkt);
		fprintf(out, "    |-Source      : ");
		print_ip(out, src);
		fprintf(out, "\n");
		fprintf(out, "    |-Destination : ");
		print_ip(out, dst);
		fprintf(out, "\n");

		fprintf(out, "    |-Length      : %u\n", packet_nb);
		fprintf(out, "    |-Hdr Length  : %u\n", header_nb);
		fprintf(out, "    |-ID          : %u\n", ntohs(hdr->id));
		fprintf(out, "    |-Offset      : %u\n", (frag & IP_OFFMASK) * 8);

		fprintf(out, "    |-Flags       :%s%s%s\n",
		        ((frag & IP_RF) ? " IP_RF" : ""),
		        ((frag & IP_DF) ? " IP_DF" : ""),
		        ((frag & IP_MF) ? " IP_MF" : "")
		);

		fprintf(out, "    |-Protocol    : %u\n", hdr->protocol);
		fprintf(out, "    |-Checksum    : 0x%04X\n", ntohs(hdr->check));
	}

	static void dump_udp(FILE* out, const StackParser& pkt) noexcept {
		const Udp::Header* hdr;
		pkt.assign_stay(hdr);
		uint16_t udp_nb = ntohs(hdr->len);
		print_header(out, "Udp", pkt);
		fprintf(out, "    |-Source      : %u\n", ntohs(hdr->source));
		fprintf(out, "    |-Destination : %u\n", ntohs(hdr->dest));
		fprintf(out, "    |-Length      : %u\n", udp_nb);
		fprintf(out, "    |-Checksum    : 0x%04X\n", ntohs(hdr->check));
	}

	static void dump_gre(FILE* out, const StackParser& pkt) noexcept {
		const Gre::Header* hdr;
		pkt.assign_stay(hdr);
		print_header(out, "Gre", pkt);
		fprintf(out, "    |-Bit Checksum        : %u\n", hdr->flag_bits.bit_checksum);
		fprintf(out, "    |-Bit Routung         : %u\n", hdr->flag_bits.bit_routung);
		fprintf(out, "    |-Bit Key             : %u\n", hdr->flag_bits.bit_key);
		fprintf(out, "    |-Bit Sequence Number : %u\n", hdr->flag_bits.bit_seq_num);
		fprintf(out, "    |-Bit SSR             : %u\n", hdr->flag_bits.bit_ssr);
		fprintf(out, "    |-Version             : %u\n", hdr->flag_bits.version);
		fprintf(out, "    |-Next Protocol       : 0x%04X\n", ntohs(hdr->next_proto));

	}

	static void dump_sctp(FILE* out, const StackParser& pkt, bool payload) noexcept {
		SctpParser sctp(pkt.packet());
		if(sctp.parse()) {
			const Sctp::Header* hdr = sctp.header_ptr();
			const Sctp::ChunkHeader* chunk = sctp.chunk_ptr();

			print_header(out, "Sctp", pkt);
			fprintf(out, "    |-Source      : %u\n", ntohs(hdr->port_src));
			fprintf(out, "    |-Destination : %u\n", ntohs(hdr->port_dst));
			fprintf(out, "    |-Ver.tag     : 0x%08X\n", ntohl(hdr->verefication_tag));
			fprintf(out, "    |-Checksum    : 0x%08X\n", ntohl(hdr->check_sum));

			while(chunk) {
				uint16_t length = ntohs(chunk->length);
				fprintf(out, "  [%s] ", Sctp::chunk_name(chunk->type));
				fprintf(out, "type=%u ", chunk->type);
				fprintf(out, "flags=0x%02x ", chunk->flags);
				fprintf(out, "length=%u\n", length);

				switch(chunk->type) {
					case Sctp::DATA:
						const Sctp::ChunkData* chunk_data;
						sctp.assign_stay(chunk_data);
						fprintf(out, "    |-Trns.number : %u\n", ntohl(chunk_data->transmission_sequence_number));
						fprintf(out, "    |-Stream ID   : 0x%04X\n", ntohs(chunk_data->stream_identifier));
						fprintf(out, "    |-Seq. number : %u\n", ntohs(chunk_data->stream_sequence_number));
						fprintf(out, "    |-Protocol ID : %u\n", ntohl(chunk_data->payload_protocol_identifier));
						break;
				}

				if(payload) {
					binio::MCArea header = sctp.chunk_header();
					if(header) {
						fprintf(out, "    |-Header [%zu] : ", header.length());
						utils::HexDumper::hex(out, header);
					}

					binio::MCArea payload = sctp.chunk_payload();
					if(payload) {
						fprintf(out, "    |-Payload [%zu] : ", payload.length());
						utils::HexDumper::hex(out, payload);
					}
				}
				//			fprintf(out, "\n");

				chunk = sctp.next();
			}
		}
	}

	static void print_mac(FILE* out, const unsigned char* mac) {
		fprintf(out, "%02X-%02X-%02X-%02X-%02X-%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	}

	static inline void print_ip(FILE* file, IPv4::Addr ip) {
		fprintf(file, "%01u.%01u.%01u.%01u", ((ip >> 24) & 0xFF), ((ip >> 16) & 0xFF), ((ip >> 8) & 0xFF),
		        ((ip) & 0xFF));
	}

private:

	static inline void print_header(FILE* out, const char* name, const StackParser& pkt) {
		binio::MCArea header = pkt.header();
		binio::MCArea payload = pkt.payload();
		fprintf(out, "[ %zu : %zu : %zu]\n", pkt.offset(), pkt.available(), pkt.padding());
		fprintf(out, "[%s] +%zu ( %zu / %zu )\n", name, pkt.offset(), header.length(), payload.length());
	}

};

}; // namespace stack_ip

#endif /* STACK_IP_DUMPER_H */

