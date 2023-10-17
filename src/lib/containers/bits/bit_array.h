#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/**
 * BitArray structure implements an array of bits sized unsigned integer variables called Items.
 * Items are packed in in a Chunk array with no gaps between them,
 * which means an Item is not a byte alignment structure.
 * The bit size of the Item must be in [1:__BIT_ARRAY_BIT_WIDTH_LIMIT] range.
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
typedef uint64_t BitArrayChunk_t;
typedef uint8_t BitArrayBitWidth_t;

#define __BIT_ARRAY_CHUNK_FORMAT "zu"
#define __BIT_ARRAY_CHUNK_FORMAT_HEX "016zx"

// calculate the limit constants which are based on the types section
#define __BIT_ARRAY_CHUNK_BYTE_WIDTH (sizeof(BitArrayChunk_t))
#define __BIT_ARRAY_CHUNK_BIT_WIDTH (__BIT_ARRAY_CHUNK_BYTE_WIDTH << 3ull)
#define __BIT_ARRAY_BIT_WIDTH_MIN (1u)
#define __BIT_ARRAY_BIT_WIDTH_MAX (__BIT_ARRAY_CHUNK_BIT_WIDTH - 1u)
#define __BIT_ARRAY_CHUNK_CAPACITY_MIN (1u)
#define __BIT_ARRAY_CHUNK_CAPACITY_MAX (~(0ull) / __BIT_ARRAY_CHUNK_BIT_WIDTH)
#define __BIT_ARRAY_ITEM_CAPACITY_MIN (1u)
#define __BIT_ARRAY_ITEM_CAPACITY_MAX (~(0ull) / __BIT_ARRAY_CHUNK_BIT_WIDTH)

struct BitArray {
	BitArrayChunk_t* chunks;
	size_t chunks_capacity;
	size_t items_capacity;
	size_t bytes_capacity;
	size_t bits_capacity;
	BitArrayBitWidth_t bit_width;
	bool internal_mem;
};

/**
 * Create a BitArray instance using an external memory space.
 * @param ins - a BitArray instance pointer. MUST NOT be NULL.
 * @param bit_width - How wide an array items in bits. MUST be in [1:__BIT_ARRAY_CHUNK_WIDTH_BITS] range.
 * @param buffer - An external memory array. MUST NOT be NULL.
 * @param buffer_bytes - Size of @buffer in bytes. MUST be in [1:@buffer_bytes / __BIT_ARRAY_CHUNK_WIDTH_BYTES]
 * @return @ins or NULL in case of errors.
 */
static inline struct BitArray* bit_array_create_ext(struct BitArray* ins, const  BitArrayBitWidth_t bit_width, void* buffer, const size_t buffer_bytes) {
	if(ins
	   && buffer
	   && bit_width >= __BIT_ARRAY_BIT_WIDTH_MIN
	   && bit_width <= __BIT_ARRAY_BIT_WIDTH_MAX
		) {

		const size_t chunk_capacity = buffer_bytes / __BIT_ARRAY_CHUNK_BYTE_WIDTH;
		if(chunk_capacity >= __BIT_ARRAY_CHUNK_CAPACITY_MIN && chunk_capacity <= __BIT_ARRAY_CHUNK_CAPACITY_MAX) {
			memset(ins, 0, sizeof(*ins));
			ins->chunks = (BitArrayChunk_t*) buffer;
			ins->chunks_capacity = chunk_capacity;
			ins->bytes_capacity = chunk_capacity * __BIT_ARRAY_CHUNK_BYTE_WIDTH;
			ins->bits_capacity = chunk_capacity * __BIT_ARRAY_CHUNK_BIT_WIDTH;
			ins->items_capacity = ins->bits_capacity / bit_width;
			ins->bit_width = bit_width;
			return ins;
		}
	}
	return NULL;
}

/**
 * Create a BitArray instance using its own memory space.
 * @param ins - a BitArray instance pointer. MUST NOT be NULL.
 * @param bit_width - How wide an array items in bits. MUST be in [1:__BIT_ARRAY_CHUNK_WIDTH_BITS] range.
 * @param item_capacity - BitArray size in items. MUST be greater than zero.
 * @return @ins or NULL in case of errors.
 */
static inline struct BitArray* bit_array_create(struct BitArray* ins, uint8_t bit_width, size_t item_capacity) {
	if(ins
	   && bit_width >= __BIT_ARRAY_BIT_WIDTH_MIN
	   && bit_width <= __BIT_ARRAY_BIT_WIDTH_MAX
	   && item_capacity >= __BIT_ARRAY_ITEM_CAPACITY_MIN
	   && item_capacity <= __BIT_ARRAY_ITEM_CAPACITY_MAX
		) {

		const size_t bit_capacity = (bit_width * item_capacity);
		size_t chunk_capacity = bit_capacity / __BIT_ARRAY_CHUNK_BIT_WIDTH;
		chunk_capacity += (bit_capacity % __BIT_ARRAY_CHUNK_BIT_WIDTH) ? 1u : 0u;
		memset(ins, 0, sizeof(*ins));
		ins->chunks = (BitArrayChunk_t*) malloc(chunk_capacity * __BIT_ARRAY_CHUNK_BYTE_WIDTH);
		if(ins->chunks) {
			ins->chunks_capacity = chunk_capacity;
			ins->bytes_capacity = chunk_capacity * __BIT_ARRAY_CHUNK_BYTE_WIDTH;
			ins->bits_capacity = chunk_capacity * __BIT_ARRAY_CHUNK_BIT_WIDTH;
			ins->items_capacity = ins->bits_capacity / bit_width;
			ins->bit_width = bit_width;
			ins->internal_mem = true;
			return ins;
		}
	}
	return NULL;
}

/**
 * Destroy a previously created instance.
 * @param ins - a BitArray instance pointer. MUST NOT be NULL.
 */
static inline void bit_array_destroy(struct BitArray* ins) {
	if(ins && ins->chunks) {
		if(ins->internal_mem) {
			free(ins->chunks);
		}
		ins->chunks = NULL;
		ins->items_capacity = 0;
	}
}

/**
 * Try to load @bit_width bits from @chunk_offset offset to @dst.
 * @param chunk - a chunk to load from.
 * @param chunk_offset - bit offset inside the chunk.
 * @param dst - a value pointer to load to.
 * @return - now many bits have been loaded from the chunk.
 */
static inline BitArrayBitWidth_t __bit_array_load_bits(
	const BitArrayChunk_t chunk
	, const size_t chunk_offset
	, const BitArrayBitWidth_t bit_width
	, BitArrayChunk_t* dst
                                                      ) {
	BitArrayBitWidth_t bits_to_load = __BIT_ARRAY_CHUNK_BIT_WIDTH - chunk_offset;
	bits_to_load = bit_width > bits_to_load ? bits_to_load : bit_width;
	const BitArrayChunk_t mask = (~((~0ull) << bits_to_load));
	*dst <<= bits_to_load;
	*dst |= ((chunk >> chunk_offset) & mask);
	return bits_to_load;
}

/**
 * Load @item_index item.
 * @param ins - a BitArray instance pointer. MUST NOT be NULL.
 * @param item_index - MUST be in [0:ins->item_capacity - 1] range.
 * @return - loaded value.
 */
static inline BitArrayChunk_t bit_array_load(const struct BitArray* ins, const size_t item_index) {
	const BitArrayBitWidth_t bit_width = ins->bit_width;
	const size_t bit_index = item_index * bit_width;
	BitArrayBitWidth_t bits_left = bit_width;
	BitArrayChunk_t value = 0;
	size_t chunk_index = (size_t)(bit_index / __BIT_ARRAY_CHUNK_BIT_WIDTH);
	size_t chunk_offset = (size_t)(bit_index % __BIT_ARRAY_CHUNK_BIT_WIDTH);
	while(bits_left){
		bits_left -= __bit_array_load_bits(ins->chunks[chunk_index], chunk_offset, bits_left, &value);
		chunk_index++;
		chunk_offset = 0;
	}
	return value;
}

/**
 * Try to store @bit_width bits to @chunk_offset offset from @src.
 * @param bit_index - a bit offset in the array.
 * @param bit_width - now many bits @src contains.
 * @param src - source variable.
 * @return - now many bits have been stored to the array.
 */
static inline BitArrayBitWidth_t __bit_array_store_bits(
	BitArrayChunk_t* chunk
	, const size_t chunk_offset
	, const BitArrayBitWidth_t bit_width
	, BitArrayChunk_t src
                                         ) {
	BitArrayBitWidth_t bits_to_store = __BIT_ARRAY_CHUNK_BIT_WIDTH - chunk_offset;
	bits_to_store = bit_width > bits_to_store ? bits_to_store : bit_width;
	src >>= (bit_width - bits_to_store);
	src <<= chunk_offset;
	const BitArrayChunk_t mask = (~((~0ull) << bits_to_store)) << chunk_offset;
	*chunk = ((*chunk) & ~mask) | (src & mask);
	return bits_to_store;
}

/**
 * Store @value to the @item_index'th index.
 * @param ins - a BitArray instance pointer. MUST NOT be NULL.
 * @param item_index - MUST be in [0:ins->item_capacity - 1] range.
 * @param value - MUST be in [0:bit_array_range(@ins)-1] range.
 */
static inline void bit_array_store(struct BitArray* ins, size_t item_index, BitArrayChunk_t value) {
	const BitArrayBitWidth_t bit_width = ins->bit_width;
	size_t bit_index = item_index * bit_width;
	BitArrayBitWidth_t bits_left = bit_width;
	size_t chunk_index = (size_t)(bit_index / __BIT_ARRAY_CHUNK_BIT_WIDTH);
	size_t chunk_offset = (size_t)(bit_index % __BIT_ARRAY_CHUNK_BIT_WIDTH);
	while(bits_left){
		bits_left -= __bit_array_store_bits(ins->chunks + chunk_index, chunk_offset, bits_left, value);
		chunk_index++;
		chunk_offset = 0;
	}
}

/**
 * Fill the array @ins with given value @value.
 * @param ins - a BitArray instance pointer. MUST NOT be NULL.
 * @param value - value to fill with.
 */
static inline void bit_array_fill(struct BitArray* ins, BitArrayChunk_t value) {
	for(size_t i = 0; i < ins->items_capacity; ++i) {
		bit_array_store(ins, i, value);
	}
}

/**
 * The range limit of the item which @ins can contain.
 * @param ins - a BitArray instance pointer. MUST NOT be NULL.
 */
static inline BitArrayChunk_t bit_array_range(struct BitArray* ins) {
	 return (1ull << ins->bit_width);
}

#endif //TOP_N_PROBLEM_BITARRAY_H
