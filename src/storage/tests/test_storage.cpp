#pragma GCC diagnostic ignored "-Wnon-virtual-dtor"
#pragma GCC diagnostic ignored "-Weffc++"

#include "../../dpdk/Utils.h"
#include "../../dpdk/Dumper.h"
#include "TestTimedQueue.h"
#include "TestRateLimiter.h"

using namespace storage;

int main(int argc, char** argv) {
	//int main_storage(int argc, char** argv) {

	dpdk::Utils::init(argc, argv);

	unsigned storage_size = 1 << 24;
	float load_factor_one = 0.7f;
	float load_factor_two = 3.0f;

	//	TestTimedQueue queue_first(storage_size, load_factor_one);
	//	queue_first.test();
	//	std::cout << "\n";
	//
	//	TestTimedQueue queue_second(storage_size, load_factor_two);
	//	queue_second.test();
	//	std::cout << "\n";

	TestRateLimiter rate_limiter_first(storage_size, load_factor_one);
	rate_limiter_first.test();

	TestRateLimiter rate_limiter_second(storage_size, load_factor_two);
	rate_limiter_second.test();

	dpdk::Dumper::malloc_stat(stdout);

	std::cout << "<---- the end of main_storage() ---->\n";
	return 0;
}
