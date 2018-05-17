#ifndef PCAP_UTILS_PCAP_H
#define PCAP_UTILS_PCAP_H

#include <pcap.h>
#include <string.h>
#include <stdint.h>

namespace pcap_utils {

struct Frame {
	pcap_pkthdr hdr;
	const uint8_t* data;
    Frame() noexcept : hdr(), data(nullptr) {} 
    Frame(const pcap_pkthdr& hdr, const uint8_t* data) noexcept : hdr(hdr), data(data) {} 
};

class FrameMutable {
public:
	pcap_pkthdr hdr;
	uint8_t* data;

	FrameMutable() noexcept: hdr(), data(nullptr) { }

	FrameMutable(struct timeval ts, unsigned data_size) noexcept: hdr(), data(new unsigned char[data_size]) {
		hdr.len = hdr.caplen = data_size;
		hdr.ts = ts;
	}

	FrameMutable(const Frame& frame) noexcept: hdr(frame.hdr), data(new unsigned char[frame.hdr.len]) {
		memcpy(data, frame.data, hdr.len);
	}

	FrameMutable(const FrameMutable&) = delete;
	FrameMutable& operator=(const FrameMutable&) = delete;

	FrameMutable(FrameMutable&& rvalue) noexcept: hdr(rvalue.hdr), data(rvalue.data) {
		rvalue.make_empty();
	}

	FrameMutable& operator=(FrameMutable&& rvalue) noexcept {
		if (this != &rvalue) {
			destroy();
			hdr = rvalue.hdr;
			data = rvalue.data;
			rvalue.make_empty();
		}
		return *this;
	}

	~FrameMutable() noexcept {
		destroy();
	}

	inline operator Frame() const noexcept {
		return Frame(hdr, data);
	}

private:

	inline void destroy() noexcept {
		if (data) {
			delete [] data;
			data = nullptr;
            make_empty();
		}
	}
    
    inline void make_empty() noexcept {
		hdr.len = hdr.caplen = 0;
		timerclear(&(hdr.ts));
    }
};

}; // namespace pcap_utils

#endif /* PCAP_UTILS_PCAP_H */

