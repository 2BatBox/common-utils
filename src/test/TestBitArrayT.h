#pragma once

#include "test_environment.h"
#include <containers/bits/BitArrayT.h>

template <uint8_t Width>
class TestBitArrayTInternal {
	BitArrayT<Width> m_bat;

public:
	TestBitArrayTInternal(const TestBitArrayTInternal&) = delete;
	TestBitArrayTInternal(TestBitArrayTInternal&&) = delete;

	TestBitArrayTInternal& operator=(const TestBitArrayTInternal&) = delete;
	TestBitArrayTInternal& operator=(TestBitArrayTInternal&&) = delete;

	virtual ~TestBitArrayTInternal() = default;

	TestBitArrayTInternal(size_t capacity) noexcept {
		unsigned step = 1;
		m_bat.allocate(capacity);
		store_load_seq(step++);
		store_load_rnd(step++);
	}

private:

	void store_load_seq(unsigned step) {
		const auto range = m_bat.range();
		const auto item_nb = m_bat.items_capacity();
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
		const auto item_nb = m_bat.items_capacity();
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
//		BitArrayT<Width> barray(buffer, buffer_nb);
//		barray.fill(0);
//		const BitArrayTChunk_t range = barray.range();
//
//		uint64_t before = rdtsc();
//		uint64_t sum = 0;
//		for(size_t i = 0; i < rounds; ++i){
//			const size_t index = i % barray.items_capacity();
//			const BitArrayTChunk_t val = barray.load(index);
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

class TestBitArrayT {
public:
	explicit TestBitArrayT(size_t capacity) noexcept {
		TEST_TRACE;
		TestBitArrayTInternal<2> bit_arrayt2(capacity);
		TestBitArrayTInternal<3> bit_arrayt3(capacity);
		TestBitArrayTInternal<4> bit_arrayt4(capacity);
		TestBitArrayTInternal<5> bit_arrayt5(capacity);
		TestBitArrayTInternal<15> bit_arrayt15(capacity);
		TestBitArrayTInternal<16> bit_arrayt16(capacity);
		TestBitArrayTInternal<31> bit_arrayt31(capacity);
		TestBitArrayTInternal<32> bit_arrayt32(capacity);
		TestBitArrayTInternal<40> bit_arrayt40(capacity);
		TestBitArrayTInternal<63> bit_arrayt63(capacity);
	}
};
