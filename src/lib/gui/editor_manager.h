#pragma once

#include <set>
#include <map>
#include <mutex>
#include <functional>

#include "core/fs.h"
#include "core/basetype.h"
#include "editor/editor.h"


namespace gui
{

class ObjAsyncCreator;


// 统一管理为每个文档构造的Editor对象
// 每个文档同时只能构造一个Editor对象
//
// 若某个对象需要通过EditorManager构造Editor对象,
// 这个对象必须:
// 在其构造函数中调用registerReciver(this)
// 
class EditorManager {
public:
    EditorManager(ObjAsyncCreator &objAsyncCreator);

    ~EditorManager();

    void asyncGet(void *objReceiver, const fs::path &file, std::function<void(std::shared_ptr<Editor>)> &&cb);

    void registerReciver(void *objReceiver);

    void removeReciver(void *objReceiver);

private:
    void deleteEditor(Editor *p);

private:
    using Mtx = std::mutex;
    using Lock = std::lock_guard<Mtx>;
    mutable Mtx mtx_;
    ObjAsyncCreator &objAsyncCreator_;
    std::map<fs::path, std::weak_ptr<Editor>> editors_;
    std::set<void *> objReceivers_;
};

}
