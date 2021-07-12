#pragma once

#include <cstdint>
#include <string>
#include <fstream>
#include <vector>

#include <QString>
#include <QByteArray>

#include "core/fs.h"
#include "doc/charset.h"
#include "tool.h"


class QTextEncoder;


namespace tool
{


class BigFileMaker : public Tool {
public:
	BigFileMaker() {}

	virtual ~BigFileMaker();

	virtual void execute() override;

protected:
	virtual void init() override;

private:
	void loadAssets();

	void loadKeyValueAsset();

	void loadParagraphMaterialAsset();

	void printLine(uintmax_t lineIndex);

private:
	doc::Charset charset_ = doc::Charset::Unknown;

	uintmax_t sizeHint_ = 0;

	fs::path path_;

	// 编码后的句号
	QByteArray period_;

	// 编码后的换行符
	QByteArray lineEnd_;

	QByteArray beforeLineNumber_;

	QByteArray afterLineNumber_;

	std::vector<QByteArray> materials_;

	QTextEncoder *encoder_ = nullptr;

	std::ofstream ofs_;
};


}
