#pragma once

#include <cstdlib>

namespace proto {

enum Protocol : unsigned char {
	L2_ETHERNET,
	L2_VLAN,
	L3_IPv4,
	L3_IPv6,
	L4_UDP,
	L4_TCP,
	L4_GRE,

	END = 100
};

}; // namespace proto

