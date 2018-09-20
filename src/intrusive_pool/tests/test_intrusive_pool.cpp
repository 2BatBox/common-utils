#ifndef ICC_TESTS_ICC_H
#define ICC_TESTS_ICC_H

#pragma GCC diagnostic ignored "-Wnon-virtual-dtor"
#pragma GCC diagnostic ignored "-Weffc++"

#include "TestHashQueuePool.h"
#include "TestDequePool.h"

#include <iostream>

using namespace intrusive;

//int main(int, char**) {
int main_intrusive_pool(int, char**) {
	
	unsigned storage_size = 64;
	float load_factor_one = 0.7f;
	float load_factor_two = 2.0f;

	TestDequePool deque_pool(storage_size);
	deque_pool.test();
	std::cout << "\n";

	TestHashQueuePool linked_hash_pool_first(storage_size, load_factor_one);
	linked_hash_pool_first.test();
	std::cout << "\n";

	TestHashQueuePool linked_hash_pool_second(storage_size, load_factor_two);
	linked_hash_pool_second.test();
	std::cout << "\n";

	std::cout << "<---- the end of main_intrusive_pool() ---->\n";
	return 0;
}

#endif /* ICC_TESTS_ICC_H */

