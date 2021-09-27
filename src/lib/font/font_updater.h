#pragma once

#include "core/fs.h"
#include "core/thread.h"
#include "core/basetype.h"
#include "core/font.h"

#include "font_db.h"


namespace font
{

class FontUpdater {
public:
    FontUpdater(FontDb &db);

    ~FontUpdater();

private:
    void loop();

    class UpdateWorker {
    public:
        UpdateWorker(FontUpdater &outer, FontDb &db);

        ~UpdateWorker();

    private:
        void loop();

        void updateFont(const fs::path &p);

    private:
        std::atomic_bool &stopping_;
        BlockQueue<fs::path> &foundFilePaths_;
        FontDb::StmtLastWriteTimeOf stmtLastWriteTimeOf_;
        FontDb::StmtDeleteFaceAndFile stmtDeleteFaceAndFiles_;
        FontDb::StmtInsertFile stmtInsertFile_;
        FontDb::StmtInsertFace stmtInsertFace_;
        FontContext context_;
        std::thread thread_;
    };

private:
    std::atomic_bool stopping_{ false };
    BlockQueue<fs::path> foundFilePaths_;
    std::thread scanThread_;
    std::vector<uptr<UpdateWorker>> updateWorkers_;
};

}
