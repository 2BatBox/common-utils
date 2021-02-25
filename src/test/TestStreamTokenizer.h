#pragma once

#include "test_environment.h"
#include <tio/StreamTokenizer.h>
#include <tio/stream/ArrayInputStream.h>

class TestStreamTokenizer {
public:
	TestStreamTokenizer() noexcept {
		case_0();
	}

private:

	static void check_list(
		tio::StreamTokenizer& skt
		, bool trace
		, std::initializer_list<tio::StreamTokenizer::Token_t> list
	                      ) noexcept {
		for(const auto l : list) {
			const auto token = skt.next_token();
			if(trace) {
				printf("%u '%s'\n", token, skt.cstring());
			}
			assert(token == l);
		}
	}

	void case_0() noexcept {
		TRACE_CALL;
		enum Token : tio::StreamTokenizer::Token_t {
			CL_EOS,
			CL_UNKNOWN,
			CL_SPACE,
			CL_OPERATOR,
			TKN_MODULE,
			TKN_INPUT,
			TKN_WIRE
		};

		tio::ArrayInputStream ais(
			"module RsNor(input wire Set)");
		tio::StreamTokenizer stk(&ais, CL_EOS, CL_UNKNOWN, true);
		stk.classify(CL_SPACE, " \n\t", true);
		stk.classify(CL_OPERATOR, "(),;", false);

		stk.tokenize(TKN_MODULE, "module");
		stk.tokenize(TKN_INPUT, "input");
		stk.tokenize(TKN_WIRE, "wire");

		check_list(stk, false, {
			Token::TKN_MODULE, // 'module'
			Token::CL_SPACE, // ' '
			Token::CL_UNKNOWN, // 'RsNor'
			Token::CL_OPERATOR, // '('
			Token::TKN_INPUT, // 'input'
			Token::CL_SPACE, // ' '
			Token::TKN_WIRE, // 'wire'
			Token::CL_SPACE, // ' '
			Token::CL_UNKNOWN, // 'Set'
			Token::CL_OPERATOR, // ')'
			Token::CL_EOS
		});
	}

};
