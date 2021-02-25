#pragma once

#include "OutputStream.h"

#include <cstdlib>

namespace tio {

class ArrayOutputStream : public OutputStream {
	char* const m_array;
	const size_t m_array_nb;
	size_t m_offset;
public:

	template <typename T>
	ArrayOutputStream(T* array, size_t array_items) noexcept
		: m_array(reinterpret_cast<char*>(array))
		, m_array_nb(sizeof(*array) * array_items)
		, m_offset(0) { }

	inline bool write(char ch) noexcept override {
		if(m_offset < m_array_nb) {
			m_array[m_offset] = ch;
			m_offset++;
			return true;
		}
		return false;
	}

	virtual ~ArrayOutputStream() = default;
};

}; // namespace tio
