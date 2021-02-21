#pragma once

namespace qlibs {
namespace utils {

class Colors {
	static bool enable;

public:
	static constexpr const char* BLACK = "\033[1;30m";
	static constexpr const char* RED = "\033[1;31m";
	static constexpr const char* GREEN = "\033[1;32m";
	static constexpr const char* YELLOW = "\033[1;33m";
	static constexpr const char* BLUE = "\033[1;34m";
	static constexpr const char* PURPLE = "\033[1;35m";
	static constexpr const char* CYAN = "\033[1;36m";
	static constexpr const char* WHITE = "\033[1;37m";
	static constexpr const char* DEFAULT = "\033[0m";

	static constexpr const char* BCG_BLACK = "\033[1;40m";
	static constexpr const char* BCG_RED = "\033[1;41m";
	static constexpr const char* BCG_GREEN = "\033[1;42m";
	static constexpr const char* BCG_YELLOW = "\033[1;43m";
	static constexpr const char* BCG_BLUE = "\033[1;44m";
	static constexpr const char* BCG_PURPLE = "\033[1;45m";
	static constexpr const char* BCG_CYAN = "\033[1;46m";
	static constexpr const char* BCG_WHITE = "\033[1;47m";
	static constexpr const char* BCG_DEFAULT = "\033[0m";

	static constexpr const char* DEFAULT_ALL = "\033[0m\033[0m";

	static const char* paint(const char* clr) noexcept {
		return enable ? clr : "";
	}

	static const char* clear() noexcept {
		return enable ? DEFAULT_ALL : "";
	}

	static void mode(bool is_enable) noexcept {
		enable = is_enable;
	}

};

}; // namespace utils
}; // namespace qlibs

#endif //QLIBS_UTILS_COLOR_H
