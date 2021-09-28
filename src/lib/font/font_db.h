#pragma once

#include <functional>

#include "core/fs.h"
#include "core/sqlite.h"

#include "font_info.h"


namespace font
{

class FontDb {
public:
    FontDb(const fs::path &dbFile);

    ~FontDb();

    class StmtForEachFontFile {
    public:
        StmtForEachFontFile(FontDb &db);

        void operator()(std::function<void(const fs::path &p)> &&action);

    private:
        sqlite::Statement stmt_;
    };

    class StmtLastWriteTimeOf {
    public:
        StmtLastWriteTimeOf(FontDb &db);

        opt<i64> operator()(const fs::path &fontFile);

    private:
        sqlite::Statement stmt_;
    };

    class StmtInsertFile {
    public:
        StmtInsertFile(FontDb &db);

        void operator()(const fs::path &fontFile, i64 writeTime);

    private:
        sqlite::Statement stmt_;
    };

    class StmtInsertFace {
    public:
        StmtInsertFace(FontDb &db);

        void operator()(const FaceInfo &info);

    private:
        sqlite::Statement stmt_;
    };

    class StmtDeleteFile {
    public:
        StmtDeleteFile(FontDb &db);

        void operator()(const fs::path &fontFile);

    private:
        sqlite::Statement stmt_;
    };

    class StmtDeleteFaces {
    public:
        StmtDeleteFaces(FontDb &db);

        void operator()(const fs::path &fontFile);

    private:
        sqlite::Statement stmt_;
    };

private:
    sqlite::Database db_;
};

}

