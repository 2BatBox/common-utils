#pragma once

#include <cstdlib>
#include <cstdint>

template <typename T, size_t Capacity>
class RingBuffer {

protected:

	T* _buffer;
	size_t _head;
	size_t _size;

public:

	RingBuffer(const T& default_value) noexcept :
		_buffer(new T[Capacity]),
		_head(0),
		_size(0) {}

	~RingBuffer() noexcept {
		delete [] _buffer;
	}

	inline constexpr auto capacity() const noexcept {
		return Capacity;
	}

	inline auto size() const noexcept {
		return _size;
	}

	inline bool is_full() const noexcept {
		return _size == Capacity;
	}

	inline const T& operator[](const size_t idx) const noexcept {
		return _buffer[(_head + idx) % Capacity];
	}

	inline T& operator[](const size_t idx) noexcept {
		return _buffer[(_head + idx) % Capacity];
	}

	inline void reset() noexcept {
		_head = 0;
		_size = 0;
	}

	void push_back(const T& value) noexcept {
		_buffer[_head] = value;
		_head++;
		if(_head >= Capacity){
			_head = 0;
		}
		if(_size < Capacity) {
			_size++;
		}
	}

};
