#pragma once

#include <memory>
#include <fstream>

#include "core/worker.h"
#include "core/membuff.h"
#include "core/charset.h"
#include "doc/doc_define.h"
#include "doc/charset_detector.h"
#include "doc/declare_document_listener.h"
#include "sqlite3/database.h"


namespace doc::detail
{

class DocumentImpl : public std::enable_shared_from_this<DocumentImpl> {
private:

    using Db = sqlite3ns::Database;

    // 需要异步处理的组件都放在这个类中
    class AsyncComponents {
    public:
        AsyncComponents(const fs::path &filePath)
            : ifs_(filePath, std::ios::binary) {}

        std::ifstream &ifs() {
            return ifs_;
        }

        MemBuff &buff() {
            return buff_;
        }

        CharsetDetector &charsetDetector() {
            return charsetDetector_;
        }

    private:
        std::ifstream ifs_;
        MemBuff buff_;
        CharsetDetector charsetDetector_;
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

    void start();

    void bind(DocumentListener &listener);

    void unbind();

    Charset charset() const;

    bool loaded() const;

    RowN loadedRowCount() const;

private:
    void asyncLoadDocument();

    struct LoadingPartInfo {

        LoadingPartInfo() {}

        LoadingPartInfo(uintmax_t off, uintmax_t len)
            : off(off), len(len) {}

        uintmax_t off = 0;
        uintmax_t len = 0;
    };

    void loadPart(AsyncComponents &comps, const MemBuff &data, const LoadingPartInfo &info);

    bool prepareDatabase();

    void loadDocument(AsyncComponents &comps);

private:
    const fs::path path_;
    const fs::path dbPath_;
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
