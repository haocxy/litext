#include "doc_server.h"



namespace doc
{

DocServer::DocServer(const fs::path &filePath)
	: filePath_(filePath) {

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
