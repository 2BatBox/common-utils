#pragma once

#include <cstdlib>
#include <cstdint>
#include <memory>

/**
 * RingArrayBuffer design overview.
 *
 * Array payload:
 * 		CC - Items to clear (set to user defined value).
 * 		.. - Untouched items.
 *
 *
 *  Moving head forward.
 *  sample:
 *  assert(sa.capacity() == 10);
 *  assert(sa.head() == 34);
 *  assert(sa.tail() == 45);
 *  sa.head(38);
 *  assert(sa.capacity() == 10);
 *  assert(sa.head() == 38);
 *  assert(sa.tail() == 49);
 *
 *
 * (case 0)
 *               old head
 *                  |---------->
 *   |..|..|..|..|..|..|..|..|..|..|..|
 *   |40|41|42|43|44|34|35|36|37|38|39|
 *
 *                           new head
 *                              |
 *   |..|..|..|..|..|CC|CC|CC|CC|..|..|
 *   |40|41|42|43|44|45|46|47|48|38|39|
 *
 * (case 1)
 *                           old head
 *   ----->                     |------
 *   |..|..|..|..|..|..|..|..|..|..|..|
 *   |36|37|38|39|40|41|42|43|44|34|35|
 *
 *      new head
 *         |
 *   |CC|CC|..|..|..|..|..|..|..|CC|CC|
 *   |47|48|38|39|40|41|42|43|44|45|46|
 *
 *
 *
 *  Moving head backward.
 *  sample:
 *  assert(sa.capacity() == 10);
 *  assert(sa.head() == 38);
 *  assert(sa.tail() == 49);
 *  sa.head(34);
 *  assert(sa.capacity() == 10);
 *  assert(sa.head() == 34);
 *  assert(sa.tail() == 45);
 *
 *
 * (case 0)
 *                           old head
 *                   <----------|
 *   |..|..|..|..|..|..|..|..|..|..|..|
 *   |40|41|42|43|44|45|46|47|48|38|39|
 *
 *               new head
 *                  |
 *   |..|..|..|..|..|CC|CC|CC|CC|..|..|
 *   |40|41|42|43|44|34|35|36|37|38|39|
 *
 *
 * (case 1)
 *      old head
 *   ------|                     <-----
 *   |..|..|..|..|..|..|..|..|..|..|..|
 *   |47|48|38|39|40|41|42|43|44|45|46|
 *
 *                           new head
 *                              |
 *   |CC|CC|..|..|..|..|..|..|..|CC|CC|
 *   |36|37|38|39|40|41|42|43|44|34|35|
 *
 *
 **/

template <typename T, size_t Capacity>
class RingArrayBuffer {

protected:

	std::unique_ptr<T[]> _buffer;
	size_t _head;

public:

	RingArrayBuffer(const T& default_value) noexcept :
		_buffer(new T[Capacity]),
		_head(0)
	{
		fill_all(default_value);
	}

	inline constexpr size_t capacity() const noexcept {
		return Capacity;
	}

	/**
	 * @return The head offset limit.
	 */
	inline constexpr size_t head_max() const noexcept {
		return SIZE_MAX - Capacity;
	}

	/**
	 * @return current head position.
	 */
	inline size_t head() const noexcept {
		return _head;
	}

	/**
	 * @return current tail position (the position next after the last element in the array).
	 */
	inline size_t tail() const noexcept {
		return _head + Capacity;
	}

	/**
	 * @param idx
	 * @return true if 'idx' within [head,tail-1] range.
	 */
	inline bool in_bounds(const size_t idx) const noexcept {
		return (idx >= head() && idx < tail());
	}

	/**
	 * Moves head to the new position.
	 * @param head_new - MUST NOT exceed 'head_max()' value.
	 * @param default_value - The value to fill all the not initialized elements in the array.
	 */
	void head(size_t head_new, const T& default_value) noexcept {
		if(head_new > _head) {
			fill(head_new - _head, default_value);
			_head = head_new;
		} else {
			_head = head_new;
			fill(_head - head_new, default_value);
		}
	}

	inline const T& operator[](const size_t idx) const noexcept {
		return _buffer[idx % Capacity];
	}

	inline T& operator[](const size_t idx) noexcept {
		return _buffer[idx % Capacity];
	}

	inline void fill(const size_t offset, const T& empty) noexcept {
		if(offset < Capacity) {
			fill_offset(offset, empty);
		} else {
			fill_all(empty);
		}
	}

private:

	inline void fill_all(const T& empty) noexcept {
		size_t idx = 0;
		while(idx < Capacity) {
			_buffer[idx] = empty;
			idx++;
		}
	}

	inline void fill_offset(size_t offset, const T& empty) noexcept {
		size_t idx = 0;
		while(offset--) {
			_buffer[(_head + idx) % Capacity] = empty;
			idx++;
		}
	}

};
