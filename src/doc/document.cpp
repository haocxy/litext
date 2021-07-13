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

}
