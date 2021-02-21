#pragma once

#include <cstdint>
#include <codecvt>

namespace utils {

class ByteOrder {
public:

	static inline constexpr uint16_t ct_cpu_to_be16(uint16_t v) noexcept {
#if __BYTE_ORDER == __LITTLE_ENDIAN
		return (((v >> 8) & 0x00FF) | ((v << 8) & 0xFF00));
#elif __BYTE_ORDER == __BIG_ENDIAN
		return v;
#endif
	}

	static inline constexpr uint32_t ct_cpu_to_be32(uint32_t v) noexcept {
#if __BYTE_ORDER == __LITTLE_ENDIAN
		return (((v >> 24) & 0x000000FF) | ((v >> 8) & 0x0000FF00) | ((v << 8) & 0x00FF0000) | ((v << 24) & 0xFF000000));
#elif __BYTE_ORDER == __BIG_ENDIAN
		return v;
#endif
	}

};

}; // namespace utils
