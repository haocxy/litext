#include "doc_server.h"

#include "charset_detect_util.h"


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

std::vector<UString> DocServer::loadRows(const RowRange &range)
{
	return std::vector<UString>(); // TODO
}

RowN DocServer::rowCount()
{
	return RowN(); // TODO
}




}
