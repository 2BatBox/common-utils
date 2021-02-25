#pragma once

namespace tio {

class OutputStream {
public:
	virtual bool write(char ch) noexcept = 0;
	virtual ~OutputStream() = default;
};

}; // namespace tio
