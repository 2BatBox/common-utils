#ifndef PCAP_UTILS_WRITER_H
#define PCAP_UTILS_WRITER_H

#include <exception>
#include <stdexcept>
#include <pcap.h>

#include "Pcap.h"

namespace pcap_utils {

class Writer {
	constexpr static uint16_t SNAPSHOT_LEN = 0xFFFF;

	pcap_dumper_t* pcap_dumper;
	unsigned frame_index;

	Writer(pcap_dumper_t* pcap_handler = nullptr) noexcept : pcap_dumper(pcap_handler), frame_index(0) {}

public:
	Writer(const Writer&) = delete;
	Writer& operator=(const Writer&) = delete;

	Writer(Writer&& rvalue) noexcept : pcap_dumper(rvalue.pcap_dumper), frame_index(rvalue.frame_index) {
		rvalue.make_empty();
	}

	Writer& operator=(Writer&& rvalue) noexcept {
		if(this != &rvalue) {
			close();
			pcap_dumper = rvalue.pcap_dumper;
			frame_index = rvalue.frame_index;
			rvalue.make_empty();
		}
		return *this;
	}

	~Writer() noexcept {
		close();
	}

	inline void close() noexcept {
		if(pcap_dumper) {
			pcap_dump_close(pcap_dumper);
			make_empty();
		}
	}

	inline void write_next(const Frame& frame) noexcept {
		pcap_dump((u_char*) pcap_dumper, &(frame.hdr), (const u_char*) frame.data);
		frame_index++;
	}

	inline unsigned next_index() const noexcept {
		return frame_index;
	}

	static Writer open(const char* file_name) throw(std::logic_error) {
		char error_buffer[PCAP_ERRBUF_SIZE];
		pcap_t* pcap_handler = pcap_open_dead(DLT_EN10MB, SNAPSHOT_LEN);
		pcap_dumper_t* result = pcap_dump_open(pcap_handler, file_name);
		if(result == nullptr) {
			throw std::logic_error(error_buffer);
		}
		return Writer(result);
	}

private:

	inline void make_empty() noexcept {
		pcap_dumper = nullptr;
		frame_index = 0;
	}

};

}; // namespace pcap_utils

#endif /* PCAP_UTILS_WRITER_H */