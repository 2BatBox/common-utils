#pragma once

#include "Frame.h"

#include <cstdio>

namespace pcapwrap {

class Dumper {
public:

	static void frame(FILE* out, const Frame& frame, bool payload = false) {
		fprintf(out, "[%zu] %u / %u bytes", frame.m_idx, frame.m_hdr.len, frame.m_hdr.caplen);
		fprintf(out, " epoc=%ld.%ld sec", frame.m_hdr.ts.tv_sec, frame.m_hdr.ts.tv_usec);
		fprintf(out, "\n");
		if(payload){
			//TODO: hex dump here
		}
	}

};

}; // namespace pcapwrap

