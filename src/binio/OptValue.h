#ifndef BINIO_OPT_VALUE_H
#define BINIO_OPT_VALUE_H

namespace binio {

/**
 * Class binio::OptValue represents an option value.
 * No memory management is provided.
 */
template<typename T>
struct OptValue {
protected:
	T m_value;
	bool m_has_value;

public:

	using Value_t = T;

	OptValue() noexcept : m_value(), m_has_value(false) {}

	OptValue(const T& v) noexcept : m_value(v), m_has_value(true) {}

	OptValue(T&& v) noexcept : m_value(std::move(v)), m_has_value(true) {}

	OptValue(const OptValue&) noexcept = default;
	OptValue(OptValue&&) noexcept = default;

	OptValue& operator=(const T& v) noexcept {
		m_value = v;
		m_has_value = true;
		return *this;
	}

	OptValue& operator=(T&& v) noexcept {
		std::swap(m_value, v);
		m_has_value = true;
		return *this;
	}

	OptValue& operator=(const OptValue& opt) noexcept {
		if(this != &opt) {
			m_value = opt.m_value;
			m_has_value = opt.m_has_value;
		}
		return *this;
	}

	OptValue& operator=(OptValue&& opt) noexcept {
		if(this != &opt) {
			m_value = std::move(opt.m_value);
			m_has_value = opt.m_has_value;
			opt.reset();
		}
		return *this;
	}

	inline T& assign() noexcept {
		m_has_value = true;
		return m_value;
	}

	inline void reset() noexcept {
		m_has_value = false;
	}

	inline bool has_value() const noexcept {
		return m_has_value;
	}

	inline const T& value() const noexcept {
		return m_value;
	}

	inline T& value() noexcept {
		return m_value;
	}

	inline bool operator==(const OptValue& lv) const noexcept {
		return (m_has_value && lv.m_has_value && m_value == lv.m_value);
	}

	inline bool operator!=(const OptValue& lv) const noexcept {
		return not operator==(lv);
	}

};

}; // namespace binio

#endif /* BINIO_OPT_VALUE_H */

