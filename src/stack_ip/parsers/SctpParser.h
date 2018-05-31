#ifndef STACK_IP_PARSERS_SCTP_PARSER_H
#define STACK_IP_PARSERS_SCTP_PARSER_H

#include "../stack_ip.h"
#include "StackParser.h"

namespace stack_ip {

/**
 * SctpParser is a parser of SCTP packets.
 * The packet data MUST be provided completely, it may contain padding.
 *   
 * SctpParser parser(...);
 * parser.parser() == true;
 * assign_stay() might be useful to assign a specific chunk type to the current chunk position.
 * const stack_ip::Sctp::ChunkHeader* chunk = sctp.chunk_ptr();
 * if (chunk && chunk->type == stack_ip::Sctp::DATA) {
 *	const stack_ip::Sctp::ChunkData* hdr_data;
 *  sctp.assign_stay(hdr_data); 
 * }
 * 
 * 1.
 *   |--Header--|--Chunk_0_hdr--\--Chunk_0_data--|--Chunk_1_hdr--\--Chunk_1_data--|...
 *              |-chunk_header()|
 *                              |-chunk_payload()|
 *              |-chunk()------------------------|
 * 
 * 2.
 * parser.next() != nullptr;
 *                                               |-chunk_header()|
 *                                                               |-chunk_payload()|
 *                                               |-chunk()------------------------|
 * ant etc.
 */

class SctpParser : protected DefaultPacketReader {
	using Base = DefaultPacketReader;

	const Sctp::Header* ptr_header;
	const Sctp::ChunkHeader* ptr_chunk;

public:
	using Base::assign_stay;

	// for debug purpose only
	using Base::offset;
	using Base::available;
	using Base::padding;

	SctpParser(binio::ByteConstBuffer pkt) :
	Base(pkt),
	ptr_header(nullptr),
	ptr_chunk(nullptr) { }

	/**
	 * @return true if the parser contains valid SCTP packet.
	 */
	bool parse() noexcept {
		if (Sctp::validate_packet(*this)) {
			assign(ptr_header);
			ptr_chunk = validate_chunk();
			return ptr_chunk != nullptr;
		}
		return false;
	}

	/**
	 * @return the data (header + payload) of the current chunk as a ByteConstBuffer.
	 */
	binio::ByteConstBuffer chunk() const noexcept {
		unsigned chunk_bytes = 0;
		const uint8_t* ptr = chunk(chunk_bytes);
		return binio::ByteConstBuffer(ptr, chunk_bytes);
	}

	/**
	 * @return the header of the current chunk as a ByteConstBuffer.
	 */
	binio::ByteConstBuffer chunk_header() const noexcept {
		unsigned header_bytes = 0;
		const uint8_t* ptr = chunk_header(header_bytes);
		return binio::ByteConstBuffer(ptr, header_bytes);
	}

	/**
	 * @return the payload of the current chunk as a ByteConstBuffer.
	 */
	binio::ByteConstBuffer chunk_payload() const noexcept {
		unsigned payload_bytes = 0;
		const uint8_t* ptr = chunk_payload(payload_bytes);
		return binio::ByteConstBuffer(ptr, payload_bytes);
	}

	/**
	 * @return a pointer to the header of the packet.
	 */
	const Sctp::Header* header_ptr() const noexcept {
		return ptr_header;
	}

	/**
	 * @return a pointer to the current chunk header.
	 */
	const Sctp::ChunkHeader* chunk_ptr() const noexcept {
		return ptr_chunk;
	}

	/**
	 * Step to the next chunk in the packet.
	 * @return - the next chunk pointer of nullptr.
	 */
	const Sctp::ChunkHeader* next() noexcept {
		if (ptr_chunk) {
			uint16_t length_total = ntohs(ptr_chunk->length);
			head_move(length_total);
			ptr_chunk = validate_chunk();
		}
		return ptr_chunk;
	}


protected:

	const Sctp::ChunkHeader* validate_chunk() noexcept {
		const Sctp::ChunkHeader* result;
		if (available(sizeof (Sctp::ChunkHeader))) {
			assign_stay(result);
			uint16_t length_total = ntohs(result->length);
			uint16_t length_header = Sctp::chunk_header_size(result->type);
			if (length_header <= length_total && available(length_total)) {
				return result;
			}
		}
		return nullptr;
	}

	inline const uint8_t* chunk(unsigned& chunk_bytes) const noexcept {
		if (ptr_chunk) {
			chunk_bytes = ntohs(ptr_chunk->length);
			return ptr_head;
		}
		return nullptr;
	}

	const uint8_t* chunk_header(unsigned& header_bytes) const noexcept {
		const uint8_t* result = nullptr;
		if (ptr_chunk) {
			header_bytes = Sctp::chunk_header_size(ptr_chunk->type);
			result = ptr_head;
		}
		return result;
	}

	const uint8_t* chunk_payload(unsigned& payload_bytes) const noexcept {
		const uint8_t* result = nullptr;
		if (ptr_chunk) {
			uint16_t length_total = ntohs(ptr_chunk->length);
			uint16_t length_header = Sctp::chunk_header_size(ptr_chunk->type);
			result = ptr_head + length_header;
			payload_bytes = length_total - length_header;
		}
		return result;
	}

};

}; // namespace stack_ip

#endif /* STACK_IP_PARSERS_SCTP_PARSER_H */

