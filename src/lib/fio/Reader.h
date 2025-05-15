#pragma once

#include <cstdint>
#include <cstdio>
#include <string>

namespace fio {

class Reader {

	std::string _name;
	FILE* _file;

public:

	Reader(std::string name) noexcept : _name(std::move(name)), _file(nullptr) {}

	Reader(const Reader&) = delete;
	Reader& operator=(const Reader&) = delete;

	Reader(Reader&& rvalue) : _name(std::move(rvalue._name)), _file(rvalue._file) {
		rvalue.clear();
	}

	Reader& operator=(Reader&& rvalue) noexcept {
		if(this != &rvalue) {
			close();
			_file = rvalue._file;
			_name = std::move(rvalue._name);
			rvalue.clear();
		}
		return *this;
	}

	~Reader() noexcept {
		close();
	}

	bool open() {
		close();
		_file = fopen(_name.c_str(), "r");
		return _file != nullptr;
	}

	void close() noexcept {
		if(_file) {
			fclose(_file);
			_file = nullptr;
		}
	}

	bool read(std::string& value, const char term = 0) noexcept {
		value.resize(0);
		int ch;
		while((ch = fgetc(_file)) != EOF) {
			if(ch == term) {
				break;
			}
			value.push_back(ch);
		}
		return ch == 0;
	}

	template<typename V, std::enable_if_t<std::is_pod_v<V>, int> = 0>
	bool read(V& value) noexcept {
		return read_impl(&value, sizeof(value));
	}

	template<typename V, typename... Args>
	bool read(V& value, Args&... args) noexcept {
		return read(value) && read(args...);
	}

private:

	bool read_impl(void* buf, size_t buf_nb) noexcept {
		return fread(buf, buf_nb, 1u, _file) == 1u;
	}

	void clear() {
		_name.clear();
		_file = nullptr;
	}

};

}; // namespace pcapwrap
