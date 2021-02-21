#pragma once

#include <cstdlib>

#include "../proto.h"
#include "../mframe/MFrame.h"
#include "../mframe/SafeMFrame.h"

#include "../procotols/Ethernet.h"
#include "../procotols/Vlan.h"
#include "../procotols/IPv4.h"
#include "../procotols/IPv6.h"
#include "../procotols/Tcp.h"
#include "../procotols/Udp.h"
#include "../procotols/Gre.h"

namespace proto {

/**
 * BasicHeaderParser is a protocol stack parser.
 * Might be used to work with packets which contain headers only.
 *   
 * Using sample:
 * BasicHeaderParser parser(...);
 * 
 * 1.
 * parser.protocol() == Protocol::L2_ETHERNET;
 * assign_stay(ptr) sets 'ptr' to the Ethernet header.
 *
 *  |--Ethernet--|----VLAN----|--IPv4----|----UDP----|
 *  |
 * head
 *
 * 2.
 * parser.next();
 * parser.protocol() == Protocol::VLAN;
 * assign_stay(ptr) sets 'ptr' to the VLAN header.
 *
 *  |--Ethernet--|----VLAN----|--IPv4----|----UDP----|
 *               |
 *              head
 *
 * 3.
 * parser.next();
 * parser.protocol() == Protocol::IPv4;
 * assign_stay(ptr) sets 'ptr' to the IPv4 header.
 * 
 *  |--Ethernet--|----VLAN----|--IPv4----|----UDP----|
 *                            |
 *                           head
 *
 * and etc.
 */

template <typename MFrame>
class BasicHeaderParser {
protected:
	MFrame m_frame;
	Protocol proto;

public:

	template <typename Ptr>
	BasicHeaderParser(const Ptr* buffer, size_t size_bytes, Protocol proto_first = Protocol::L2_ETHERNET) noexcept :
		m_frame(buffer, size_bytes), proto(validate_header(proto_first)) {}

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
		switch(proto) {
			case Protocol::L2_ETHERNET:
				next_proto = Ethernet::next(m_frame);
				break;

			case Protocol::L2_VLAN:
				next_proto = Vlan::next(m_frame);
				break;

			case Protocol::L3_IPv4:
				next_proto = IPv4::next(m_frame);
				break;

			case Protocol::L3_IPv6:
				next_proto = IPv6::next(m_frame);
				break;

			case Protocol::L4_TCP:
				next_proto = Tcp::next(m_frame);
				break;

			case Protocol::L4_UDP:
				next_proto = Udp::next(m_frame);
				break;

			case Protocol::L4_GRE:
				next_proto = Gre::next(m_frame);
				break;

			default:
				break;
		}

		proto = validate_header(next_proto);
		return proto;
	}

	template <typename Hdr>
	inline void assign(const Hdr*& hdr) noexcept {
		m_frame.assign_stay(hdr);
	}

private:

	inline Protocol validate_header(Protocol new_proto) noexcept {
		bool result = false;
		switch(new_proto) {
			case Protocol::L2_ETHERNET:
				result = Ethernet::validate_header(m_frame);
				break;

			case Protocol::L2_VLAN:
				result = Vlan::validate_header(m_frame);
				break;

			case Protocol::L3_IPv4:
				result = IPv4::validate_header(m_frame);
				break;

			case Protocol::L3_IPv6:
				result = IPv6::validate_header(m_frame);
				break;

			case Protocol::L4_TCP:
				result = Tcp::validate_header(m_frame);
				break;

			case Protocol::L4_UDP:
				result = Udp::validate_header(m_frame);
				break;

			case Protocol::L4_GRE:
				result = Gre::validate_header(m_frame);
				break;

			default:
				break;
		}
		if(not result) {
			new_proto = Protocol::END;
		}

		return new_proto;
	}

};

using HeaderParser = BasicHeaderParser<RoMFrame>;
using SafeHeaderParser = BasicHeaderParser<RoSafeMFrame>;

}; // namespace proto

