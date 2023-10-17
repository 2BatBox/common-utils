#pragma once

#include <cstdlib>
#include <cstdint>

/**
 * Provides an independent stream of pseudo-random numbers.
 */
class DiceMachine {
	uint16_t m_seed[3];
public:

	explicit DiceMachine(uint64_t seed) noexcept {
		m_seed[2] = uint16_t((seed >> uint64_t(0)) & uint64_t(0xFFFF));
		m_seed[1] = uint16_t((seed >> uint64_t(16)) & uint64_t(0xFFFF));
		m_seed[0] = uint16_t((seed >> uint64_t(32)) & uint64_t(0xFFFF));
	}

	/**
	 * @param prob - must be in [0, 1] interval.
	 * @return The probability of returning true is @prob.
	 */
	inline bool pass(double prob) noexcept {
		return erand48(m_seed) < prob;
	}

	inline double range_double(double min, double max) noexcept {
		const double range = max - min;
		return min + erand48(m_seed) * range;
	}

	inline double drand48() noexcept {
		return erand48(m_seed);
	}

	inline long int lrand48() noexcept {
		return jrand48(m_seed);
	}

	inline uint32_t u32() noexcept {
		return jrand48(m_seed);
	}

	inline uint64_t u64() noexcept {
		uint64_t result = jrand48(m_seed);
		result <<= 32u;
		return result + jrand48(m_seed);
	}

};
