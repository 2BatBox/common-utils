#ifndef UTILS_H
#define UTILS_H

#include <string.h>

#include "Pcap.h"

namespace pcap_utils {

class Dumper {
public:

	static void dump(const Frame& fr, FILE* file = stdout) {
		fprintf(file, "data=%p len=%u caplen=%u ", fr.data, fr.hdr.len, fr.hdr.caplen);
		fprintf(file, "sec=%ld usec=%ld\n", fr.hdr.ts.tv_sec, fr.hdr.ts.tv_usec);
	}

	static void dump(const FrameMutable& fr, FILE* file = stdout) {
		fprintf(file, "data=%p len=%u caplen=%u ", fr.data, fr.hdr.len, fr.hdr.caplen);
		fprintf(file, "sec=%ld usec=%ld\n", fr.hdr.ts.tv_sec, fr.hdr.ts.tv_usec);
	}

};

struct FrameHeaderComparator {

	bool operator()(const Frame& x, const Frame& y) const noexcept {
		return x.hdr.len == y.hdr.len
			&& x.hdr.caplen == y.hdr.caplen
			&& timercmp(&(x.hdr.ts), &(y.hdr.ts), ==);
	}
};

struct FramePayloadComparator {

	bool operator()(const Frame& x, const Frame& y) const noexcept {
		return x.hdr.len == y.hdr.len
			&& x.hdr.caplen == y.hdr.caplen
			&& (memcmp(x.data, y.data, x.hdr.len) == 0);
	}
};

struct FrameComparator {

	bool operator()(const Frame& x, const Frame& y) const noexcept {
		return x.hdr.len == y.hdr.len
			&& x.hdr.caplen == y.hdr.caplen
			&& timercmp(&(x.hdr.ts), &(y.hdr.ts), ==)
			&& (memcmp(x.data, y.data, x.hdr.len) == 0);
	}
};

}; // namespace pcap_utils

#endif /* UTILS_H */

