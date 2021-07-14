#pragma once

#include <cassert>
#include <fstream>
#include <memory>
#include <future>

#include "core/fs.h"
#include "core/worker.h"

#include "charset.h"
#include "doc_define.h"
#include "declare_document_listener.h"


namespace doc::detail
{

class DocumentImpl : std::enable_shared_from_this<DocumentImpl> {
public:

	using Pointer = std::shared_ptr<DocumentImpl>;

	DocumentImpl(const fs::path &file, Worker &ownerThread);

	~DocumentImpl();

	void bind(DocumentListener &listener);

	void unbind();

	Charset charset() const;

	bool loaded() const;

	RowN loadedRowCount() const;

private:
	void asyncLoadOnePart();

	void asyncHandleFile(std::function<void(std::ifstream &ifs)> &&action, std::function<void()> &&done) {
		if (!ifs_) {
			throw std::logic_error("bad logic, [ifs_] is null");
		}
		if (!action) {
			throw std::logic_error("bad logic, [action] is null");
		}
		if (!done) {
			throw std::logic_error("bad logic, [done] is null");
		}

		auto self(shared_from_this());
		std::ifstream *ifs = ifs_;
		ifs_ = nullptr;
		std::async(std::launch::async, [this, self, action = std::move(action), done = std::move(done), ifs]() mutable {
			action(*ifs);
			ownerThread_.post([this, self, done = std::move(done), ifs]{
				ifs_ = ifs;
				done();
			});
		});
	}

	void asyncHandleFile(std::function<void(std::ifstream &ifs)> &&action) {
		asyncHandleFile(std::move(action), [] {});
	}

	void async(std::function<void(Pointer self)> &&f) {
		if (f) {
			auto self(shared_from_this());
			std::async(std::launch::async, [self, f = std::move(f)]{
				f(self);
			});
		}
	}

	static void sync(Pointer self, std::function<void()> &&f) {
		if (self && f) {
			self->ownerThread_.post([self, f = std::move(f)]{
				f();
			});
		}
	}

private:
	const fs::path path_;
	std::ifstream *ifs_;
	Worker &ownerThread_;
	DocumentListener *listener_ = nullptr;

private:
	Charset charset_ = Charset::Unknown;
	bool loaded_ = false;
	RowN loadedRowCount_ = 0;
};

}


namespace doc
{

class Document {
public:
	Document(const fs::path &file, Worker &ownerThread)
		: ptr_(std::make_shared<detail::DocumentImpl>(file, ownerThread)) {}

	~Document() {
		assert(ptr_);
		ptr_->unbind();

		// 这一句很重要，这不是普通的指针
		ptr_ = nullptr;
	}

	void bind(DocumentListener &listener) {
		assert(ptr_);
		ptr_->bind(listener);
	}

	void unbind() {
		assert(ptr_);
		ptr_->unbind();
	}

	Charset charset() const {
		assert(ptr_);
		return ptr_->charset();
	}

	bool loaded() const {
		assert(ptr_);
		return ptr_->loaded();
	}

	RowN loadedRowCount() const {
		assert(ptr_);
		return ptr_->loadedRowCount();
	}

private:
	std::shared_ptr<detail::DocumentImpl> ptr_;
};

}
