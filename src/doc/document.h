#pragma once

#include <memory>
#include <fstream>

#include <boost/signals2.hpp>

#include "core/strand.h"
#include "core/membuff.h"
#include "core/charset.h"
#include "core/sqlite.h"
#include "core/charset_detector.h"
#include "doc/doc_define.h"


namespace doc::detail
{

class DocumentImpl : public std::enable_shared_from_this<DocumentImpl> {
private:

    using Db = sqlite::Database;
    using Statement = sqlite::Statement;

    // 需要异步处理的组件都放在这个类中
    class AsyncComponents {
    public:
        AsyncComponents(const fs::path &filePath)
            : ifs_(filePath, std::ios::binary) {}

        std::ifstream &ifs() {
            return ifs_;
        }

        MemBuff &readBuff() {
            return readBuff_;
        }

        MemBuff &decodeBuff() {
            return decodeBuff_;
        }

        CharsetDetector &charsetDetector() {
            return charsetDetector_;
        }

        Statement &saveDataStatement() {
            return saveDataStatement_;
        }

    private:
        std::ifstream ifs_;
        MemBuff readBuff_;
        MemBuff decodeBuff_;
        CharsetDetector charsetDetector_;
        Statement saveDataStatement_;
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
    DocumentImpl(const fs::path &file, Strand &ownerThread);

    virtual ~DocumentImpl();

    void start();

    Charset charset() const;

    bool loaded() const;

    RowN loadedRowCount() const;

private:
    void asyncLoadDocument();

    struct LoadingPartInfo {
        uintmax_t off = 0;
        uintmax_t len = 0;
        Charset charset = Charset::Unknown;
    };

    void loadPart(AsyncComponents &comps, const LoadingPartInfo &info);

    bool prepareDatabase();

    bool prepareSaveDataStatement(Statement &stmt);

    void loadDocument(AsyncComponents &comps);

private:
    const fs::path path_;
    AsyncComponentsMovePointer asyncComponents_;
    Db db_;
    Strand &ownerThread_;

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
    Document(const fs::path &file, Strand &ownerThread)
        : ptr_(std::make_shared<detail::DocumentImpl>(file, ownerThread)) {

        // 只有ptr_初始化完成后才能启动需要使用shared_from_this的逻辑，
        // 因为需要确保共享指针的内部状态正确初始化
        ptr_->start();
    }

    ~Document() {}

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
