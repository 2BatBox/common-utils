#ifndef STORAGE_RATELIMITERSTAT_H
#define STORAGE_RATELIMITERSTAT_H

#include <cstdlib>
#include <cstdio>
#include <cstdint>

namespace storage {

struct RateLimiterStat {
	size_t capacity;
	size_t size;

	static void print_field(FILE* out, const char* name, uint64_t value, uint64_t value_prev) noexcept {
		fprintf(out, "%s=%zu(%zu) ", name, value, value - value_prev);
	}

	void print(FILE* out, const RateLimiterStat&) const noexcept {
		float load_factor = (static_cast<float>(size) / capacity) * 100.0f;
		fprintf(out, "[TQ] ");
		fprintf(out, "%zu/%zu (%.2f%%) ", size, capacity, load_factor);
		//        print_field(out, "put_col", put_collision, prev.put_collision);
	}
};

}; // namespace storage

#endif /* STORAGE_RATELIMITERSTAT_H */

