#include "big_file_maker.h"

#include <cctype>
#include <optional>
#include <stdexcept>


namespace tool
{

static bool isFloatNumberWithoutSuffix(const std::string &s) {
	bool alreadyMeetPoint = false;
	for (char ch : s) {
		if (ch == '.') {
			if (alreadyMeetPoint) {
				return false;
			} else {
				alreadyMeetPoint = true;
			}
		} else if (!std::isdigit(ch)) {
			return false;
		}
	}
	return true;
}

static bool isFloatNumberMayWithSuffix(const std::string &s) {
	if (s.empty()) {
		return false;
	}

	const char last = std::tolower(s.back());
	if (last == 'k' || last == 'm' || last == 'g') {
		return isFloatNumberWithoutSuffix(s.substr(0, s.size() - 1));
	} else {
		return isFloatNumberWithoutSuffix(s);
	}
}

static uintmax_t getFactorBySuffix(char suffix) {
	constexpr uintmax_t k = 1024;
	constexpr uintmax_t kb = k;
	constexpr uintmax_t mb = kb * k;
	constexpr uintmax_t gb = mb * k;

	switch (std::tolower(suffix)) {
	case 'k': return kb;
	case 'm': return mb;
	case 'g': return gb;
	default: return 1;
	}
}

static uintmax_t readableSizeHintToByteCount(const std::string &s) {


	if (!isFloatNumberMayWithSuffix(s)) {
		std::ostringstream ss;
		ss << "bad cmd arg fmt, float num required, but got: [" << s << "]";
		throw std::runtime_error(ss.str());
	}

	const char last = std::tolower(s.back());
	double num = 0;
	if (last == 'k' || last == 'm' || last == 'g') {
		std::istringstream ss(s.substr(0, s.size() - 1));
		ss >> num;
		return static_cast<uintmax_t>(num * getFactorBySuffix(last) + 0.5);
	} else {
		std::istringstream ss(s);
		ss >> num;
		return static_cast<uintmax_t>(num);
	}
}

void BigFileMaker::fillArgs() {
	std::string s;
	fillArg(s);
	sizeHint_ = readableSizeHintToByteCount(s);

	fillArg(path_);
}


}
