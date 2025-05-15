#pragma once

#include <cstdint>
#include <cstdio>
#include <string>

namespace fio {

class Writer {

	std::string _name;
	FILE* _file;

public:

	Writer(std::string name) noexcept : _name(std::move(name)), _file(nullptr) {}

	Writer(const Writer&) = delete;
	Writer& operator=(const Writer&) = delete;

	Writer(Writer&& rvalue) : _name(std::move(rvalue._name)), _file(rvalue._file) {
		rvalue.clear();
	}

	Writer& operator=(Writer&& rvalue) noexcept {
		if(this != &rvalue) {
			close();
			_file = rvalue._file;
			_name = std::move(rvalue._name);
			rvalue.clear();
		}
		return *this;
	}

	~Writer() noexcept {
		close();
	}

	bool open() {
		close();
		_file = fopen(_name.c_str(), "w");
		return _file != nullptr;
	}

	void close() noexcept {
		if(_file) {
			fclose(_file);
			_file = nullptr;
		}
	}

	bool write(const std::string& value, const char term = 0) noexcept {
		return (fwrite(value.data(), value.size(), 1u, _file) == 1u) && write(term);
	}

	bool write(const std::string_view& value, const char term = 0) noexcept {
		return (fwrite(value.data(), value.size(), 1u, _file) == 1u) && write(term);
	}

	template<typename V, std::enable_if_t<std::is_pod_v<V>, int> = 0>
	bool write(V& value) noexcept {
		return fwrite(&value, sizeof(value), 1u, _file) == 1u;
	}

	template<typename V, typename... Args>
	bool write(V& value, Args&... args) noexcept {
		return write(value) && write(args...);
	}

private:

	void clear() {
		_name.clear();
		_file = nullptr;
	}

};

}; // namespace pcapwrap
