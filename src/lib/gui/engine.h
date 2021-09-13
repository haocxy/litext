#pragma once

#include "global/work_dir_manager.h"

#include "config.h"
#include "propdb.h"
#include "init_info.h"
#include "editor_manager.h"
#include "obj_async_creator.h"
#include "singleton_server.h"


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
        return singletonServer_;
    }

    SingletonServer &singletonServer() {
        return singletonServer_;
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

    const EditorManager &editorManager() const {
        return editorManager_;
    }

    EditorManager &editorManager() {
        return editorManager_;
    }

private:
    void initLogger(const InitInfo &initInfo);

    // 清理程序运行过程中生成的内部无用文件
    void cleanUseless();

    void initSingletonServer();

private:
    DirManager dirManager_;
    Config config_;
    SingletonServer singletonServer_;
    PropDb propDb_;
    ObjAsyncCreator objAsyncCreator_;
    EditorManager editorManager_;
};

}
