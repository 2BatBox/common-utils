#pragma once

#include "test_environment.h"
#include <containers/bits/BitArray.h>

template <uint8_t Width, typename Chunk>
class TestBitArrayInternal {
	BitArray<Width, Chunk> m_bat;

public:
	TestBitArrayInternal(const TestBitArrayInternal&) = delete;
	TestBitArrayInternal(TestBitArrayInternal&&) = delete;

	TestBitArrayInternal& operator=(const TestBitArrayInternal&) = delete;
	TestBitArrayInternal& operator=(TestBitArrayInternal&&) = delete;

	virtual ~TestBitArrayInternal() = default;

	TestBitArrayInternal(size_t capacity) noexcept {
		unsigned step = 1;
		m_bat.allocate(capacity);
		store_load_seq(step++);
		store_load_rnd(step++);
	}

private:

	void store_load_seq(unsigned step) {
		const auto range = m_bat.range();
		const auto item_nb = m_bat.capacity();
		assert(range);
		assert(item_nb);
		m_bat.fill(0);
		for(size_t i = 0; i < item_nb; ++i) {
			const auto value = (i + step) % range;
			m_bat.store(i, value);
			assert(m_bat.load(i) == value);
		}
		m_bat.fill(0);
		for(size_t i = 0; i < item_nb; ++i) {
			const auto value = (i + step) % range;
			size_t index = size_t(item_nb - 1 - i);
			m_bat.store(index, value);
			assert(m_bat.load(index) == value);
		}
		m_bat.fill(0);
		for(size_t i = 0; i < item_nb; ++i) {
			const auto value = (i + step) % range;
			m_bat.store(i, value);
		}
		for(size_t i = 0; i < item_nb; ++i) {
			const auto value = (i + step) % range;
			assert(m_bat.load(i) == value);
		}
	}

	void store_load_rnd(unsigned step) {
		const auto range = m_bat.range();
		const auto item_nb = m_bat.capacity();
		assert(range);
		assert(item_nb);
		m_bat.fill(0);

		size_t sum_in = 0;
		for(size_t i = 0; i < item_nb; ++i) {
			const auto value = (i + step) % range;
			m_bat.store(i, value);
			assert(m_bat.load(i) == value);
			sum_in += value;
		}

		size_t rounds = item_nb * 100;
		while(rounds--){
			const auto idx_first = rand() % item_nb;
			const auto idx_second = rand() % item_nb;
			const auto value_first = m_bat.load(idx_first);
			const auto value_second = m_bat.load(idx_second);
			m_bat.store(idx_first, value_second);
			m_bat.store(idx_second, value_first);
		}
		size_t sum_out = 0;
		for(size_t i = 0; i < item_nb; ++i) {
			sum_out += m_bat.load(i);
		}
		assert(sum_out);
		assert(sum_in == sum_out);
	}

//	void perf(size_t rounds) {
//		TRACE_CALL;
//		char buffer[999];
//		const size_t buffer_nb = sizeof(buffer) / sizeof(*buffer);
//
//		BitArraySeqT<Width> barray(buffer, buffer_nb);
//		barray.fill(0);
//		const BitArraySeqTChunk_t range = barray.range();
//
//		uint64_t before = rdtsc();
//		uint64_t sum = 0;
//		for(size_t i = 0; i < rounds; ++i){
//			const size_t index = i % barray.items_capacity();
//			const BitArraySeqTChunk_t val = barray.load(index);
//			barray.store(index, (val + 1) % range);
//			sum += val;
//		}
//		uint64_t spent = rdtsc() - before;
//		printf("width=%u", Width);
//		printf(" rounds=%zu", rounds);
//		printf(" sum=0x%zx\n", sum);
//		printf("perf=%f clocks per one store/load\n\n", spent / double(rounds));
//	}

};

class TestBitArray {
public:
	explicit TestBitArray(size_t capacity) noexcept {
		TEST_TRACE;
		chunk_type_iteration<2>(capacity);
		chunk_type_iteration<3>(capacity);
		chunk_type_iteration<7>(capacity);
		chunk_type_iteration<8>(capacity);
		chunk_type_iteration<9>(capacity);
		chunk_type_iteration<15>(capacity);
		chunk_type_iteration<16>(capacity);
		chunk_type_iteration<17>(capacity);
		chunk_type_iteration<31>(capacity);
		chunk_type_iteration<32>(capacity);
		chunk_type_iteration<33>(capacity);
		chunk_type_iteration<63>(capacity);
	}

private:

	template <uint8_t Width>
	void chunk_type_iteration(const size_t capacity) noexcept {
		if constexpr (Width < 8) {
			TestBitArrayInternal<Width, uint8_t> bit_array_8(capacity);
		}
		if constexpr (Width < 16) {
			TestBitArrayInternal<Width, uint16_t> bit_array_16(capacity);
		}
		if constexpr (Width < 32) {
			TestBitArrayInternal<Width, uint32_t> bit_array_32(capacity);
		}

		TestBitArrayInternal<Width, uint64_t> bit_array_64(capacity);
	}

};
