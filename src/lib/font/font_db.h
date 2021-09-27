#pragma once

#include "core/fs.h"
#include "core/sqlite.h"

#include "font_info.h"


namespace font
{

class FontDb {
public:
    FontDb(const fs::path &dbFile);

    ~FontDb();

    class StmtLastWriteTimeOf {
    public:
        StmtLastWriteTimeOf(FontDb &db);

        opt<i64> operator()(const fs::path &fontFile);

    private:
        sqlite::Statement stmt_;
    };

    opt<i64> lastWriteTimeOf(const fs::path &fontFile);

    class StmtInsertFile {
    public:
        StmtInsertFile(FontDb &db);

        void operator()(const fs::path &fontFile, i64 writeTime);

    private:
        sqlite::Statement stmt_;
    };

    void insertFile(const fs::path &fontFile, i64 writeTime);

    class StmtInsertFace {
    public:
        StmtInsertFace(FontDb &db);

        void operator()(const FaceInfo &info);

    private:
        sqlite::Statement stmt_;
    };

    void insertFace(const FaceInfo &info);

    class StmtDeleteFile {
    public:
        StmtDeleteFile(FontDb &db);

        void operator()(const fs::path &fontFile);

    private:
        sqlite::Statement stmt_;
    };

    void removeFile(const fs::path &fontFile);

    class StmtDeleteFaces {
    public:
        StmtDeleteFaces(FontDb &db);

        void operator()(const fs::path &fontFile);

    private:
        sqlite::Statement stmt_;
    };

    void removeFaces(const fs::path &fontFile);

private:
    sqlite::Database db_;
    sqlite::Statement stmtSelectLastWriteTimeByPath_;
    sqlite::Statement stmtInsertFontFile_;
    sqlite::Statement stmtInsertFontFace_;
    sqlite::Statement stmtDeleteFontFile_;
    sqlite::Statement stmtDeleteFontFaces_;
};

}

