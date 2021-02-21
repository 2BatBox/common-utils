#pragma once

#include <pcap.h>
#include <cstring>
#include <cstdint>

namespace pcapwrap {

struct Frame {
	struct pcap_pkthdr m_hdr;
	const uint8_t* m_data;
	uint64_t m_idx;

	Frame() noexcept : m_hdr(), m_data(nullptr), m_idx(0) {}

	Frame(const struct pcap_pkthdr& hdr, const uint8_t* data) noexcept : m_hdr(hdr), m_data(data), m_idx(0) {}

	virtual ~Frame() noexcept {};

	inline uint64_t nanosec() const noexcept {
		uint64_t result = m_hdr.ts.tv_sec * uint64_t(1000000000) + m_hdr.ts.tv_usec;
		return uint64_t(result);
	}

	inline void nanosec(uint64_t value) noexcept {
		m_hdr.ts.tv_sec = value / uint64_t(1000000000);
		m_hdr.ts.tv_usec = value % uint64_t(1000000000);
	}
};

}; // namespace pcapwrap

