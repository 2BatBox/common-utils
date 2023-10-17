#pragma once

#include <cstdlib>

/**
 * RangeBuffer design overview.
 *
 * An instance of RangeBuffer class represents a memory area divided into three subareas.
 * The RangeBuffer provides bounds checking.
 *
 *
 * Payload examples:
 * 		D - User data.
 * 		. - Empty space.
 *
 * (_size == 0)
 *
 *                   |
 *   |               |                                 |
 *   |.|.|.|.|.|.|.|.|.|.|.|.|.|.|.|.|.|.|.|.|.|.|.|.|.|
 *   | <----------------- _capacity -----------------> |
 * _buffer
 *
 *
 *
 * (_size > 0 && _size < _capacity)
 *                 head
 *                   |                 |
 *   |               |< --- size  --- >|               |
 *   |.|.|.|.|.|.|.|.|D|D|D|D|D|D|D|D|D|.|.|.|.|.|.|.|.|
 *   | <----------------- _capacity -----------------> |
 * _buffer
 *
 *
 *
 * (_size > 0 && _size < _capacity)
 *                                    head
 *                   |                 |
 *   ----- size ---->|                 |< --- size -----
 *   |D|D|D|D|D|D|D|D|.|.|.|.|.|.|.|.|.|D|D|D|D|D|D|D|D|
 *   | <----------------- _capacity -----------------> |
 * _buffer
 *
 *
 *
 * (_size == _capacity)

 *                 head
 *                   |
 *   ----- size ---->|< ------------- size -------------
 *   |D|D|D|D|D|D|D|D|D|D|D|D|D|D|D|D|D|D|D|D|D|D|D|D|D|
 *   | <----------------- _capacity -----------------> |
 * _buffer
 *
 * The RangeBuffer memory buffer is divided into two subareas 'used' and 'empty space'.
 *
 * Moving 'head' and 'tail' points affects the subareas they start or end with.
 *
 **/
template <typename T>
class RangeBuffer {

protected:

	T* _buffer;
	const T _default_value;
	const size_t _capacity;
	const size_t _max_head_index;
	size_t _head;
	size_t _size;
	size_t _user_offset;
	T _dummy_empty;

public:

	RangeBuffer(size_t capacity, T empty) noexcept :
		_buffer(new T[capacity]),
		_default_value(empty),
		_capacity(capacity),
		_max_head_index(SIZE_MAX - _capacity),
		_head(0),
		_size(0),
		_user_offset(0)
	{}

	~RangeBuffer() noexcept {
		delete [] _buffer;
	}

	inline size_t capacity() const noexcept {
		return _capacity;
	}

	inline size_t size() const noexcept {
		return _size;
	}

	inline size_t max_head_index() const noexcept {
		return _max_head_index;
	}

	inline size_t head_index() const noexcept {
		return _user_offset;
	}

	inline size_t tail_index() const noexcept {
		return _user_offset + _size;
	}

	inline bool in_bounds(const size_t idx) const noexcept {
		return (idx >= head_index() && idx < tail_index());
	}

	inline const T& front() const noexcept {
		return _buffer[_head];
	}

	inline T& front() noexcept {
		return _buffer[_head];
	}


	/**
	 * Moves head to the new position.
	 * @param new_head_offset - MUST NOT exceed 'max_user_offset()' value.
	 */
	void head_index(size_t new_head_offset) noexcept {
		if(new_head_offset > _max_head_index) {
			new_head_offset = _max_head_index;
		}
		if(new_head_offset > _user_offset) {
			head_move_right(new_head_offset - _user_offset);
		} else {
			head_move_left(_user_offset - new_head_offset);
		}
		_user_offset = new_head_offset;
	}

	/**
	 * Resizes the buffer.
	 * @param new_size - MUST NOT exceed 'capacity()' value.
	 */
	void resize(size_t new_size) noexcept {
		if(new_size > _capacity) {
			new_size = _capacity;
		}
		if(new_size > _size) {
			clear(_head + _size, new_size - _size);
		}
		_size = new_size;
	}

	inline const T& operator[](const size_t idx) const noexcept {
		if(in_bounds(idx)) {
			const auto offset = idx - _user_offset;
			return _buffer[(_head + offset) % _capacity];
		} else {
			return _default_value;
		}
	}

	inline T& operator[](const size_t idx) noexcept {
		if(in_bounds(idx)) {
			const auto offset = idx - _user_offset;
			return _buffer[(_head + offset) % _capacity];
		} else {
			_dummy_empty = _default_value;
			return _dummy_empty;
		}
	}

private:

	inline void clear() noexcept {
		size_t idx = 0;
		while(idx < _capacity) {
			_buffer[idx] = _default_value;
			idx++;
		}
	}

	inline void clear(size_t idx_begin, size_t offset) noexcept {
		while(offset--) {
			_buffer[idx_begin] = _default_value;
			idx_begin++;
			if(idx_begin >= _capacity) {
				idx_begin = 0;
			}
		}
	}

	inline void head_move_right(size_t offset) noexcept {
		if(offset < _size) {
			_head = (_head + offset) % _capacity;
			_size -= offset;
		} else {
			_size = 0;
		}
	}

	inline void head_move_left(size_t offset) noexcept {
		if(offset < _capacity) {
			const auto empty = _capacity - _size;
			_head = (_head + _capacity - offset) % _capacity;
			_size += (offset < empty) ? offset : empty;
			clear(_head, offset);
		} else {
			_size = _capacity;
			clear();
		}
	}

};
