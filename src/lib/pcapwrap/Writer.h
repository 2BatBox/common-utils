#pragma once

#include "Frame.h"

#include <exception>
#include <stdexcept>

namespace pcapwrap {

class Writer {
	constexpr static uint16_t SNAPSHOT_LEN = 0xFFFF;

	pcap_dumper_t* m_pcap_hnd;
	uint64_t m_frame_idx;

	explicit Writer(pcap_dumper_t* pcap_handler) noexcept : m_pcap_hnd(pcap_handler), m_frame_idx(0) {}

public:
	Writer(const Writer&) = delete;
	Writer& operator=(const Writer&) = delete;

	Writer(Writer&& rvalue) noexcept : m_pcap_hnd(rvalue.m_pcap_hnd), m_frame_idx(rvalue.m_frame_idx) {
		rvalue.clear();
	}

	Writer& operator=(Writer&& rvalue) noexcept {
		if(this != &rvalue) {
			close();
			m_pcap_hnd = rvalue.m_pcap_hnd;
			m_frame_idx = rvalue.m_frame_idx;
			rvalue.clear();
		}
		return *this;
	}

	~Writer() noexcept {
		close();
	}

	inline void close() noexcept {
		if(m_pcap_hnd) {
			pcap_dump_close(m_pcap_hnd);
			clear();
		}
	}

	inline void write(const Frame& frame) noexcept {
		pcap_dump((u_char*) m_pcap_hnd, &(frame.m_hdr), frame.m_data);
		m_frame_idx++;
	}

	inline uint64_t frame_index() const noexcept {
		return m_frame_idx;
	}

	static Writer open(const std::string& file_name) noexcept(false) {
		char error_buffer[PCAP_ERRBUF_SIZE];
		auto pcap_handler = pcap_open_dead_with_tstamp_precision(DLT_EN10MB, SNAPSHOT_LEN, PCAP_TSTAMP_PRECISION_NANO);
		auto result = pcap_dump_open(pcap_handler, file_name.c_str());
		if(result == nullptr) {
			throw std::runtime_error(error_buffer);
		}
		return Writer(result);
	}

private:

	inline void clear() noexcept {
		m_pcap_hnd = nullptr;
		m_frame_idx = 0;
	}

};

}; // namespace pcapwrap
