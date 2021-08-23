#pragma once

#include <optional>

#include "core/fs.h"
#include "core/signal.h"
#include "core/charset.h"

#include "doc_error.h"
#include "doc_row.h"
#include "decoded_part.h"


namespace doc
{

class DocumentImpl;

class Document {
public:
    Document(const fs::path &file);

    ~Document();

    void start();

    Signal<void(DocError)> &sigFatalError();

    Signal<void(Charset)> &sigCharsetDetected();

    Signal<void(const DecodedPart &)> &sigPartLoaded();

    Signal<void()> &sigAllLoaded();

    Signal<void(RowN)> &sigRowCountUpdated();

    RowN rowCnt() const;

    std::optional<Row> rowAt(RowN row);

    std::map<RowN, Row> rowsAt(const RowRange &range);

private:
    doc::DocumentImpl *impl_ = nullptr;
};

}
