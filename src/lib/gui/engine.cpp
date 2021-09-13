#include "engine.h"

#include "core/logger.h"
#include "doc/dbfiles.h"


namespace gui
{

Engine::Engine()
    : editorManager_(objAsyncCreator_, objAsyncDeleter_)
{
}

void Engine::init(const InitInfo &initInfo)
{
    initLogger(initInfo);

    cleanUseless();

    if (initInfo.shouldStartAsServer()) {
        initSingletonServer();
    }
}

void Engine::initLogger(const InitInfo &initInfo)
{
    logger::control::Option logOpt;
    logOpt.setLevel(initInfo.logLevel());
    logOpt.setDir("./tmp/log");
    logOpt.setBasename("litextlog");
    logOpt.setWriteToStdout(initInfo.shouldLogToStd());
    logger::control::init(logOpt);
}

void Engine::cleanUseless()
{
    doc::dbfiles::removeUselessDbFiles();
}

void Engine::initSingletonServer()
{
    SingletonServer::StartInfo info;
    info.serverRunningLock = dirManager_.singletonServerRunningLockFile();
    info.infoFile = dirManager_.singletonServerInfoFile();
    info.infoFileLock = dirManager_.singletonServerInfoFileLock();

    singletonServer_.start(info);
}

}
