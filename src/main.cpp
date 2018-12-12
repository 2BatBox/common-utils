
#include "binio/MArea.h"
#include "utils/HexDumper.h"

#include <iostream>

struct Mtp3Message {
	uint32_t src = 0;
	uint32_t dst = 0;
	uint32_t length = 0;
	bool presented;
	binio::MCArea payload;

	Mtp3Message() noexcept = default;

	void dump(FILE* out) const noexcept {
		//        if (presented) {
		fprintf(out, "| MTP 0x%x -> 0x%x (%u b) ", src, dst, length);
		//        }
		utils::HexDumper::memory(out, payload);
	}
};

int main_main(int, char**) {

//	pid_t tid;
//	unsigned summ = 0;
//	for (int i = 0; i < 10000000; i++) {
//		tid = syscall(SYS_gettid);
//	}

	std::cout << "<---- the end of main_main() ---->\n";
	return 0;
}


