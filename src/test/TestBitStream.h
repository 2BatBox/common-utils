#pragma once

#include "test_environment.h"
#include <containers/bits/BitStream.h>
#include <memory>

class TestBitStream {

	std::unique_ptr<uint8_t> m_array;
	BitStream m_bs;

public:
	explicit TestBitStream(size_t capacity) noexcept
		: m_array(new uint8_t[capacity])
		, m_bs(m_array.get(), capacity) {

		TRACE_CALL;

		const uint64_t seed = 1717;
		fill_rnd(m_bs, seed);
		check_rnd(m_bs, seed);
	}

	static void fill_rnd(BitStream& bs, const uint64_t seed) noexcept {
		DiceMachine dm(seed);
		size_t capacity_sum = 0;

		bs.reset();
		while(bs.available()) {
			const BitStream::Width_t bits = dm.u32() % (BitStream::CHUNK_BIT_WIDTH + 1u);
			BitStream::Chunk_t chunk = dm.u64();
			if(bits < BitStream::CHUNK_BIT_WIDTH) {
				const BitStream::Chunk_t chunk_mask = ~((~0ull) << bits);
				chunk &= chunk_mask;
			}

			if(bs.write(chunk, bits)) {
//				printf("write [%u] 0x%zx\n", bits, chunk);
				capacity_sum += bits;
			}
		}
		assert(capacity_sum == bs.capacity());
	}

	static void check_rnd(BitStream& bs, const uint64_t seed) noexcept {
		DiceMachine dm(seed);
		size_t capacity_sum = 0;

		bs.reset();
		while(bs.available()) {
			const BitStream::Width_t bits = dm.u32() % (BitStream::CHUNK_BIT_WIDTH + 1u);
			BitStream::Chunk_t chunk = dm.u64();
			if(bits < BitStream::CHUNK_BIT_WIDTH) {
				const BitStream::Chunk_t chunk_mask = ~((~0ull) << bits);
				chunk &= chunk_mask;
			}

			BitStream::Chunk_t chunk_to_read;
			if(bs.read(chunk_to_read, bits)) {
//				printf("read [%u] 0x%zx\n", bits, chunk_to_read);
				assert(chunk_to_read == chunk);
				capacity_sum += bits;
			}
		}
		assert(capacity_sum == bs.capacity());
	}

};
