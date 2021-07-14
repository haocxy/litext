#include "document.h"

#include <future>



namespace doc::detail
{

DocumentImpl::DocumentImpl(const fs::path &path, Worker &ownerThread)
	: path_(path)
	, ifs_(new std::ifstream(path, std::ios::binary))
	, ownerThread_(ownerThread)
{
	asyncLoadOnePart();
}

void DocumentImpl::asyncLoadOnePart()
{
	asyncHandleFile([](std::ifstream &f) {

	});
}

DocumentImpl::~DocumentImpl()
{

}

void DocumentImpl::bind(DocumentListener &listener)
{
	listener_ = &listener;
}

void DocumentImpl::unbind()
{
	listener_ = nullptr;
}

bool DocumentImpl::loaded() const
{
	return loaded_;
}

RowN DocumentImpl::loadedRowCount() const
{
	return loadedRowCount_;
}

Charset DocumentImpl::charset() const
{
	return charset_;
}

}
