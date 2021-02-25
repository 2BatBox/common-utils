#pragma once

#include "stream/InputStream.h"

#include <vector>

namespace tio {

class CharClassifier {
public:
	using Char_t = char;
	using CharClassId = unsigned;

protected:

	static constexpr size_t CharClassMapSize = 1ull << (sizeof(Char_t) << 3ull);

	struct CharClass {
		CharClassId id;
		bool accumulate;
	};

	const CharClassId m_cclass_eos;
	InputStream* m_stream;
	std::vector<Char_t> m_accum;
	CharClass m_char_class_map[CharClassMapSize];
	bool m_next;

public:
	CharClassifier(const CharClassifier&) = delete;
	CharClassifier(CharClassifier&&) = delete;

	CharClassifier& operator=(const CharClassifier&) = delete;
	CharClassifier& operator=(CharClassifier&&) = delete;

	virtual ~CharClassifier() = default;

	/**
	 *
	 * @param stream - A tio::InputStream instance pointer to read from.
	 * @param cclass_eos - End of stream user identifier.
	 * @param cclass_unknown - Unknown/default char class user identifier.
	 * @param accumulate - enables accumulating unknown/default characters.
	 */
	CharClassifier(
		InputStream* stream
		, const CharClassId cclass_eos
		, const CharClassId cclass_unknown
		, bool accumulate
	              ) noexcept
		: m_cclass_eos(cclass_eos)
		, m_stream(stream)
		, m_accum()
		, m_next(stream->next()) {
		clear_char_class_map({cclass_unknown, accumulate});
	}

	/**
	 * Append a new char class.
	 * @param cclass_id - user space identifier.
	 * @param cclass_str - char list as a as a null-terminated string.
	 * @param accumulate - enables accumulating @cclass_id characters.
	 */
	void classify(const CharClassId cclass_id, const Char_t* cclass_str, bool accumulate) noexcept {
		const CharClass cclass{cclass_id, accumulate};
		while(*cclass_str) {
			const unsigned char idx = *cclass_str;
			m_char_class_map[idx] = cclass;
			cclass_str++;
		}
	}

	/**
	 * @return char class identifier.
	 */
	CharClassId next() noexcept {
		CharClassId result = m_cclass_eos;
		m_accum.resize(0);
		if(m_next) {
			const char ch = m_stream->read();
			const unsigned char idx = ch;
			const auto cclass = m_char_class_map[idx];
			if(cclass.accumulate) {
				accumulate(cclass.id);
			} else {
				m_accum.push_back(ch);
				m_next = m_stream->next();
			}
			result = cclass.id;
		}
		m_accum.push_back(0);
		return result;
	}

	/**
	 * @return Accumulator as a null-terminated string.
	 */
	const Char_t* cstring() const noexcept {
		return m_accum.data();
	}

protected:

	void accumulate(const CharClassId cclass_id) noexcept {
		do {
			const char ch = m_stream->read();
			const unsigned char idx = ch;
			if(m_char_class_map[idx].id == cclass_id) {
				m_accum.push_back(ch);
				continue;
			}
			break;
		} while((m_next = m_stream->next()));
	}

	/**
	 * Fill the char class map with @cclass value.
	 */
	void clear_char_class_map(const CharClass& cclass) noexcept {
		for(size_t i = 0; i < CharClassMapSize; ++i) {
			m_char_class_map[i] = cclass;
		}
	}
};

}; // namespace tio
