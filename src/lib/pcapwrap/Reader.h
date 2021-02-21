#pragma once

#include "Frame.h"

#include <exception>
#include <stdexcept>

namespace pcapwrap {

class Reader {
	pcap_t* m_pcap_hnd;
	uint64_t m_frame_idx;

	explicit Reader(pcap_t* handler) noexcept : m_pcap_hnd(handler), m_frame_idx(0) {}

public:
	Reader(const Reader&) = delete;
	Reader& operator=(const Reader&) = delete;

	Reader(Reader&& rvalue) noexcept : m_pcap_hnd(rvalue.m_pcap_hnd), m_frame_idx(rvalue.m_frame_idx) {
		rvalue.clear();
	}

	Reader& operator=(Reader&& rvalue) noexcept {
		if(this != &rvalue) {
			close();
			m_pcap_hnd = rvalue.m_pcap_hnd;
			m_frame_idx = rvalue.m_frame_idx;
			rvalue.clear();
		}
		return *this;
	}

	~Reader() noexcept {
		close();
	}

	inline void close() noexcept {
		if(m_pcap_hnd) {
			pcap_close(m_pcap_hnd);
			clear();
		}
	}

	inline bool next(Frame& frame) noexcept {
		frame.m_data = pcap_next(m_pcap_hnd, &(frame.m_hdr));
		if(frame.m_data) {
			m_frame_idx++;
			frame.m_idx = m_frame_idx;
		}
		return (frame.m_data != nullptr);
	}

	inline uint64_t frame_index() const noexcept {
		return m_frame_idx;
	}

	static Reader open(const std::string& file_name) noexcept(false) {
		char error_buffer[PCAP_ERRBUF_SIZE];
		auto pcap_handler = pcap_open_offline_with_tstamp_precision(file_name.c_str(), PCAP_TSTAMP_PRECISION_NANO, error_buffer);
		if(pcap_handler == nullptr) {
			throw std::runtime_error(error_buffer);
		}
		return Reader(pcap_handler);
	}

private:

	inline void clear() noexcept {
		m_pcap_hnd = nullptr;
		m_frame_idx = 0;
	}

};

}; // namespace pcapwrap
