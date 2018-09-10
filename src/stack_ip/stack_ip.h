#ifndef STACK_IP_STACK_IP_H
#define STACK_IP_STACK_IP_H

#include <cstdlib>

#include "../binio/packet/PacketReader.h"

namespace stack_ip {

using DefaultPacketReader = binio::PacketReader;

enum class Protocol : uint8_t {
	L2_ETHERNET,
	L2_VLAN,
	L3_IPv4,
	L4_UDP,
	L4_GRE,
	L4_SCTP,

	END = 100
};

}; // namespace stack_ip

#endif /* STACK_IP_STACK_IP_H */

