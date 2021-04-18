#pragma once

#include <cstdlib>
#include <cstdint>
#include <cctype>
#include <cmath>

namespace cli {

class Multiplier {
public:
	static bool power_of_2(char ch, double& mult) noexcept {
		bool result = true;
		double exp;
		ch = std::tolower(ch);
		switch(ch) {
			case 'k':
				exp = 10;
				break;
			case 'm':
				exp = 20;
				break;
			case 'g':
				exp = 30;
				break;
			case 't':
				exp = 40;
				break;
			case 'p':
				exp = 50;
				break;
			case 'e':
				exp = 60;
				break;
			default:
				result = false;
				break;
		}
		mult = std::pow(2.0, exp);
		return result;
	}

	static bool power_of_10(char ch, double& mult) noexcept {
		bool result = true;
		double exp;
		ch = std::tolower(ch);
		switch(ch) {
			case 'k':
				exp = 3;
				break;
			case 'm':
				exp = 6;
				break;
			case 'g':
				exp = 9;
				break;
			case 't':
				exp = 12;
				break;
			case 'p':
				exp = 15;
				break;
			case 'e':
				exp = 18;
				break;
			default:
				result = false;
				break;
		}
		mult = std::pow(10.0, exp);
		return result;
	}
};

}; // namespace cli
