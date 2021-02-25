#pragma once

#include "InputStream.h"

#include <cstdio>

namespace tio {

class FileInputStream : public InputStream {
	FILE* m_stream;
	char m_char;
public:
	FileInputStream(FILE* stream) noexcept : m_stream(stream) {}

	inline char read() noexcept override {
		return m_char;
	}

	inline bool next() noexcept override {
		const auto ret = getc(m_stream);
		if(ret >= 0) {
			m_char = char(ret);
			return true;
		}
		return false;
	}

	virtual ~FileInputStream() = default;
};

}; // namespace tio
