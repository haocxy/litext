#include "big_file_maker.h"

#include <cstdlib>
#include <ctime>
#include <cctype>
#include <optional>
#include <stdexcept>
#include <fstream>
#include <iostream>

#include <QTextEncoder>


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


BigFileMaker::~BigFileMaker()
{
	delete encoder_;
	encoder_ = nullptr;
}

void BigFileMaker::execute()
{
	if (fs::is_directory(path_)) {
		std::ostringstream ss;
		ss << "specified path [" << path_.generic_string() << "] is an existed dir";
		throw std::runtime_error(ss.str());
	}

	fs::create_directories(path_.parent_path());

	ofs_.open(path_, std::ios::binary);
	if (!ofs_) {
		std::ostringstream ss;
		ss << "cannot open file: [" << path_.generic_string() << "]";
		throw std::runtime_error(ss.str());
	}

	int prevProgress = 0;
	bool firstProgressPrint = true;
	for (uintmax_t lineIndex = 0; ofs_ && static_cast<uintmax_t>(ofs_.tellp()) < sizeHint_; ++lineIndex) {
		printLine(lineIndex);
		uintmax_t size = ofs_.tellp();
		int currProgress = static_cast<int>(double(size) / double(sizeHint_) * 100);
		if (currProgress != prevProgress) {
			if (!firstProgressPrint) {
				std::cout << "\r";
			}
			std::cout << "progress: " << currProgress << "%" << std::flush;
		}
		prevProgress = currProgress;
		firstProgressPrint = false;
	}

	std::cout << std::endl;
	std::cout << "progress: Done" << std::endl;
}

static const char *const gUtf8StrPart = " 这a是b一c段d用于测试E的文字， ";

static const char *const gUtf8Period = ".";

static const char *const gUtf8BeforeLineNumber = "row index: ";


void BigFileMaker::init() {
	std::string sCharset;
	arg(sCharset);
	charset_ = doc::CharsetUtil::strToCharset(sCharset);

	std::string sSizeHint;
	arg(sSizeHint);
	sizeHint_ = readableSizeHintToByteCount(sSizeHint);

	arg(path_);

	const QTextCodec *codec = QTextCodec::codecForName(sCharset.c_str());
	if (!codec) {
		std::ostringstream ss;
		ss << "unsupported charset: [" << sCharset << "]";
		throw std::runtime_error(ss.str());
	}

	encoder_ = codec->makeEncoder();
	if (!encoder_) {
		std::ostringstream ss;
		ss << "cannot create encoder for charset: [" << sCharset << "]";
		throw std::runtime_error(ss.str());
	}

	const QString sPart = QString::fromUtf8(gUtf8StrPart);
	part_ = encoder_->fromUnicode(sPart);

	const QString sPeriod = QString::fromUtf8(gUtf8Period);
	period_ = encoder_->fromUnicode(sPeriod);

	const QString sLineEnd = QString::fromUtf8("\r\n");
	lineEnd_ = encoder_->fromUnicode(sLineEnd);

	const QString sBeforeLineNumber = QString::fromUtf8(gUtf8BeforeLineNumber);
	beforeLineNumber_ = encoder_->fromUnicode(sBeforeLineNumber);

	std::srand(std::time(nullptr));
}

static int randBetween(int a, int b) {
	const int left = std::min(a, b);
	const int right = std::max(a, b);
	const int len = right - left + 1;
	return left + std::rand() % (len + 1);
}

void BigFileMaker::printLine(uintmax_t lineIndex)
{
	const int repeatCount = randBetween(1, 10);

	for (int i = 0; i < repeatCount; ++i) {
		ofs_.write(part_.data(), part_.size());
	}

	ofs_.write(beforeLineNumber_.data(), beforeLineNumber_.size());

	QString sLineNumber = QString("%1").arg(lineIndex + 1);
	QByteArray bLineNumber = encoder_->fromUnicode(sLineNumber);
	ofs_.write(bLineNumber.data(), bLineNumber.size());

	ofs_.write(period_.data(), period_.size());
	ofs_.write(lineEnd_.data(), lineEnd_.size());
}


}
