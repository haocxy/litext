#include "work_dir_manager.h"

#include <fstream>

#include "core/system.h"


static void createFile(const fs::path &f)
{
    std::ofstream ofs(f, std::ios::binary);
}

static void ensureFileExsit(const fs::path &f)
{
    if (fs::exists(f)) {
        return;
    }

    fs::create_directories(f.parent_path());
    createFile(f);
}


DirManager::DirManager()
    : workDir_(SystemUtil::userHome() / LITEXT_WORKDIR_NAME)
{
}

fs::path DirManager::singletonLockFile() const
{
    const fs::path f = workDir_ / "single.lock";
    ensureFileExsit(f);
    return f;
}

fs::path DirManager::singletonServerInfoFileLock() const
{
    const fs::path f = workDir_ / "server_info.lock";
    ensureFileExsit(f);
    return f;
}

fs::path DirManager::singletonServerInfoFile() const
{
    const fs::path f = workDir_ / "server_info.txt";
    ensureFileExsit(f);
    return f;
}
