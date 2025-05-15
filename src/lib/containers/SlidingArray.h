#pragma once

#include <cstdlib>

#include <memory>

/**
 * SlidingArray design overview.
 *
 *
 *                                 |<-   capacity    ->|
 *                     |<-   capacity    ->|
 *                     | head-room | array | tail-room |
 * | | | | | | **. | | | | | | | | | | | | | | | | | | | | | | **. | | | | | | |  <- virtual index
 * |                               |       |                                   |
 * 0                              head    tail                              MAX_SIZE
 *
 *
 * Array payload:
 * 		UD - Valid User Data.
 * 		** - Garbage.
 * 		cc - Items to clear (set to user defined value).
 *
 *  Samples:
 *
 *  ==== Case 0 : Moving head forward. ====
 *             head(34)
 *               |---------->
 *   |UD|UD|UD|UD|UD|UD|UD|UD|UD|UD| <- array
 *   | 0| 1| 2| 3| 4| 5| 6| 7| 8| 9| <- array index
 *   |40|41|42|43|34|35|36|37|38|39| <- sliding index
 *               |
 *             tail(44)
 *
 *    rab.head_move_forward(4);
 *
 *                         head(38)
 *                           |
 *   |UD|UD|UD|UD|**|**|**|**|UD|UD|
 *   | 0| 1| 2| 3| 4| 5| 6| 7| 8| 9|
 *   |40|41|42|43|  |  |  |  |38|39|
 *               |
 *             tail(44)
 *
 *  ==== Case 1 : Moving head forward. ====
 *                         head(38)
 *   ----->                  |------
 *   |UD|UD|UD|UD|UD|**|**|**|UD|UD|
 *   | 0| 1| 2| 3| 4| 5| 6| 7| 8| 9|
 *   |40|41|42|43|44|  |  |  |38|39|
 *                  |
 *                tail(45)
 *
 *    rab.head_move_forward(4);
 *
 *       head(42)
 *         |
 *   |**|**|UD|UD|UD|**|**|**|**|**|
 *   | 0| 1| 2| 3| 4| 5| 6| 7| 8| 9|
 *   |  |  |42|43|44|  |  |  |  |  |
 *                  |
 *                tail(45)
 *
  *  ==== Case 2 : Moving head forward. ====
 *                         head(38)
 *   -------->               |------
 *   |UD|**|**|**|**|**|**|**|UD|UD|
 *   | 0| 1| 2| 3| 4| 5| 6| 7| 8| 9|
 *   |40|  |  |  |  |  |  |  |38|39|
 *      |
 *    tail(41)
 *
 *    rab.head_move_forward(5);
 *
 *       head(42)
 *         |
 *   |**|**|**|**|**|**|**|**|**|**|
 *   | 0| 1| 2| 3| 4| 5| 6| 7| 8| 9|
 *   |  |  |  |  |  |  |  |  |  |  |
 *         |
 *       tail(43)
 *
 *
 *  ==== Case 3 : Moving head backward. ====
 *                         head(78)
 *             <-------------|
 *   |UD|UD|UD|UD|UD|UD|UD|UD|UD|UD|
 *   | 0| 1| 2| 3| 4| 5| 6| 7| 8| 9|
 *   |80|81|82|83|84|85|86|87|78|79|
 *                           |
 *                         tail(88)
 *                         
 *  rab.head_move_backward(5);
 *
 *          head(73)
 *            |
 *   |UD|UD|UD|cc|cc|cc|cc|cc|UD|UD|
 *   | 0| 1| 2| 3| 4| 5| 6| 7| 8| 9|
 *   |80|81|82|73|74|75|76|77|78|79|
 *            |
 *          tail(83)
 *
 *  ==== Case 4 : Moving head backward. ====
 *          head(73)
 *   ---------|               <-----
 *   |**|**|**|UD|UD|**|**|**|**|**|
 *   | 0| 1| 2| 3| 4| 5| 6| 7| 8| 9|
 *   |  |  |  |73|74|  |  |  |  |  |
 *                  |
 *                tail(75)
 *
 *  rab.head_move_backward(5);
 *
 *                          head(68)
 *                           |
 *   |cc|cc|cc|UD|UD|**|**|**|cc|cc|
 *   | 0| 1| 2| 3| 4| 5| 6| 7| 8| 9|
 *   |70|71|72|73|74|  |  |  |68|69|
 *                  |
 *                tail(75)
 *
 *  ==== Case 5 : Moving head backward. ====
 *          head(73)
 *   ---------|<--------------------
 *   |**|**|**|UD|UD|UD|UD|UD|UD|**|
 *   | 0| 1| 2| 3| 4| 5| 6| 7| 8| 9|
 *   |  |  |  |73|74|75|76|77|78|  |
 *                              |
 *                             tail(79)
 *
 *  rab.head_move_backward(60);
 *
 *         head(13)
 *            |
 *   |cc|cc|cc|cc|cc|cc|cc|cc|cc|cc|
 *   | 0| 1| 2| 3| 4| 5| 6| 7| 8| 9|
 *   |20|21|22|13|14|15|16|17|18|19|
 *            |
 *          tail(23)
 *
 **/

namespace utils {

template <typename T, size_t Capacity, typename A = std::allocator<T> >
class SlidingArray {

protected:

	const T _def_value;
	A _allocator;
	T* _buffer;
	size_t _head;
	size_t _size;

public:

	SlidingArray(const T& default_value) noexcept :
		_def_value(default_value),
		_allocator(),
		_buffer(_allocator.allocate(Capacity)),
		_head(0),
		_size(0)
	{}

	SlidingArray(const SlidingArray&) = delete;
	SlidingArray& operator=(const SlidingArray&) = delete;

	SlidingArray(SlidingArray&& rv) = delete;
	SlidingArray& operator=(SlidingArray&&) = delete;

	~SlidingArray() noexcept {
		for(size_t i = 0; i < Capacity; ++i) {
			_allocator.destroy(_buffer + i);
		}
		_allocator.deallocate(_buffer, Capacity);
		_buffer = nullptr;
	}

	/**
	 * @return How many user data items the array can contain.
	 */
	inline constexpr size_t capacity() const noexcept {
		return Capacity;
	}

	/**
	 * @return How many user data items the array contains at the moment.
	 */
	inline size_t size() const noexcept {
		return _size;
	}

	/**
	 * @return Current head position.
	 */
	inline size_t head() const noexcept {
		return _head;
	}

	/**
	 * @return Current tail position (the position next after the last element in the array).
	 */
	inline size_t tail() const noexcept {
		return _head + _size;
	}

	/**
	 * @return How many new data items may be put in front of the array without losing any user data items.
	 */
	inline size_t head_room() const noexcept {
		return std::min(_head, Capacity - _size);
	}

	/**
	 * @return How many new data items may be put in back of the array without losing any user data items.
	 */
	inline size_t tail_room() const noexcept {
		return std::min(SIZE_MAX - tail(), Capacity - _size);
	}

	/**
	 * @return true if 'idx' is within [head,tail-1] range.
	 */
	inline bool in_bounds(const size_t idx) const noexcept {
		return (idx >= head()) && (idx < tail());
	}

	/**
	 * The only getter with bounds checking.
	 */
	inline const T& get(const size_t idx) const noexcept {
		return in_bounds(idx) ? _buffer[idx % Capacity] : _def_value;
	}

	/**
	 * The getters without bounds checking.
	 */
	inline const T& operator[](const size_t idx) const noexcept {
		return _buffer[idx % Capacity];
	}

	inline T& operator[](const size_t idx) noexcept {
		return _buffer[idx % Capacity];
	}

	inline T& front() const noexcept {
		return _buffer[_head % Capacity];
	}

	inline T& back() const noexcept {
		return _buffer[(tail() - 1u)  % Capacity];
	}

	void initialize(const size_t head, const size_t size) noexcept {
		_head = head;
		_size = 0;
		_size = std::min(size, tail_room());
		clear_offset(head, _size);
	}

	/**
	 * Moves head forward. May affect the tail position.
	 */
	void head_move_forward(size_t offset) noexcept {
		offset = std::min(SIZE_MAX - _head, offset);
		const auto items_to_forget = std::min(_size, offset);
		_head += offset;
		_size -= items_to_forget;
	}

	/**
	 * Moves head forward until the tail is not affected.
	 */
	void head_move_forward_available(size_t offset) noexcept {
		offset = std::min(_size, offset);
		_head += offset;
		_size -= offset;
	}

	/**
	 * Moves head backward. May affect the tail position.
	 */
	void head_move_backward(size_t offset) noexcept {
		offset = std::min(_head, offset);
		const auto avail = std::min(Capacity - _size, offset);
		_head -= offset;
		_size += avail;
		if(offset > Capacity) {
			clear_all();
		} else {
			clear_offset(_head, offset);
		}
	}

	/**
	 * Moves head backward until the tail is not affected.
	 */
	void head_move_backward_available(size_t offset) noexcept {
		offset = std::min(offset, head_room());
		_head -= offset;
		_size += offset;
		clear_offset(_head, offset);
	}

	/**
	 * Moves tail forward. May affect the head position.
	 */
	void tail_move_forward(size_t offset) noexcept {
		offset = std::min(SIZE_MAX - tail(), offset);
		if(offset > Capacity) {
			clear_all();
		} else {
			clear_offset(tail(), offset);
		}

		auto avail = std::min(offset, tail_room());
		_size += avail;
		offset -= avail;
		_head += offset;
	}

	/**
	 * Moves tail forward until the head is not affected
	 */
	void tail_move_forward_available(size_t offset) noexcept {
		offset = std::min(offset, tail_room());
		clear_offset(tail(), offset);
		_size += offset;
	}

	/**
	 * Moves tail backward. May affect the head position.
	 */
	void tail_move_backward(size_t offset) noexcept {
		offset = std::min(tail(), offset);
		const auto available = std::min(_size, offset);
		_size -= available;
		offset -= available;
		_head -= offset;
	}

	/**
	 * Moves tail backward until the head is not affected
	 */
	void tail_move_backward_available(size_t offset) noexcept {
		const auto avail = std::min(_size, offset);
		_size -= avail;
	}

private:

	inline void clear_all() noexcept {
		size_t idx = 0;
		while(idx < Capacity) {
			_buffer[idx] = _def_value;
			idx++;
		}
	}

	inline void clear_offset(const size_t head, const size_t offset) noexcept {
		const size_t tail = head + offset;
		for(size_t idx = head; idx < tail; ++idx) {
			_buffer[idx % Capacity] = _def_value;
		}
	}

};

}; // namespace utils
