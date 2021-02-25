#pragma once

#include "OutputStream.h"

#include <cstdio>

namespace tio {

class FileOutputStream : public OutputStream {
	FILE* m_stream;
public:
	FileOutputStream(FILE* stream) noexcept : m_stream(stream) {}

	inline bool write(char ch) noexcept override {
		return putc(ch, m_stream) >= 0;
	}

	virtual ~FileOutputStream() = default;
};

}; // namespace tio

