#ifndef UTILS_COLORS_H
#define UTILS_COLORS_H

namespace utils {

class Colors {
public:
	static constexpr const char* RED = "\033[1;31m";
	static constexpr const char* GREEN = "\033[1;32m";
	static constexpr const char* NORMAL = "\033[0m";
};

}; // namespace utils

#endif /* UTILS_COLORS_H */

