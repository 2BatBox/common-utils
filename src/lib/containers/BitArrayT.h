#pragma once

#include "cstdlib"
#include "cstdint"
#include "cassert"

/**
 * Header-only. No dependencies.
 *
 * BitArrayT template class implements an array of bits sized unsigned integer variables called Items.
 * Items are packed in in a Chunk array with no gaps between them,
 * which means an Item is not a byte alignment structure.
 * The bit size of the Item must be in [1:BIT_WIDTH_LIMIT] range.
 *
 * Sample of packing 3-bit items on 8-bit chunks.
 *
 * iZ:X - X'th bit of the Z'th item.
 * chunk index | <----           8 bit chunk            ---- > |
 *           0 |i3:2 |i3:1 |i1:2 |i1:1 |i1:0 |i0:2 |i0:1 |i0:0 |
 *           1 |i6:2 |i5:2 |i5:1 |i5:0 |i4:2 |i4:1 |i4:0 |i3:0 |
 *           2 |i8:2 |i8:1 |i8:0 |i7:2 |i7:1 |i7:0 |i6:1 |i6:0 |
 * 			...
 *
 */

// type definition section
using BitArrayTChunk_t = uint64_t;
using BitArrayTWidth_t = uint8_t;

template<BitArrayTWidth_t Width>
class BitArrayT {

	// short aliases
	using Chunk_t = BitArrayTChunk_t;
	using Width_t = BitArrayTWidth_t;

	// calculate the limit constants which are based on the types section
	static constexpr Width_t CHUNK_BYTE_WIDTH = sizeof(Chunk_t);
	static constexpr Width_t CHUNK_BIT_WIDTH = CHUNK_BYTE_WIDTH << 3ull;
	static constexpr Width_t BIT_WIDTH_MIN = 1u;
	static constexpr Width_t BIT_WIDTH_MAX = CHUNK_BIT_WIDTH - 1u;
	static constexpr size_t CHUNK_CAPACITY_MIN = 1ull;
	static constexpr size_t CHUNK_CAPACITY_MAX = (~(0ull)) / CHUNK_BIT_WIDTH;
	static constexpr size_t ITEM_CAPACITY_MIN = 1ull;
	static constexpr size_t ITEM_CAPACITY_MAX = (~(0ull)) / CHUNK_BIT_WIDTH;

	Chunk_t* m_chunks = nullptr;
	size_t m_chunks_capacity = 0;
	size_t m_items_capacity = 0;
	bool m_internal_mem = false;

public:
	BitArrayT(const BitArrayT&) = delete;
	BitArrayT(BitArrayT&&) = delete;

	BitArrayT& operator=(const BitArrayT&) = delete;
	BitArrayT& operator=(BitArrayT&&) = delete;

	BitArrayT() noexcept = default;

	~BitArrayT() noexcept {
		destroy();
	}

	/**
	 * Create a BitArrayT instance using an external memory space.
	 * @param buffer - An external memory array. MUST NOT be NULL.
	 * @param buffer_bytes - Size of @buffer in bytes. MUST be in [1:@buffer_bytes / CHUNK_WIDTH_BYTES]
 	*/
	void allocate(void* buffer, const size_t buffer_bytes) noexcept {
		static_assert(Width >= BIT_WIDTH_MIN, "Template argument 'Width' must be greater than BIT_WIDTH_MIN.");
		static_assert(Width <= BIT_WIDTH_MAX, "Template argument 'Width' must be less than BIT_WIDTH_MAX.");
		const size_t chunk_capacity = buffer_bytes / CHUNK_BYTE_WIDTH;

		assert(m_chunks == nullptr);
		assert(buffer);
		assert(chunk_capacity >= CHUNK_CAPACITY_MIN);
		assert(chunk_capacity <= CHUNK_CAPACITY_MAX);

		m_chunks = reinterpret_cast<Chunk_t*>(buffer);
		m_chunks_capacity = chunk_capacity;
		m_items_capacity = m_chunks_capacity * CHUNK_BIT_WIDTH / Width;
	}

	/**
	 * Create a BitArrayT instance using its own memory space.
	 * @param item_capacity - BitArray size in items. MUST be greater than zero.
	 */
	void allocate(size_t item_capacity) noexcept {
		static_assert(Width >= BIT_WIDTH_MIN, "Template argument 'Width' must be greater than BIT_WIDTH_MIN.");
		static_assert(Width <= BIT_WIDTH_MAX, "Template argument 'Width' must be less than BIT_WIDTH_MAX.");

		assert(m_chunks == nullptr);
		assert(item_capacity >= ITEM_CAPACITY_MIN);
		assert(item_capacity <= ITEM_CAPACITY_MAX);

		const size_t bit_capacity = (Width * item_capacity);
		size_t chunk_capacity = bit_capacity / CHUNK_BIT_WIDTH;
		chunk_capacity += (bit_capacity % CHUNK_BIT_WIDTH) ? 1u : 0u;
		m_chunks = new Chunk_t[chunk_capacity];
		assert(m_chunks);
		m_chunks_capacity = chunk_capacity;
		m_items_capacity = m_chunks_capacity * CHUNK_BIT_WIDTH / Width;
		m_internal_mem = true;
	}

	void destroy() noexcept {
		if(m_internal_mem && m_chunks) {
			delete[] m_chunks;
		}
		m_chunks = nullptr;
		m_chunks_capacity = 0;
		m_items_capacity = 0;
	}

	// getter collection
	inline size_t chunk_capacity() const noexcept {
		return m_chunks_capacity;
	}

	inline size_t items_capacity() const noexcept {
		return m_items_capacity;
	}

	inline size_t byte_capacity() const noexcept {
		return m_chunks_capacity * CHUNK_BYTE_WIDTH;
	}

	inline size_t bits_capacity() const noexcept {
		return m_chunks_capacity * CHUNK_BIT_WIDTH;
	}

	/**
	 * Load @item_index item.
	 * @param item_index - MUST be in [0:items_capacity() - 1] range.
	 * @return - a loaded value in [0:range() - 1] range.
	 */
	Chunk_t load(const size_t item_index) const noexcept {
		const size_t bit_index = item_index * Width;
		Width_t bits_left = Width;
		Chunk_t value = 0;
		auto chunk_index = size_t(bit_index / CHUNK_BIT_WIDTH);
		auto chunk_offset = size_t(bit_index % CHUNK_BIT_WIDTH);
		while(bits_left) {
			bits_left -= load_bits(m_chunks[chunk_index], chunk_offset, bits_left, &value);
			chunk_index++;
			chunk_offset = 0;
		}
		return value;
	}

	/**
	 * Store @value to the @item_index'th index.
	 * @param item_index - MUST be in [0:items_capacity() - 1] range.
	 * @param value - MUST be in [0:range() - 1] range.
	 */
	void store(size_t item_index, Chunk_t value) noexcept {
		size_t bit_index = item_index * Width;
		Width_t bits_left = Width;
		auto chunk_index = size_t(bit_index / CHUNK_BIT_WIDTH);
		auto chunk_offset = size_t(bit_index % CHUNK_BIT_WIDTH);
		while(bits_left) {
			bits_left -= store_bits(m_chunks + chunk_index, chunk_offset, bits_left, value);
			chunk_index++;
			chunk_offset = 0;
		}
	}

	/**
	 * Fill the array with given value @value.
	 * @param value - value to fill with.
	 */
	void fill(Chunk_t value) noexcept {
		for(size_t i = 0; i < m_items_capacity; ++i) {
			store(i, value);
		}
	}

	/**
	 * The range limit of the item which the template can contain.
	 */
	static constexpr Chunk_t range() noexcept {
		return (1ull << Width);
	}

	/**
	 * The maximum value that the template can contain.
	 */
	static constexpr Chunk_t value_max() noexcept {
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
		const Chunk_t chunk
		, const size_t chunk_offset
		, const Width_t bit_width
		, Chunk_t* dst
	                               ) noexcept {
		Width_t bits_to_load = CHUNK_BIT_WIDTH - chunk_offset;
		bits_to_load = bit_width > bits_to_load ? bits_to_load : bit_width;
		const Chunk_t mask = (~((~0ull) << bits_to_load));
		*dst <<= bits_to_load;
		*dst |= ((chunk >> chunk_offset) & mask);
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
		Chunk_t* chunk
		, const size_t chunk_offset
		, const Width_t bit_width
		, Chunk_t src
	                                ) noexcept {
		Width_t bits_to_store = CHUNK_BIT_WIDTH - chunk_offset;
		bits_to_store = bit_width > bits_to_store ? bits_to_store : bit_width;
		src >>= (bit_width - bits_to_store);
		src <<= chunk_offset;
		const Chunk_t mask = (~((~0ull) << bits_to_store)) << chunk_offset;
		*chunk = ((*chunk) & ~mask) | (src & mask);
		return bits_to_store;
	}

};
