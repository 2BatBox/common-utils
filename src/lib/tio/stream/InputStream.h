#pragma once

namespace tio {

class InputStream {
public:
	virtual char read() noexcept = 0;
	virtual bool next() noexcept = 0;
	virtual ~InputStream() = default;

};

}; // namespace tio
