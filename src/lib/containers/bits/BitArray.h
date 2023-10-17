#pragma once

#include <cstdlib>
#include <cstdint>
#include <cassert>

/**
 * Header-only. No dependencies.
 *
 * BitArray template class implements an array of bits sized unsigned integer variables called Items.
 * Items are packed in a Chunk array with no bit gaps between them,
 * which means an Item is not a byte alignment structure.
 * The bit size of the Item must be in [1:BIT_WIDTH_LIMIT] range.
 *
 * Sample of packing 3-bit items on 8-bit chunks.
 *
 * iZ:X - X'th bit of the Z'th item.
 *
 *             | <----           8 bit chunk            ---- > |
 *   bit index |  7  |  6  |  5  |  4  |  3  |  2  |  1  |  0  |
 *             -------------------------------------------------
 * chunk index
 *           0 |i0:2 |i0:1 |i0:0 |i1:2 |i1:1 |i1:0 |i2:2 |i2:1 |
 *           1 |i2:0 |i3:2 |i3:1 |i3:0 |i4:2 |i4:1 |i4:0 |i5:2 |
 *           2 |i5:1 |i5:0 |i6:2 |i6:1 |i6:0 |i7:2 |i7:1 |i7:0 |
 * 			...
 *
 */

template<uint8_t BitWidth, typename Chunk = uint64_t>
class BitArray {

	using Width_t = uint8_t;

	// calculate the limit constants which are based on the types section
	static constexpr Width_t CHUNK_BYTE_WIDTH = sizeof(Chunk);
	static constexpr Width_t CHUNK_BIT_WIDTH = CHUNK_BYTE_WIDTH << 3ull;
	static constexpr Width_t BIT_WIDTH_MIN = 1u;
	static constexpr Width_t BIT_WIDTH_MAX = CHUNK_BIT_WIDTH - 1u;
	static constexpr size_t CHUNK_CAPACITY_MIN = 1ull;
	static constexpr size_t CHUNK_CAPACITY_MAX = (~(0ull)) / CHUNK_BIT_WIDTH;
	static constexpr size_t ITEM_CAPACITY_MIN = 1ull;
	static constexpr size_t ITEM_CAPACITY_MAX = (~(0ull)) / CHUNK_BIT_WIDTH;

	Chunk* m_chunks = nullptr;
	size_t m_capacity = 0;
	bool m_internal_mem = false;

public:
	BitArray(const BitArray&) = delete;
	BitArray(BitArray&&) = delete;

	BitArray& operator=(const BitArray&) = delete;
	BitArray& operator=(BitArray&&) = delete;

	BitArray() noexcept = default;

	~BitArray() noexcept {
		destroy();
	}

	/**
	 * Create a BitArraySeqT instance using an external memory space.
	 * @param buffer - An external memory array. MUST NOT be NULL.
	 * @param buffer_bytes - Size of @buffer in bytes. MUST be in [1:@buffer_bytes / CHUNK_WIDTH_BYTES]
 	*/
	void allocate(void* buffer, const size_t buffer_bytes) noexcept {
		static_assert(BitWidth >= BIT_WIDTH_MIN, "Template argument 'Width' must be greater than BIT_WIDTH_MIN.");
		static_assert(BitWidth <= BIT_WIDTH_MAX, "Template argument 'Width' must be less than BIT_WIDTH_MAX.");
		const size_t chunk_capacity = buffer_bytes / CHUNK_BYTE_WIDTH;

		assert(m_chunks == nullptr);
		assert(buffer);
		assert(chunk_capacity >= CHUNK_CAPACITY_MIN);
		assert(chunk_capacity <= CHUNK_CAPACITY_MAX);

		m_chunks = reinterpret_cast<Chunk*>(buffer);
		m_capacity = chunk_capacity * CHUNK_BIT_WIDTH / BitWidth;
	}

	/**
	 * Create a BitArraySeqT instance using its own memory space.
	 * @param item_capacity - BitArray size in items. MUST be greater than zero.
	 */
	void allocate(size_t item_capacity) noexcept {
		static_assert(BitWidth >= BIT_WIDTH_MIN, "Template argument 'Width' must be greater than BIT_WIDTH_MIN.");
		static_assert(BitWidth <= BIT_WIDTH_MAX, "Template argument 'Width' must be less than BIT_WIDTH_MAX.");

		assert(m_chunks == nullptr);
		assert(item_capacity >= ITEM_CAPACITY_MIN);
		assert(item_capacity <= ITEM_CAPACITY_MAX);

		const size_t bit_capacity = (BitWidth * item_capacity);
		size_t chunk_capacity = bit_capacity / CHUNK_BIT_WIDTH;
		chunk_capacity += (bit_capacity % CHUNK_BIT_WIDTH) ? 1u : 0u;
		m_chunks = new Chunk[chunk_capacity];
		assert(m_chunks);
		m_capacity = chunk_capacity * CHUNK_BIT_WIDTH / BitWidth;
		m_internal_mem = true;
	}

	void destroy() noexcept {
		if(m_internal_mem && m_chunks) {
			delete[] m_chunks;
		}
		m_chunks = nullptr;
		m_capacity = 0;
	}

	/**
	 * @return The array capacity in items.
	 */
	inline size_t capacity() const noexcept {
		return m_capacity;
	}

	/**
	 * Load @item_index item.
	 * @param item_index - MUST be in [0:items_capacity() - 1] range.
	 * @return - a loaded value in [0:range() - 1] range.
	 */
	Chunk load(const size_t item_index) const noexcept {
		const size_t bit_index = item_index * BitWidth;
		Width_t bits_left = BitWidth;
		Chunk value = 0;
		auto chunk_offset = size_t(bit_index / CHUNK_BIT_WIDTH);
		auto bit_offset = size_t(bit_index % CHUNK_BIT_WIDTH);
		while(bits_left) {
			bits_left -= load_bits(m_chunks[chunk_offset], bit_offset, bits_left, &value);
			chunk_offset++;
			bit_offset = 0;
		}
		return value;
	}

	/**
	 * Store @value to the @item_index'th index.
	 * @param item_index - MUST be in [0:items_capacity() - 1] range.
	 * @param value - MUST be in [0:range() - 1] range.
	 */
	void store(size_t item_index, Chunk value) noexcept {
		size_t bit_index = item_index * BitWidth;
		Width_t bits_left = BitWidth;
		auto chunk_offset = size_t(bit_index / CHUNK_BIT_WIDTH);
		auto bit_offset = size_t(bit_index % CHUNK_BIT_WIDTH);
		while(bits_left) {
			bits_left -= store_bits(m_chunks + chunk_offset, bit_offset, bits_left, value);
			chunk_offset++;
			bit_offset = 0;
		}
	}

	/**
	 * Fill the array with given value @value.
	 * @param value - value to fill with.
	 */
	void fill(Chunk value) noexcept {
		for(size_t i = 0; i < m_capacity; ++i) {
			store(i, value);
		}
	}

	/**
	 * The range limit of the item which the template can contain.
	 */
	static constexpr Chunk range() noexcept {
		return (1ull << BitWidth);
	}

	/**
	 * The maximum value that the template can contain.
	 */
	static constexpr Chunk value_max() noexcept {
		return range() - 1ull;
	}

private:

	/**
	 * Try to load @bit_width bits from @chunk_offset offset to @dst.
	 * @param chunk - a chunk to load from.
	 * @param chunk_offset - bit offset inside the chunk.
	 * @param dst - a value pointer to load to.
	 * @return - now many bits have been loaded from the chunk.
	 */
	static inline Width_t load_bits(
		const Chunk chunk
		, const size_t chunk_offset
		, const Width_t bit_width
		, Chunk* dst
	                               ) noexcept {
		const Width_t chunk_bits_available = CHUNK_BIT_WIDTH - chunk_offset;
		const Width_t bits_to_load = bit_width > chunk_bits_available ? chunk_bits_available : bit_width;
		const Width_t chunk_rshift = chunk_bits_available - bits_to_load;
		const Chunk chunk_mask = (~((~0ull) << bits_to_load));
		*dst <<= bits_to_load;
		*dst |= ((chunk >> chunk_rshift) & chunk_mask);
		return bits_to_load;
	}

	/**
	 * Try to store @bit_width bits to @chunk_offset offset from @src.
	 * @param bit_index - a bit offset in the array.
	 * @param bit_width - now many bits @src contains.
	 * @param src - source variable.
	 * @return - now many bits have been stored to the array.
	 */
	static inline Width_t store_bits(
		Chunk* chunk
		, const size_t chunk_offset
		, const Width_t bit_width
		, Chunk src
	                                ) noexcept {
		const Width_t chunk_bits_available = CHUNK_BIT_WIDTH - chunk_offset;
		const Width_t bits_to_store = bit_width > chunk_bits_available ? chunk_bits_available : bit_width;
		const Width_t src_rshift = bit_width - bits_to_store;
		const Width_t src_lshift = chunk_bits_available - bits_to_store;
		const Chunk mask = (~((~0ul) << bits_to_store)) << src_lshift;
		src >>= src_rshift;
		src <<= src_lshift;
		*chunk = ((*chunk) & ~mask) | (src & mask);
		return bits_to_store;
	}

};
