#include <iostream>

#include "TestMArea.h"
#include "TestPacket.h"
#include "TestPacketSafe.h"
#include "TestVLArray.h"

using namespace binio;

int main(int, char**) {

//int main_binio(int, char**) {

	TestMArea byte_marea;
	byte_marea.test();

	TestPacket packet;
	packet.test();

	TestPacketSafe packet_safe;
	packet_safe.test();

	TestVLArray static_array;
	static_array.test();

	std::cout << "<---- the end of main_binio() ---->\n";
	return 0;
}


