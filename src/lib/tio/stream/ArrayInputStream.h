#pragma once

#include "InputStream.h"

#include <cstdlib>
#include <cstring>

namespace tio {

class ArrayInputStream : public InputStream {
	const char* const m_array;
	const size_t m_array_nb;
	size_t m_offset;
	char m_char;
public:

	template<typename T>
	ArrayInputStream(const T* array, size_t array_items) noexcept
		: m_array(reinterpret_cast<const char*>(array))
		, m_array_nb(sizeof(*array) * array_items)
		, m_offset(0)
		, m_char(0) {}

	ArrayInputStream(const char* str) noexcept
		: m_array(str)
		, m_array_nb(strlen(str))
		, m_offset(0)
		, m_char(0) {}

	inline char read() noexcept override {
		return m_char;
	}

	inline bool next() noexcept override {
		if(m_offset < m_array_nb) {
			m_char = m_array[m_offset];
			m_offset++;
			return true;
		}
		return false;
	}

	virtual ~ArrayInputStream() = default;
};

}; // namespace tio
