#include <iostream>

#include "TestList.h"
#include "TestMap.h"

typedef unsigned Key_t;
typedef unsigned Value_t;

int main(int, char**) {
	
	unsigned storage_size = 1024;
	float load_factor = 0.7f;
	intrusive::TestList list_test(storage_size);
	list_test.test();
	std::cout << "\n";

	intrusive::TestMap map_test(storage_size, load_factor);
	map_test.test();
	std::cout << "\n";

	std::cout << "<---- the end of main_intrusive() ---->\n";
	return 0;
}

