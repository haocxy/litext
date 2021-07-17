#pragma once

#include <cassert>

#include "document_impl.h"


namespace doc
{

class Document {
public:
	Document(const fs::path &file, Worker &ownerThread)
		: ptr_(std::make_shared<detail::DocumentImpl>(file, ownerThread)) {

		// 只有ptr_初始化完成后才能启动需要使用shared_from_this的逻辑，
		// 因为需要确保共享指针的内部状态正确初始化
		ptr_->start();
	}

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
