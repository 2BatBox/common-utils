#ifndef NET_PROTOCOLS_SCTP_H
#define NET_PROTOCOLS_SCTP_H

#include "../net.h"

namespace net {

class Sctp {
public:

	enum ChuckType {
		DATA = 0, // Payload data
		INIT = 1, // Initiation
		INIT_ACK = 2, // Initiation acknowledgement
		SACK = 3, // Selective acknowledgement
		HEARTBEAT = 4, // Heartbeat request
		HEARTBEAT_ACK = 5, // Heartbeat acknowledgement
		ABORT = 6, // Abort
		SHUTDOWN = 7, // Shutdown
		SHUTDOWN_ACK = 8, // Shutdown acknowledgement
		ERROR = 9, // Operation error
		COOKIE_ECHO = 10, // State cookie
		COOKIE_ACK = 11, // Cookie acknowledgement
		ECNE = 12, // Explicit congestion notification echo(reserved)
		CWR = 13, // Congestion window reduced(reserved)
		SHUTDOWN_COMPLETE = 14, // Shutdown complete
		AUTH = 15, // Authentication chunk
		IETF_EXT_63 = 63, // chunk extensions
		I_DATA = 64, // Payload data supporting packet interleaving
		IETF_EXT_127 = 127, // chunk extensions
		ASCONF_ACK = 128, // Address configuration change acknowledgement
		RE_CONFIG = 130, // Stream reconfiguration
		PAD = 132, // Packet padding
		IETF_EXT_191 = 191, // chunk extensions
		FORWARD_TSN = 192, // Increment expected TSN
		ASCONF = 193, // Address configuration change
		I_FORWARD_TSN = 194, // Increment expected TSN, supporting packet interleaving
		IETF_EXT_255 = 255, // chunk extensions
	};

	struct ChunkHeader {
		uint8_t type;
		uint8_t flags;
		uint16_t length;
	} __attribute__ ((__packed__));

	struct ChunkData {
		ChunkHeader header;
		uint32_t transmission_sequence_number;
		uint16_t stream_identifier;
		uint16_t stream_sequence_number;
		uint32_t payload_protocol_identifier;
	} __attribute__ ((__packed__));

	struct Header {
		uint16_t port_src;
		uint16_t port_dst;
		uint32_t verefication_tag;
		uint32_t check_sum;
	} __attribute__ ((__packed__));

	static inline bool validate_packet(const Packet& pkt) noexcept {
		return pkt.available(sizeof (Header));
	}

	static inline bool validate_header(const Packet& pkt) noexcept {
		return pkt.available(sizeof (Header));
	}

	static Protocol next(Packet& pkt) noexcept {
		pkt.head_move(sizeof (Header));
		return Protocol::END;
	}

	static inline constexpr unsigned length_header(const Packet&) noexcept {
		return sizeof (Header);
	}

	static inline unsigned length_payload(const Packet& pkt) noexcept {
		return pkt.available() - sizeof (Header);
	}

	static const char* chunk_name(uint8_t chunk) noexcept {
		switch (chunk) {
		case DATA: return "DATA";
		case INIT: return "INIT";
		case INIT_ACK: return "INIT_ACK";
		case SACK: return "SACK";
		case HEARTBEAT: return "HEARTBEAT";
		case HEARTBEAT_ACK: return "HEARTBEAT_ACK";
		case ABORT: return "ABORT";
		case SHUTDOWN: return "SHUTDOWN";
		case SHUTDOWN_ACK: return "SHUTDOWN_ACK";
		case ERROR: return "ERROR";
		case COOKIE_ECHO: return "COOKIE_ECHO";
		case COOKIE_ACK: return "COOKIE_ACK";
		case ECNE: return "ECNE";
		case CWR: return "CWR";
		case SHUTDOWN_COMPLETE: return "SHUTDOWN_COMPLETE";
		case AUTH: return "AUTH";
		case IETF_EXT_63: return "IETF_EXT_63";
		case I_DATA: return "I_DATA";
		case IETF_EXT_127: return "IETF_EXT_127";
		case ASCONF_ACK: return "ASCONF_ACK";
		case RE_CONFIG: return "RE_CONFIG";
		case PAD: return "PAD";
		case IETF_EXT_191: return "IETF_EXT_191";
		case FORWARD_TSN: return "FORWARD_TSN";
		case ASCONF: return "ASCONF";
		case I_FORWARD_TSN: return "I_FORWARD_TSN";
		case IETF_EXT_255: return "IETF_EXT_255";
		default: return "UNKNOWN";
		}
	}

};

}; // namespace net

#endif /* NET_PROTOCOLS_SCTP_H */

