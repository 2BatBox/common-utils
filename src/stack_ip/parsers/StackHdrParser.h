#ifndef STACK_IP_PARSERS_STACKHDRPARSER_H
#define STACK_IP_PARSERS_STACKHDRPARSER_H

#include <cstdlib>

#include "../stack_ip.h"
#include "../procotols/Ethernet.h"
#include "../procotols/Vlan.h"
#include "../procotols/Gre.h"
#include "../procotols/IPv4.h"
#include "../procotols/Udp.h"
#include "../procotols/Sctp.h"

namespace stack_ip {

/**
 * StackHdrParser is a parser of IP packet stack.
 * Might be useful to work with packets which contain headers only.
 *   
 * Using sample:
 * StackHdrParser parser(...);
 * 
 * 1.
 * parser.protocol() == Protocol::L2_ETHERNET;
 * assign_stay(ptr) sets 'ptr' to the Ethernet header.
 * Methods header(), payload() and packet() would return ByteBuffers as shown
 * in the picture.
 * 
 *   |--Ethernet--|----VLAN----|--IPv4----|----UDP----|
 *   |-header()---|
 *   |-packet()---------------------------------------|
 * 
 * 2.
 * parser.next();
 * parser.protocol() == Protocol::VLAN;
 * assign_stay(ptr) sets 'ptr' to the VLAN header.
 * 
 *                |-header()---|
 *                |-packet()--------------------------|
 * 
 * 3.
 * parser.next();
 * parser.protocol() == Protocol::IPv4;
 * assign_stay(ptr) sets 'ptr' to the IPv4 header.
 * 
 *                             |-header()-|
 *                             |-packet()-|
 * and etc.
 */

class StackHdrParser : protected DefaultPacketReader {
	using Base = DefaultPacketReader;

protected:
	Protocol proto;

public:
	using Base::assign_stay;

	// for debug purpose only
	using Base::offset;
	using Base::available;
	using Base::padding;

	StackHdrParser(binio::MCArea pkt) :
	Base(pkt),
	proto(Protocol::END) { }

	/**
	 * @param proto_first - the first protocol in the stack.
	 * @return true if the packet contains valid IP stack protocols.
	 */
	bool parse(Protocol proto_first = Protocol::L2_ETHERNET) noexcept {
		proto = validate_header(proto_first);
		return proto != Protocol::END;
	}

	/**
	 * Return a current protocol in the stack.
	 * @return current protocol
	 */
	inline Protocol protocol() const noexcept {
		return proto;
	}

	/**
	 * Step to the next protocol in the stack.
	 * @return - the next protocol.
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

		case Protocol::L4_GRE:
			next_proto = Gre::next(*this);
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

		proto = validate_header(next_proto);
		return proto;
	}

	/**
	 * @return A current packet in the stack as a MemArea object.
	 */
	inline binio::MCArea packet() const noexcept {
		return available_area();
	}

	/**
	 * @return A current protocol header as a MemArea object.
	 */
	binio::MCArea header() const noexcept {
		unsigned hdr_len = 0;
		const uint8_t* ptr = header(hdr_len);
		return binio::MCArea(ptr, hdr_len);
	}

private:

	Protocol validate_header(Protocol new_proto) noexcept {
		bool result = false;
		switch (new_proto) {
		case Protocol::L2_ETHERNET:
			result = Ethernet::validate_header(*this);
			break;

		case Protocol::L2_VLAN:
			result = Vlan::validate_header(*this);
			break;

		case Protocol::L3_IPv4:
			result = IPv4::validate_header(*this);
			break;

		case Protocol::L4_GRE:
			result = Gre::validate_header(*this);
			break;

		case Protocol::L4_UDP:
			result = Udp::validate_header(*this);
			break;

		case Protocol::L4_SCTP:
			result = Sctp::validate_header(*this);
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

		case Protocol::L4_GRE:
			out_hdr_len = Gre::length_header(*this);
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

		return Base::m_head;
	}

};

}; // namespace stack_ip

#endif /* STACK_IP_PARSERS_STACKHDRPARSER_H */

