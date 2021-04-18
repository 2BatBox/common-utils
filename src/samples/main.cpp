#include "../lib/cli/types/RangeSet.h"
#include "../lib/cli/types/MacAddress.h"
#include "../lib/cli/types/Integer.h"

#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <climits>
#include <cassert>

using namespace cli;

int main(int argc, char** argv) {

	RangeSet rs;
	MacAddress ms;

//	uint8_t u8 = 0;
//	int8_t i8 = 0;
//
//	uint16_t u16 = 0;
//	int16_t i16 = 0;
//
//	uint32_t u32 = 0;
	int32_t i32 = 0;
//
//	uint64_t u64 = 0;
	int64_t i64 = 0;
//
//	assert(Integer::parse("0", i8) && i8 == 0);
//	assert(Integer::parse("1", i8) && i8 == 1);
//	assert(Integer::parse("127", i8) && i8 == 127);
//	assert(not Integer::parse("128", i8));
//
//	assert(Integer::parse("0", i64) && i64 == 0);
//	assert(Integer::parse("1", i64) && i64 == 1);
//	assert(Integer::parse("9223372036854775807", i64) && i64 == 9223372036854775807ll);
	assert(Integer::parse("0", i64, 16));
//	assert(Integer::parse("7fffffff", i32, 16));
	printf("value=%zx\n", i64);

//	Integer::parse("127", i16);
//	Integer::parse("127", i32);
//	Integer::parse("127", i64);
//	const auto offset = Integer::parse("127", i64);


	printf("<---- the end of main() ---->\n");
	return EXIT_SUCCESS;
}
