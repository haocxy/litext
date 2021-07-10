#include "doc_server.h"

#include "charset_detect_util.h"


namespace doc
{

DocServer::DocServer(const fs::path &filePath)
	: filePath_(filePath)
	, charset_(CharsetDetectUtil::quickDetectCharset(filePath)) {

}

DocServer::~DocServer() {

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
