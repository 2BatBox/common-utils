#ifndef CACHE_SAMPLE_H
#define CACHE_SAMPLE_H

#pragma GCC diagnostic ignored "-Wnon-virtual-dtor"
#pragma GCC diagnostic ignored "-Weffc++"

#include "TestLinkedHashPool.h"
#include "TestLinkedPool.h"

#include <iostream>

typedef unsigned Key_t;
typedef unsigned Value_t;

//int main(int, char**) {

int main_cache(int, char**) {
	unsigned storage_size = 1024;
	float load_factor_one = 0.2f;
	float load_factor_two = 10.0f;

	cache::TestLinkedPool linked_pool(storage_size);
	linked_pool.test();
	std::cout << "\n";

	cache::TestLinkedHashPool linked_hash_pool_first(storage_size, load_factor_one);
	linked_hash_pool_first.test();
	std::cout << "\n";

	cache::TestLinkedHashPool linked_hash_pool_second(storage_size, load_factor_two);
	linked_hash_pool_second.test();
	std::cout << "\n";

	std::cout << "<---- the end of main_cache() ---->\n";
	return 0;
}

#endif /* CACHE_SAMPLE_H */

