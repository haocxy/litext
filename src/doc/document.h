#pragma once

#include <cassert>
#include <fstream>
#include <memory>
#include <future>

#include "core/fs.h"
#include "core/worker.h"
#include "core/direct_file_reader.h"
#include "sqlite3_wrapper/database.h"
#include "charset.h"
#include "doc_define.h"
#include "declare_document_listener.h"


namespace doc::detail
{

class DocumentImpl : std::enable_shared_from_this<DocumentImpl> {
private:

	using Db = sqlite3_wrapper::Database;

	// 需要异步处理的组件都放在这个类中
	class AsyncComponents {
	public:
		AsyncComponents(const fs::path &filePath)
			: reader_(filePath) {}

		DirectFileReader &reader() {
			return reader_;
		}

	private:
		DirectFileReader reader_;
	};

	// 用于把异步组件AsyncComponents的对象在不同的线程中移动，以确保只有一个线程能够处理这些组件。
	// 这个类会在任何拷贝语义和移动语义时转移内部指针的所有，本质上是为了解决使用lambda时遇到的一些问题。
	//
	// 问：为什么不使用std::unique_ptr ？
	// 答：std::unique_ptr 移动对象的能力基于对于移动语义的适配，但是目前发现lambda的捕获列表无法很好地支持移动语义
	//
	// 问：为什么不直接使用原始的指针 ？
	// 答：在移动的过程中，如果其所在的函数对象丢失，则会导致对象丢失，不再被任何逻辑管理，进一步导致内存泄露及其它可能的问题
	//
	// 问：为什么不把这个类实现为模板类？
	// 答：这里的处理方式是不得以而为之，是很不好的。
	// 
	class AsyncComponentsMovePointer {
	public:
		AsyncComponentsMovePointer() {}

		AsyncComponentsMovePointer(AsyncComponents *ptr) : ptr_(ptr) {}

		AsyncComponentsMovePointer(const AsyncComponentsMovePointer &other) {
			if (this != &other && ptr_ != other.ptr_) {
				ptr_ = other.ptr_;
				mute(other).ptr_ = nullptr;
			}
		}

		AsyncComponentsMovePointer(AsyncComponentsMovePointer &&other) noexcept {
			if (this != &other && ptr_ != other.ptr_) {
				ptr_ = other.ptr_;
				other.ptr_ = nullptr;
			}
		}

		~AsyncComponentsMovePointer() {
			delete ptr_;
			ptr_ = nullptr;
		}

		AsyncComponentsMovePointer &operator=(const AsyncComponentsMovePointer &other) {
			if (this != &other && ptr_ != other.ptr_) {
				delete ptr_;
				ptr_ = other.ptr_;
				mute(other).ptr_ = nullptr;
			}
			return *this;
		}

		AsyncComponentsMovePointer &operator=(AsyncComponentsMovePointer &&other) noexcept {
			if (this != &other && ptr_ != other.ptr_) {
				delete ptr_;
				ptr_ = other.ptr_;
				other.ptr_ = nullptr;
			}
			return *this;
		}

		operator bool() const {
			return ptr_;
		}

		AsyncComponents &operator*() const {
			return *ptr_;
		}

		AsyncComponents *operator->() const {
			return ptr_;
		}

	private:
		AsyncComponentsMovePointer &mute(const AsyncComponentsMovePointer &other) {
			return const_cast<AsyncComponentsMovePointer &>(other);
		}

	private:
		AsyncComponents *ptr_ = nullptr;
	};

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

	void async(std::function<void(AsyncComponents &comps)> &&action, std::function<void()> &&done);

	void async(std::function<void(AsyncComponents &comps)> &&action) {
		async(std::move(action), [] {});
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
	AsyncComponentsMovePointer asyncComponents_;
	Db db_;
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
