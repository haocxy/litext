#include "document.h"

#include <future>



namespace doc
{

Document::Document(const fs::path &path, Worker &ownerThread)
	: path_(path)
	, fstream_(path, std::ios::binary)
	, ownerThread_(ownerThread)
{
	asyncLoadOnePart();
}

void Document::asyncLoadOnePart()
{
	asyncHandleFile([](std::fstream &f) {

	});
}

Document::~Document()
{

}

void Document::bind(DocumentListener &listener)
{
	listener_ = &listener;
}

void Document::unbind()
{
	listener_ = nullptr;
}

bool Document::loaded() const
{
	return loaded_;
}

RowN Document::loadedRowCount() const
{
	return loadedRowCount_;
}

Charset Document::charset() const
{
	return charset_;
}

}
