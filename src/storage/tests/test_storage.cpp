#pragma GCC diagnostic ignored "-Wnon-virtual-dtor"
#pragma GCC diagnostic ignored "-Weffc++"

#include "../../dpdk/Utils.h"
#include "TestTimedQueue.h"
#include "TestIpTable.h"
#include "TestPyramid.h"

using namespace storage;

//int main(int argc, char** argv) {
int main_storage(int argc, char** argv) {

//	dpdk::Utils::init(argc, argv);
//
	size_t storage_size = 1024 * 1024 * 1024;
//	float load_factor_one = 0.7f;
//	float load_factor_two = 3.0f;
//
//	TestTimedQueue queue_first(storage_size, load_factor_one);
//	queue_first.test();
//	std::cout << "\n";
//
//	TestTimedQueue queue_second(storage_size, load_factor_two);
//	queue_second.test();
//	std::cout << "\n";
//
//	TestRateLimiter rate_limiter_first(storage_size, load_factor_one);
//	rate_limiter_first.test();
//	std::cout << "\n";
//
//	TestRateLimiter rate_limiter_second(storage_size, load_factor_two);
//	rate_limiter_second.test();
//	std::cout << "\n";
//
//	TestIpTable ip_table_first(storage_size, load_factor_one);
//	ip_table_first.test();
//	std::cout << "\n";
//
//	TestIpTable ip_table_second(storage_size, load_factor_two);
//	ip_table_second.test();
//	std::cout << "\n";
//
//	dpdk::Dumper::malloc_stat(stdout);

	TestPyramid pyramid(storage_size);
	pyramid.test();

	std::cout << "<---- the end of main_storage() ---->\n";
	return 0;
}
