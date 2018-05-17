#ifndef PCAP_UTILS_READER_H
#define PCAP_UTILS_READER_H

#include <exception>
#include <stdexcept>

#include "Pcap.h"

namespace pcap_utils {

class Reader {
	pcap_t* pcap_handler;
	unsigned frame_index;

	Reader(pcap_t* pcap_handler = nullptr) noexcept: pcap_handler(pcap_handler), frame_index(0) { }

public:
	Reader(const Reader&) = delete;
	Reader& operator=(const Reader&) = delete;

	Reader(Reader&& rvalue) noexcept: pcap_handler(rvalue.pcap_handler), frame_index(rvalue.frame_index) {
		rvalue.make_empty();
	}

	Reader& operator=(Reader&& rvalue) noexcept {
		if (this != &rvalue) {
			close();
			pcap_handler = rvalue.pcap_handler;
			frame_index = rvalue.frame_index;
			rvalue.make_empty();
		}
		return *this;
	}

	~Reader() noexcept {
		close();
	}

	inline void close() noexcept {
		if (pcap_handler) {
			pcap_close(pcap_handler);
			make_empty();
		}
	}

	inline bool read_next(Frame& frame) noexcept {
		frame.data = pcap_next(pcap_handler, &(frame.hdr));
		if (frame.data) {
			frame_index++;
		}
		return (frame.data != nullptr);
	}

	inline unsigned next_index() const noexcept {
		return frame_index;
	}

	static Reader open(const char* file_name) throw (std::logic_error) {
		char error_buffer[PCAP_ERRBUF_SIZE];
		pcap_t* pcap_handler = pcap_open_offline(file_name, error_buffer);
		if (pcap_handler == nullptr) {
			throw std::logic_error(error_buffer);
		}
		return Reader(pcap_handler);
	}
    
private:
    
    inline void make_empty() noexcept {
        pcap_handler = nullptr;
		frame_index = 0;
    }

};

}; // namespace pcap_utils

#endif /* PCAP_UTILS_READER_H */