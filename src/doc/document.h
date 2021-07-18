#pragma once

#include <memory>
#include <fstream>

#include <boost/signals2.hpp>

#include "core/strand.h"
#include "core/membuff.h"
#include "core/charset.h"
#include "core/sqlite.h"
#include "core/io_context_strand.h"
#include "core/charset_detector.h"
#include "doc/doc_define.h"
#include "text_database.h"


namespace doc::detail
{

class DocumentImpl : public std::enable_shared_from_this<DocumentImpl> {
public:
    DocumentImpl(IOContextStrand::Pool &pool, const fs::path &file);

    virtual ~DocumentImpl();

    Charset charset() const;

    bool loaded() const;

    RowN loadedRowCount() const;

private:
    const fs::path path_;
    TextDatabase textDb_;

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
    Document(IOContextStrand::Pool &pool, const fs::path &file)
        : ptr_(std::make_shared<detail::DocumentImpl>(pool, file)) {}

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
