#pragma once

#include <memory>

#include "core/thread.h"

#include "global/work_dir_manager.h"

#include "config.h"
#include "propdb.h"
#include "init_info.h"
#include "editor_manager.h"
#include "obj_async_creator.h"
#include "obj_async_deleter.h"
#include "singleton_server.h"
#include "font/font_repo.h"


namespace gui
{

// 所有平台无关逻辑的根
// 所有平台无关的模块都直接或间接地由这个类的实例管理
class Engine {
public:
    Engine();

    void init(const InitInfo &initInfo);

    const DirManager &dirManager() const {
        return dirManager_;
    }

    DirManager &dirManager() {
        return dirManager_;
    }

    const Config &config() const {
        return config_;
    }

    Config &config() {
        return config_;
    }

    const SingletonServer &singletonServer() const {
        return *singletonServer_;
    }

    SingletonServer &singletonServer() {
        return *singletonServer_;
    }

    const PropDb &propDb() const {
        return propDb_;
    }

    PropDb &propDb() {
        return propDb_;
    }

    ObjAsyncCreator &objAsyncCreator() {
        return objAsyncCreator_;
    }

    ObjAsyncDeleter &objAsyncDeleter() {
        return objAsyncDeleter_;
    }

    const EditorManager &editorManager() const {
        return editorManager_;
    }

    EditorManager &editorManager() {
        return editorManager_;
    }

    StrandPool &documentStrandPool() {
        return documentStrandPool_;
    }

private:
    void initLogger(const InitInfo &initInfo);

    // 清理程序运行过程中生成的内部无用文件
    void cleanUseless();

    void initSingletonServer();

private:
    DirManager dirManager_;
    Config config_;
    std::unique_ptr<SingletonServer> singletonServer_;
    PropDb propDb_;
    StrandPool documentStrandPool_;
    ObjAsyncCreator objAsyncCreator_;
    ObjAsyncDeleter objAsyncDeleter_;
    EditorManager editorManager_;
    font::FontRepo fontRepo_;
    
};

}
