#pragma once

#include <fstream>
#include <memory>
#include <future>

#include "core/fs.h"
#include "core/worker.h"

#include "charset.h"
#include "doc_define.h"
#include "declare_document_listener.h"


namespace doc
{

class Document : std::enable_shared_from_this<Document> {
public:

	using Pointer = std::shared_ptr<Document>;

	// 使用Document::create(...)函数创建对象，确保每个对象都被shared_ptr管理，
// 以使得shared_from_this正常工作
	Document(const fs::path &file, Worker &ownerThread);

	// 这个类的对象会利用shared_from_this来确保自身在lambda的生命周期中不会被释放，
	// 必须却保该类的每一个对象都是通过shared_ptr创建的
	static Pointer create(const fs::path &path, Worker &ownerThread) {
		return std::make_shared<Document>(path, ownerThread);
	}

	~Document();

	void bind(DocumentListener &listener);

	void unbind();

	Charset charset() const;

	bool loaded() const;

	RowN loadedRowCount() const;

private:


	void asyncLoadOnePart();

	void asyncHandleFile(std::function<void(std::fstream &fstream)> &&action, std::function<void()> &&done) {
		auto self(shared_from_this());
		std::async(std::launch::async, [this, self, action = std::move(action), done = std::move(done), fstream = std::move(fstream_)]() mutable {
			action(fstream);
			std::fstream file = std::move(fstream);
			auto lam = [this, self, done = std::move(done)]() mutable {};
			ownerThread_.post(std::move(lam));
		});
	}

	void asyncHandleFile(std::function<void(std::fstream &fstream)> &&action) {
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
	std::fstream fstream_;
	Worker &ownerThread_;
	DocumentListener *listener_ = nullptr;

private:
	Charset charset_ = Charset::Unknown;
	bool loaded_ = false;
	RowN loadedRowCount_ = 0;
};

}
