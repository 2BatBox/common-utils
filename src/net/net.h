#ifndef NET_NET_H
#define NET_NET_H

#include <cstdlib>

#include "BasicPacket.h"

namespace net {

using Packet = BasicPacket<unsigned>;

enum class Protocol : uint8_t {
	L2_ETHERNET,
	L2_VLAN,
	L3_IPv4,
	L4_UDP,
	L4_SCTP,

	END = 100
};

}; // namespace net


#endif /* NET_NET_H */

