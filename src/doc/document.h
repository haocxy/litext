#pragma once

#include "core/fs.h"
#include "core/worker.h"

#include "async_doc_server.h"
#include "declare_document_listener.h"


namespace doc
{

class Document {
public:
	Document(const fs::path &file, Worker &ownerThread);

	~Document();

	void bind(DocumentListener &listener);

	void unbind();

	Charset charset() const;

	bool loaded() const;

	RowN loadedRowCount() const;

private:
	const fs::path file_;
	Worker &ownerThread_;
	AsyncDocServer server_;
	DocumentListener *listener_ = nullptr;

private:
	Charset charset_ = Charset::Unknown;
	bool loaded_ = false;
	RowN loadedRowCount_ = 0;

};

}
