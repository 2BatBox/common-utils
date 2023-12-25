#include <iostream>

#pragma GCC diagnostic ignored "-Wnon-virtual-dtor"
#pragma GCC diagnostic ignored "-Weffc++"


typedef unsigned Key_t;

typedef unsigned Value_t;

int main_intrusive(int, char**) {
	//int main(int, char**) {

	unsigned storage_size = 64;
	float load_factor_first = 0.7f;
	float load_factor_second = 2.0f;

	intrusive::TestLinkedList test_list(storage_size);
	test_list.test();
	std::cout << "\n";

	intrusive::TestHashMap test_map_first(storage_size, load_factor_first);
	test_map_first.test();
	std::cout << "\n";

	intrusive::TestHashMap test_map_second(storage_size, load_factor_second);
	test_map_second.test();
	std::cout << "\n";

	std::cout << "<---- the end of main_intrusive() ---->\n";
	return 0;
}

