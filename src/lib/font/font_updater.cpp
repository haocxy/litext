#include "font_updater.h"

#include "core/system.h"
#include "core/logger.h"


namespace font
{

static i64 decideUpdateThreadCount()
{
    return std::max(2, SystemUtil::processorCount() / 2);
}

FontUpdater::FontUpdater(FontDb &db)
{
    scanThread_ = std::thread([this, &db] {
        removeUselessData(db);
        loop();
    });

    const i64 updateThreadCount = decideUpdateThreadCount();

    for (i64 i = 0; i < updateThreadCount; ++i) {
        updateWorkers_.push_back(std::make_unique<UpdateWorker>(*this, db));
    }
}

FontUpdater::~FontUpdater()
{
    stopping_ = true;
    foundFilePaths_.stop();
    scanThread_.join();
    updateWorkers_.clear();
}

void FontUpdater::removeUselessData(FontDb &db)
{
    FontDb::StmtDeleteFaces stmtDeleteFaces(db);
    FontDb::StmtDeleteFile stmtDeleteFile(db);
    FontDb::StmtForEachFontFile stmtForEachFontFile(db);

    stmtForEachFontFile([&stmtDeleteFaces, &stmtDeleteFile](const fs::path &p) {
        if (!fs::exists(p) || !fs::is_regular_file(p)) {
            stmtDeleteFaces(p);
            stmtDeleteFile(p);
            LOGI << "FontUpdater::removeUselessData() data for file [" << p << "] deleted";
        }
    });
}

void FontUpdater::loop()
{
    ThreadUtil::setNameForCurrentThread("FontUpdater-ScanThread");

    for (const fs::path &path : SystemUtil::fonts()) {
        foundFilePaths_.push(path);
    }
}

FontUpdater::UpdateWorker::UpdateWorker(FontUpdater &outer, FontDb &db)
    : stopping_(outer.stopping_)
    , foundFilePaths_(outer.foundFilePaths_)
    , stmtLastWriteTimeOf_(db)
    , stmtDeleteFile_(db)
    , stmtDeleteFaces_(db)
    , stmtInsertFile_(db)
    , stmtInsertFace_(db)
{
    thread_ = std::thread([this] { loop(); });
}

FontUpdater::UpdateWorker::~UpdateWorker()
{
    thread_.join();
}

void FontUpdater::UpdateWorker::loop()
{
    ThreadUtil::setNameForCurrentThread("FontUpdater-UpdateWorker");

    while (!stopping_) {
        opt<fs::path> pathOpt = foundFilePaths_.pop();
        if (pathOpt) {
            updateFont(*pathOpt);
        }
    }
}

static i64 lastWriteTimeOf(const fs::path &path)
{
    return fs::last_write_time(path).time_since_epoch().count();
}

static FaceInfo mkFaceInfo(const fs::path &fontFilePath, const FontFace &fontFace)
{
    FaceInfo info;
    info.setFilePath(fontFilePath);
    info.setFaceId(fontFace.faceIndex());
    info.setFamily(fontFace.familyName());
    info.setIsScalable(fontFace.isScalable());
    info.setIsBold(fontFace.isBold());
    info.setIsItalic(fontFace.isItalic());
    return info;
}

void FontUpdater::UpdateWorker::updateFont(const fs::path &p)
{
    const char *title = "FontUpdater::UpdateWorker::updateFont ";

    const opt<i64> writeTimeInDb = stmtLastWriteTimeOf_(p);
    const i64 writeTimeInFs = lastWriteTimeOf(p);
    if (writeTimeInDb) {
        if (*writeTimeInDb == writeTimeInFs) {
            LOGD << title << "font file [" << p << "] is not changed, skip it";
            return;
        } else {
            LOGI << title << "font file [" << p << "] is changed";
            stmtDeleteFaces_(p);
            stmtDeleteFile_(p);
        }
    } else {
        LOGI << title << "font file [" << p << "] is new";
    }

    FontFile fontFile(context_, p);
    if (!fontFile.isValid()) {
        return;
    }

    const i64 faceCount = fontFile.faceCount();
    if (faceCount <= 0) {
        return;
    }

    stmtInsertFile_(p, writeTimeInFs);

    for (i64 faceIndex = 0; faceIndex < faceCount; ++faceIndex) {
        const FontFace fontFace(fontFile, faceIndex);
        if (fontFace.isValid()) {
            FaceInfo info = mkFaceInfo(p, fontFace);
            stmtInsertFace_(info);
        }
    }
}

}
