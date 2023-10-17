#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <climits>
#include <cassert>
#include <list>
#include <string>

#include <cstdio>
#include <cassert>
#include <utils/DiceMachine.h>
#include <containers/bits/BitStream.h>

#include "containers/bits/BitArray.h"

int main(int argc, char** argv) {

	BitStream::Byte_t pattern[] = { 0x12, 0x34, 0x56, 0x78, 0x9A };
	BitStream bs(pattern, sizeof(pattern));
	BitStream::Chunk_t chunk;
	while(bs.read(chunk, 1)) {
		printf("%zx.", chunk);
		if(bs.offset() % 8 == 0){
			printf("  ");
		}
	}
	printf("\n");

	printf("<---- the end of main() ---->\n");
	return EXIT_SUCCESS;
}
