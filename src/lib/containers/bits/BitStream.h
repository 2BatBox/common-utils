#pragma once

#include <cstdlib>
#include <cstdint>

/**
 * Header-only.
 *
 * BitStream template class helps using any byte array as a stream of bits that can be either read or written.
 * The bit interpretation format:
 *
 * byte-array-index :  0                          1                         ...
 * byte-array       : [b7|b6|b5|b4|b3|b2|b1|b0]  [b7|b6|b5|b4|b3|b2|b1|b0]  ...
 * bit-array-index  :  0  1  2  3  4  5  6  7     8  9  10 11 12 13 14 15   ...
 *
 * Sample of using:
 *
 * 1. A nibble printer.
 *
 * BitStream::Byte_t pattern[] = { 0xAB, 0xCD, 0xFF, 0x00, 0xAA };
 * BitStream bs(pattern, sizeof(pattern));
 * BitStream::Chunk_t chunk;
 * while(bs.read(chunk, 4)) {
 * 		printf("%zx.", chunk);
 * }
 * printf("\n");
 *
 *
 */

class BitStream {

public:

	using Width_t = uint8_t;
	using Byte_t = uint8_t;
	using Chunk_t = uint64_t;

	static constexpr Width_t BYTE_BIT_WIDTH = sizeof(Byte_t) << 3ull;
	static constexpr Width_t CHUNK_BIT_WIDTH = sizeof(Chunk_t) << 3ull;

protected:

	Byte_t* const m_byte_array = nullptr;
	const size_t m_capacity_bits = 0;

	size_t m_offset_bytes = 0;
	Width_t m_offset_bits = 0;

public:

	/**
	 * Create a BitStream instance using an external memory space.
	 * @param buffer - An external memory array. MUST NOT be NULL.
	 * @param buffer_bytes - Size of @buffer in bytes.
 	*/
	BitStream(Byte_t* buffer, const size_t buffer_bytes) noexcept
		: m_byte_array(buffer)
		, m_capacity_bits(buffer_bytes * BYTE_BIT_WIDTH)
		, m_offset_bytes(0)
		, m_offset_bits(0) {}

	/**
	 * @return The array capacity in bits.
	 */
	inline size_t capacity() const noexcept {
		return m_capacity_bits;
	}

	/**
	 * Returns the reading/writing head to the initial point.
	 */
	inline void reset() noexcept {
		m_offset_bytes = 0;
		m_offset_bits = 0;
	}

	/**
	 * Returns the reading/writing head offset from the initial point.
	 */
	inline size_t offset() const noexcept {
		return m_offset_bytes * BYTE_BIT_WIDTH + m_offset_bits;
	}

	/**
	 * @return How may bits are available for reading/writing.
	 */
	size_t available() const noexcept {
		return m_capacity_bits - offset();
	}

	/**
	 * @param bits
	 * @return Checks is \a bits are available.
	 */
	bool available(const Width_t bits) const noexcept {
		return available() >= bits;
	}

	/**
	 * Reads \a bits_to_read bits from the stream and moves the current reading/writing head forward.
	 * @param value - a chunk to read to.
	 * @return true - in case of reading had been successfully done.
	 */
	bool read(Chunk_t& value, Width_t bits_to_read) noexcept {
		if(not available(bits_to_read) || bits_to_read > CHUNK_BIT_WIDTH) {
			return false;
		}
		value = 0;

		while(bits_to_read) {
			const Width_t read = read_byte(m_byte_array[m_offset_bytes], m_offset_bits, bits_to_read, &value);
			bits_to_read -= read;
			m_offset_bits += read;
			if(m_offset_bits >= BYTE_BIT_WIDTH) {
				m_offset_bits -= BYTE_BIT_WIDTH;
				m_offset_bytes++;
			}
		}
		return true;
	}

	/**
	 * Writes \a bits_to_write bits to the stream and moves the current reading/writing head forward.
	 * @param value - a chunk to read from.
	 * @return true - in case of writing had been successfully done.
	 */
	bool write(const Chunk_t value, Width_t bits_to_write) noexcept {
		if(not available(bits_to_write) || bits_to_write > CHUNK_BIT_WIDTH) {
			return false;
		}

		while(bits_to_write) {
			const Width_t written = write_byte(m_byte_array + m_offset_bytes, m_offset_bits, bits_to_write, value);
			bits_to_write -= written;
			m_offset_bits += written;
			if(m_offset_bits >= BYTE_BIT_WIDTH) {
				m_offset_bits -= BYTE_BIT_WIDTH;
				m_offset_bytes++;
			}
		}
		return true;
	}

private:

	/**
	 * Reads \a bit_width bits from \a byte to \a dst.
	 * @param byte - a byte to read bits from.
	 * @param byte_bit_offset - bit offset in \a byte value.
	 * @param bits_left - a number of bits are left to read.
	 * @param chunk - a chunk to store to.
	 * @return - now many bits have been read from the chunk. Can be in [0:bit_width] range.
	 */
	static inline Width_t read_byte(
		const Byte_t byte
		, const Width_t byte_bit_offset
		, const Width_t bits_left
		, Chunk_t* chunk
	                               ) noexcept {

		// How many bits are available to read from 'byte'.
		const Width_t chunk_bits_available = BYTE_BIT_WIDTH - byte_bit_offset;

		// How many bits are going to be read.
		const Width_t bits_to_read = bits_left > chunk_bits_available ? chunk_bits_available : bits_left;

		// How many LS bits in the byte must be skipped.
		const Width_t byte_rshift = chunk_bits_available - bits_to_read;

		const Chunk_t chunk_mask = (~((~0ull) << bits_to_read));

		*chunk <<= bits_to_read;
		*chunk |= (Byte_t(byte >> byte_rshift) & chunk_mask);

		return bits_to_read;
	}

	/**
	 * Writes \a bit_width bits from \a src to \a byte.
	 * @param byte - a byte to write bits to.
	 * @param byte_bit_offset - bit offset in \a byte value.
	 * @param bits_left - a number of bits are left to write.
	 * @param chunk - a chunk to read from.
	 * @return - now many bits have been written.
	 */
	static inline Width_t write_byte(
		Byte_t* byte
		, const Width_t byte_bit_offset
		, const Width_t bits_left
		, Chunk_t chunk
	                                ) noexcept {

		// How many bits are available to write to 'byte'.
		const Width_t chunk_bits_available = BYTE_BIT_WIDTH - byte_bit_offset;

		// How many bits are going to be written.
		const Width_t bits_to_write = bits_left > chunk_bits_available ? chunk_bits_available : bits_left;

		// How many chunk LS bits must be skipped.
		const Width_t chunk_rshift = bits_left - bits_to_write;

		const Width_t lshift = chunk_bits_available - bits_to_write;

		const Chunk_t mask = (~((~0ul) << bits_to_write)) << lshift;

		chunk >>= chunk_rshift;
		chunk <<= lshift;
		*byte = ((*byte) & ~mask) | Byte_t(chunk & mask);

		return bits_to_write;
	}

};
