#include <iostream>

#pragma GCC diagnostic ignored "-Wnon-virtual-dtor"
#pragma GCC diagnostic ignored "-Weffc++"

#include "TestList.h"
#include "TestMap.h"

typedef unsigned Key_t;
typedef unsigned Value_t;

int main_intrusive(int, char**) {
	//	int main(int, char**) {

	unsigned storage_size = 16;
	float load_factor = 0.7f;
	intrusive::TestList list_test(storage_size);
	list_test.test();
	std::cout << "\n";

	intrusive::TestMap map_test(storage_size, load_factor);
	map_test.test();
	std::cout << "\n";

	load_factor = 10.0f;
	intrusive::TestList list_test2(storage_size);
	list_test2.test();
	std::cout << "\n";

	intrusive::TestMap map_test2(storage_size, load_factor);
	map_test2.test();
	std::cout << "\n";

	std::cout << "<---- the end of main_intrusive() ---->\n";
	return 0;
}

