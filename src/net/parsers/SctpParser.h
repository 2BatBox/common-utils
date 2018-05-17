#ifndef NET_PARSERS_SCTP_PARSER_H
#define NET_PARSERS_SCTP_PARSER_H

#include "../net.h"
#include "IpStackParser.h"
#include "../../binio/ArrayPointer.h"

namespace net {

/**
 * SctpPacket
 * H - Sctp Header bytes
 * Cx - Chunks bytes
 *   
 *                | <-                     size                  -> |
 *                | <- offset -> | <- available -> | <- padding  -> |
 * packet         |H|H|H|C0|C0|C0|C0|C1|C1|C1|C2|...................|
 * assign points                 | next() -> |
 *                             chunk()
 */

class SctpParser : public Packet {
	using Base = Packet;
	
	const Sctp::ChunkHeader* current_chunk;

public:

	SctpParser(binio::ByteArrayConstPtr pkt) :
	Base(pkt),
	current_chunk(nullptr) {
		validate_packet();
	}

	template <typename T>
	SctpParser(T* raw_data, unsigned raw_data_len) :
	Base(raw_data, raw_data_len),
	current_chunk(nullptr) {
		validate_packet();
	}

	const Sctp::ChunkHeader* chunk() noexcept {
		return current_chunk;
	}

	const Sctp::ChunkHeader* next() noexcept {
		if (current_chunk) {
			uint16_t length_total = ntohs(current_chunk->length);
			head_move(length_total);
			current_chunk = validate_chunk();
		}
		return current_chunk;
	}

	binio::ByteArrayConstPtr chunk_header() const noexcept {
		unsigned header_bytes;
		const uint8_t* ptr = chunk_header(header_bytes);
		return binio::ByteArrayConstPtr(ptr, header_bytes);
	}

	binio::ByteArrayConstPtr chunk_payload() const noexcept {
		unsigned payload_bytes;
		const uint8_t* ptr = chunk_payload(payload_bytes);
		return binio::ByteArrayConstPtr(ptr, payload_bytes);
	}

protected:

	void validate_packet() noexcept {
		if (Sctp::validate_packet(*this)) {
			Sctp::next(*this);
			current_chunk = validate_chunk();
		}
	}

	const Sctp::ChunkHeader* validate_chunk() noexcept {
		const Sctp::ChunkHeader* result;
		if (assign_try(result)) {
			uint16_t length_total = ntohs(result->length);
			uint16_t length_header = chunk_header_size(result->type);
			if (length_header <= length_total && available(length_total))
				return result;
		}
		return nullptr;
	}

	const uint8_t* chunk_header(unsigned& header_bytes) const noexcept {
		const uint8_t* result = nullptr;
		if (current_chunk) {
			header_bytes = chunk_header_size(current_chunk->type);
			result = ptr_head;
		}
		return result;
	}

	const uint8_t* chunk_payload(unsigned& payload_bytes) const noexcept {
		const uint8_t* result = nullptr;
		if (current_chunk) {
			uint16_t length_total = ntohs(current_chunk->length);
			uint16_t length_header = chunk_header_size(current_chunk->type);
			result = ptr_head + length_header;
			payload_bytes = length_total - length_header;
		}
		return result;
	}

	static unsigned chunk_header_size(uint8_t type) noexcept {
		switch (type) {
		case Sctp::DATA:
			return sizeof (Sctp::ChunkData);
		}
		return 0;
	}

};

}; // namespace net

#endif /* NET_PARSERS_SCTP_PARSER_H */

