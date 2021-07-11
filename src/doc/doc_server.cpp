#include "doc_server.h"

#include <stdexcept>

#include "charset_detect_util.h"
#include "doc_cutter_by_row.h"


namespace doc
{

DocServer::DocServer(const fs::path &filePath)
	: filePath_(filePath) {

}

DocServer::~DocServer() {

}

void DocServer::detectCharset()
{
	const std::string charsetStr = CharsetDetectUtil::quickDetectCharset(filePath_);
	charset_ = CharsetUtil::strToCharset(charsetStr);
}

static uintmax_t determinePartSizeForDocCutter(const fs::path &path) {
	const uintmax_t filesize = fs::file_size(path);

}

void DocServer::startDetectRowCount()
{
	if (cutter_) {
		throw std::logic_error("DocServer::startDetectRowCount() already called");
	}

	cutter_ = new DocCutterByRow(filePath_, charset_, 1024);

}

std::vector<UString> DocServer::loadRows(const RowRange &range)
{
	return std::vector<UString>(); // TODO
}

RowN DocServer::rowCount()
{
	return RowN(); // TODO
}




}
