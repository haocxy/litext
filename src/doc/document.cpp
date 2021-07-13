#include "document.h"

namespace doc
{

Document::Document(const fs::path &file, Worker &ownerThread)
	: file_(file)
	, ownerThread_(ownerThread)
	, server_(ownerThread, file)
{

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
