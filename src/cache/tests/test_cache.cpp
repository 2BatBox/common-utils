#ifndef CACHE_SAMPLE_H
#define CACHE_SAMPLE_H

#pragma GCC diagnostic ignored "-Wnon-virtual-dtor"
#pragma GCC diagnostic ignored "-Weffc++"

#include "TestLruCache.h"
#include "TestLruAssembler.h"
//#include "TimerQueueTest.h"

#include "../LruAssembler.h"

#include <iostream>

typedef unsigned Key_t;
typedef unsigned Value_t;

int main_cache(int, char**) {
//int main(int, char**) {
	unsigned storage_size = 8;
	float load_factor = 0.2f;

	cache::TestLruCache cache_test(storage_size, load_factor);
	cache_test.test();
	std::cout << "\n";

	cache::TestLruAssembler asm_test(storage_size, load_factor);
	asm_test.test();
	std::cout << "\n";

	load_factor = 10.0f;
	cache::TestLruCache cache_test2(storage_size, load_factor);
	cache_test2.test();
	std::cout << "\n";

	cache::TestLruAssembler asm_test2(storage_size, load_factor);
	asm_test2.test();
	std::cout << "\n";

	//	cache::TimerQueueTest time_cache_test(storage_size, load_factor);
	//	time_cache_test.test();
	//	std::cout << "\n";

	std::cout << "<---- the end of main_cache() ---->\n";
	return 0;
}

#endif /* CACHE_SAMPLE_H */

