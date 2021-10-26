#include "engine.h"

#include "core/system.h"
#include "core/logger.h"
#include "doc/dbfiles.h"


namespace gui
{

static i32 decideDocumentStrandPoolSize()
{
    return std::max(2, SystemUtil::processorCount() / 2);
}

Engine::Engine()
    : singletonServer_(SingletonServer::newObj())
    , documentStrandPool_("DocumentStrandPool", decideDocumentStrandPoolSize())
    , editorManager_(documentStrandPool_,objAsyncCreator_, objAsyncDeleter_)
    , fontRepo_(dirManager_.fontDbFile())
{
}

void Engine::init(const InitInfo &initInfo)
{
    initLogger(initInfo);

    cleanUseless();

    if (initInfo.shouldStartAsServer()) {
        initSingletonServer();
    }

    //fontRepo_.test(); // TODO test code
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

    singletonServer_->start(info);
}

}
