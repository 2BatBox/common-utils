#pragma once

#include "test_environment.h"
#include <tio/CharClassifier.h>
#include <tio/stream/ArrayInputStream.h>

class TestCharClassifier {
public:
	TestCharClassifier() noexcept {
		case_0();
		case_1();
		case_2();
	}

private:

	static void check_list(
		tio::CharClassifier& skt
		, bool trace
		, std::initializer_list<tio::CharClassifier::CharClassId> list
	                      ) noexcept {
		for(const auto l : list) {
			const auto token = skt.next();
			if(trace) {
				printf("%u '%s'\n", token, skt.cstring());
			}
			assert(token == l);
		}
	}

	void case_0() noexcept {
		TEST_TRACE;
		enum CClassId : tio::CharClassifier::CharClassId {
			EOS,
			UNKNOWN,
			SEPARATOR
		};

		tio::ArrayInputStream ais("Cut  by\t \nthe\n separators ");
		tio::CharClassifier ccf(&ais, EOS, UNKNOWN, true);
		ccf.classify(SEPARATOR, " \t\n", true);
		check_list(ccf, false, {
			CClassId::UNKNOWN, // Cut
			CClassId::SEPARATOR,
			CClassId::UNKNOWN, // by
			CClassId::SEPARATOR,
			CClassId::UNKNOWN, // the
			CClassId::SEPARATOR,
			CClassId::UNKNOWN, // separators
			CClassId::SEPARATOR,
			CClassId::EOS
		});
	}

	void case_1() noexcept {
		TEST_TRACE;
		enum CClassId : tio::CharClassifier::CharClassId {
			EOS,
			UNKNOWN,
			SEPARATOR,
			EQ,
			AND,
			OR
		};

		tio::ArrayInputStream ais("f==a&&b||c");
		tio::CharClassifier ccf(&ais, EOS, UNKNOWN, true);
		ccf.classify(CClassId::EQ, "=", false);
		ccf.classify(CClassId::AND, "&", false);
		ccf.classify(CClassId::OR, "|", false);

		check_list(ccf, false, {
			CClassId::UNKNOWN, // 'f'
			CClassId::EQ, // '='
			CClassId::EQ, // '='
			CClassId::UNKNOWN, // 'a'
			CClassId::AND, // '&'
			CClassId::AND, // '&'
			CClassId::UNKNOWN, // 'b'
			CClassId::OR, // '|'
			CClassId::OR, // '|'
			CClassId::UNKNOWN, // 'c'
			CClassId::EOS
		});
	}

	void case_2() noexcept {
		TEST_TRACE;
		enum CClassId : tio::CharClassifier::CharClassId {
			EOS,
			UNKNOWN,
			SEPARATOR,
			OPERATOR
		};

		tio::ArrayInputStream ais("a=b/c; //it_does_nothing\n /*neither_it_does*/");
		tio::CharClassifier ccf(&ais, EOS, UNKNOWN, true);
		ccf.classify(CClassId::SEPARATOR, " ;\t\n", true);
		ccf.classify(CClassId::OPERATOR, "=/*", true);

		check_list(ccf, false, {
			CClassId::UNKNOWN, // 'a'
			CClassId::OPERATOR, // '='
			CClassId::UNKNOWN, // 'b'
			CClassId::OPERATOR, // '/'
			CClassId::UNKNOWN, // 'c'
			CClassId::SEPARATOR, // '; '
			CClassId::OPERATOR, // '//'
			CClassId::UNKNOWN, // it_does_nothing
			CClassId::SEPARATOR, // '\n '
			CClassId::OPERATOR, // '/*'
			CClassId::UNKNOWN, // 'neither_it_does'
			CClassId::OPERATOR, // '*/'
			CClassId::EOS
		});
	}

};
