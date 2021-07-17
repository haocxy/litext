#include "big_file_maker.h"

#include <cstdlib>
#include <ctime>
#include <cctype>
#include <optional>
#include <stdexcept>
#include <fstream>
#include <iostream>
#include <memory>
#include <map>

#include <QTextEncoder>
#include <QTextStream>

#include "asset.big_file_maker_kv.txt.h"
#include "asset.big_file_maker_mat.txt.h"


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
			std::cout << "progress: " << std::min(currProgress, 100) << "%" << std::flush;
		}
		prevProgress = currProgress;
		firstProgressPrint = false;
	}

	std::cout << std::endl;
	std::cout << "progress: Done" << std::endl;
}

void BigFileMaker::init() {
	std::string sCharset;
	arg(sCharset);
	charset_ = CharsetUtil::strToCharset(sCharset);

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

	loadAssets();

	const QString sLineEnd = QString::fromUtf8("\r\n");
	lineEnd_ = encoder_->fromUnicode(sLineEnd);

	std::srand(std::time(nullptr));
}

static int randBetween(int a, int b) {
	const int left = std::min(a, b);
	const int right = std::max(a, b);
	const int len = right - left + 1;
	return left + std::rand() % (len + 1);
}

void BigFileMaker::loadAssets()
{
	loadKeyValueAsset();
	loadParagraphMaterialAsset();
}

static void getKeyAndValInLine(const QString &line, QString &key, QString &val)
{
	const int size = line.size();
	for (int i = 0; i < size; ++i) {
		QChar ch = line[i];
		if (ch == ':') {
			key = line.mid(0, i);
			val = line.mid(i + 1);
			return;
		}
	}
}

void BigFileMaker::loadKeyValueAsset()
{
	std::map<QString, QString> kv;
	QString content = QString::fromUtf8(
		reinterpret_cast<const char *>(Asset::Data::big_file_maker_kv__txt),
		static_cast<int>(Asset::Len::big_file_maker_kv__txt)
	);

	QTextStream stream(&content);
	QString line;
	while (stream.readLineInto(&line)) {
		const int size = line.size();
		QString key;
		QString val;
		getKeyAndValInLine(line, key, val);
		kv[key] = val;
	}

	period_ = encoder_->fromUnicode(kv["period"]);

	beforeLineNumber_ = encoder_->fromUnicode(kv["beforeLineN"]);

	afterLineNumber_ = encoder_->fromUnicode(kv["afterLineN"]);
}

void BigFileMaker::loadParagraphMaterialAsset()
{
	QString content = QString::fromUtf8(
		reinterpret_cast<const char *>(Asset::Data::big_file_maker_mat__txt),
		static_cast<int>(Asset::Len::big_file_maker_mat__txt)
	);

	QTextStream stream(&content);
	QString line;
	while (stream.readLineInto(&line)) {
		materials_.push_back(encoder_->fromUnicode(line));
	}
}

void BigFileMaker::printLine(uintmax_t lineIndex)
{
	if (!materials_.empty()) {
		const QByteArray &paragraph = materials_[std::rand() % materials_.size()];
		ofs_.write(paragraph.data(), paragraph.size());
	}

	ofs_.write(beforeLineNumber_.data(), beforeLineNumber_.size());

	QString sLineNumber = QString("%1").arg(lineIndex + 1);
	QByteArray bLineNumber = encoder_->fromUnicode(sLineNumber);
	ofs_.write(bLineNumber.data(), bLineNumber.size());

	ofs_.write(afterLineNumber_.data(), afterLineNumber_.size());

	ofs_.write(period_.data(), period_.size());
	ofs_.write(lineEnd_.data(), lineEnd_.size());
}


}
