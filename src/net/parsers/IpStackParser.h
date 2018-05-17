#ifndef NET_PARSERS_IP_STACK_PARSER_H
#define NET_PARSERS_IP_STACK_PARSER_H

#include <cstdlib>

#include "../../binio/ArrayPointer.h"

#include "../net.h"
#include "../BasicPacket.h"
#include "../procotols/Ethernet.h"
#include "../procotols/Vlan.h"
#include "../procotols/IPv4.h"
#include "../procotols/Udp.h"
#include "../procotols/Sctp.h"

namespace net {

/**
 * Packet
 * Lx - Headers
 * D - data bytes
 * P - padding bytes
 *   
 *                | <-                     size                  -> |
 *                | <- offset -> | <- available ->  | <- padding -> |
 * packet         |L2|L2|L3|L3|L3|L3|L4|L4|D|D|D|D|D|P|P|P|P|P|P|P|P|
 * assign points        | next() -> |
 *                    head        next
 * 
 */

class IpStackParser : public Packet {
	using Base = Packet;

protected:
	Protocol proto;

public:

	IpStackParser(binio::ByteArrayConstPtr pkt, Protocol proto = Protocol::L2_ETHERNET) :
	Base(pkt),
	proto(proto) {
		proto = validate_packet(proto);
	}

	template <typename T>
	IpStackParser(T* raw_data, unsigned raw_data_len, Protocol proto = Protocol::L2_ETHERNET) :
	Base(raw_data, raw_data_len),
	proto(proto) {
		proto = validate_packet(proto);
	}

	/**
	 * Return a protocol the head set on.
	 * @return current protocol
	 */
	inline Protocol protocol() const noexcept {
		return proto;
	}

	/**
	 * @return 
	 */
	Protocol next() noexcept {
		Protocol next_proto = Protocol::END;
		switch (proto) {
		case Protocol::L2_ETHERNET:
			next_proto = Ethernet::next(*this);
			break;
		case Protocol::L2_VLAN:
			next_proto = Vlan::next(*this);
			break;
		case Protocol::L3_IPv4:
			next_proto = IPv4::next(*this);
			break;
		case Protocol::L4_UDP:
			next_proto = Udp::next(*this);
			break;
		case Protocol::L4_SCTP:
			next_proto = Sctp::next(*this);
			break;

		default:
			break;
		}

		proto = validate_packet(next_proto);
		return proto;
	}

	binio::ByteArrayConstPtr header() const noexcept {
		unsigned hdr_len;
		const uint8_t* ptr = header(hdr_len);
		return binio::ByteArrayConstPtr(ptr, hdr_len);
	}

	binio::ByteArrayConstPtr payload() const noexcept {
		unsigned payload_len;
		const uint8_t* ptr = payload(payload_len);
		return binio::ByteArrayConstPtr(ptr, payload_len);
	}

private:

	Protocol validate_packet(Protocol new_proto) noexcept {
		bool result = false;
		switch (new_proto) {
		case Protocol::L2_ETHERNET:
			result = Ethernet::validate_packet(*this);
			break;

		case Protocol::L2_VLAN:
			result = Vlan::validate_packet(*this);
			break;

		case Protocol::L3_IPv4:
			result = IPv4::validate_packet(*this);
			break;

		case Protocol::L4_UDP:
			result = Udp::validate_packet(*this);
			break;

		case Protocol::L4_SCTP:
			result = Sctp::validate_packet(*this);
			break;

		default:
			break;
		}
		if (not result)
			new_proto = Protocol::END;

		return new_proto;
	}

	const uint8_t* header(unsigned& out_hdr_len) const noexcept {
		switch (proto) {
		case Protocol::L2_ETHERNET:
			out_hdr_len = Ethernet::length_header(*this);
			break;

		case Protocol::L2_VLAN:
			out_hdr_len = Vlan::length_header(*this);
			break;

		case Protocol::L3_IPv4:
			out_hdr_len = IPv4::length_header(*this);
			break;

		case Protocol::L4_UDP:
			out_hdr_len = Udp::length_header(*this);
			break;

		case Protocol::L4_SCTP:
			out_hdr_len = Sctp::length_header(*this);
			break;

		default:
			return nullptr;
		}

		return Base::ptr_head;
	}

	const uint8_t* payload(unsigned& out_payload_len) const noexcept {
		unsigned hdr_len = 0;

		switch (proto) {
		case Protocol::L2_ETHERNET:
			out_payload_len = Ethernet::length_payload(*this);
			break;

		case Protocol::L2_VLAN:
			out_payload_len = Vlan::length_payload(*this);
			break;

		case Protocol::L3_IPv4:
			out_payload_len = IPv4::length_payload(*this);
			break;

		case Protocol::L4_UDP:
			out_payload_len = Udp::length_payload(*this);
			break;

		case Protocol::L4_SCTP:
			out_payload_len = Sctp::length_payload(*this);
			break;

		default:
			return nullptr;
		}

		header(hdr_len);

		return Base::ptr_head + hdr_len;
	}

};

}; // namespace net

#endif /* NET_PARSERS_IP_STACK_PARSER_H */

