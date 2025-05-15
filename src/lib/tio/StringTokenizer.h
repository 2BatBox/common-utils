#pragma once

#include <cstdlib>
#include <cstdint>
#include <cctype>

namespace tio {

/**
 * No dependencies. No virtual functions.
 * @tparam Capacity - The token length limit including the terminal zero byte.
 * @tparam LowCase - Translate tokens to lower case.
 */
template<size_t Capacity, bool LowCase = false>
class StringTokenizer {
	static_assert(Capacity > 1, "StringTokenizer::Capacity > 1");
	enum class CharClass : uint8_t {
		ZERO,
		SEPARATOR,
		REGULAR
	};

	const char* m_str;
	const char* const m_separators;
	char m_acc[Capacity];
	size_t m_acc_next;
	bool m_overflown;

public:

	/**
	 * @param str - An input char sequence as a zero terminated string. MUST NOT be NULL.
	 * @param separators - An separator char sequence as a zero terminated string. MUST NOT be NULL.
	 */
	StringTokenizer(const char* str, const char* separators) noexcept
		: m_str(str)
		, m_separators(separators)
		, m_acc_next(0)
		, m_overflown(false) {
		m_acc[0] = 0;
	}

	StringTokenizer(const StringTokenizer& rv) = delete;
	StringTokenizer& operator=(const StringTokenizer& rv) = delete;

	StringTokenizer(StringTokenizer&&) = delete;
	StringTokenizer& operator=(StringTokenizer&&) = delete;

	bool next() noexcept {
		m_acc_next = 0;
		m_overflown = false;
		while(char_class(*m_str) == CharClass::SEPARATOR) {
			m_str++;
		}
		while(char_class(*m_str) == CharClass::REGULAR) {
			if(m_acc_next < Capacity - 1ull) {
				m_acc[m_acc_next++] = LowCase ? tolower(*m_str) : *m_str;
				m_str++;
				continue;
			}
			m_overflown = true;
			break;
		}
		m_acc[m_acc_next] = 0;
		return m_acc_next;
	}

	inline const char* token() const noexcept {
		return m_acc;
	}

	inline bool overflown() const noexcept {
		return m_overflown;
	}

private:

	CharClass char_class(char ch) const noexcept {
		if(ch == 0) {
			return CharClass::ZERO;
		} else if(is_space(ch)) {
			return CharClass::SEPARATOR;
		} else {
			return CharClass::REGULAR;
		}
	}

	bool is_space(char ch) const noexcept {
		const char* spaces = m_separators;
		while(*spaces) {
			if(*spaces == ch) {
				return true;
			}
			spaces++;
		}
		return false;
	}
};

}; // namespace tio
