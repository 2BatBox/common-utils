#pragma once

#include "CharClassifier.h"

#include <map>

namespace tio {

class StreamTokenizer : public CharClassifier {
public:
	using Token_t = CharClassifier::CharClassId;

protected:
	using Base_t = CharClassifier;
	using TokenMap_t = std::map<std::string, Token_t>;

	TokenMap_t m_token_map;

public:
	StreamTokenizer(const StreamTokenizer&) = delete;
	StreamTokenizer(StreamTokenizer&&) = delete;

	StreamTokenizer& operator=(const StreamTokenizer&) = delete;
	StreamTokenizer& operator=(StreamTokenizer&&) = delete;

	virtual ~StreamTokenizer() = default;

	StreamTokenizer(
		InputStream* stream
		, const Token_t token_eos
		, const Token_t token_unknown
		, bool accumulate
	               ) noexcept
		: Base_t(stream, token_eos, token_unknown, accumulate)
		, m_token_map() {}

	void tokenize(const Token_t token, const char* token_str) noexcept {
		m_token_map.insert(std::make_pair(token_str, token));
	}

	inline Token_t next_token() noexcept {
		Token_t token = next();
		const auto it = m_token_map.find(cstring());
		if(it != m_token_map.end()) {
			token = it->second;
		}
		return token;
	}
};

}; // namespace tio
