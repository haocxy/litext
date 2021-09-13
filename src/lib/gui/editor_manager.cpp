#include "editor_manager.h"

#include "obj_async_creator.h"


namespace gui
{

EditorManager::EditorManager(ObjAsyncCreator &objAsyncCreator)
    : objAsyncCreator_(objAsyncCreator)
{
}

EditorManager::~EditorManager()
{
}

void EditorManager::asyncGet(void *objReceiver, const fs::path &file, std::function<void(std::shared_ptr<Editor>)> &&cb)
{
    Lock lock(mtx_);

    if (objReceivers_.find(objReceiver) == objReceivers_.end()) {
        throw std::logic_error("must call EditorManager::registerReciver() first");
    }

    const fs::path absPath = fs::absolute(file);
    auto it = editors_.find(absPath);
    if (it != editors_.end()) {
        cb(it->second.lock());
        return;
    }

    objAsyncCreator_.asyncCreate<Editor, fs::path>(absPath, [this, objReceiver, absPath, cb] (Editor *p) {

        std::shared_ptr<Editor> editor;

        {
            Lock lock(mtx_);

            if (objReceivers_.find(objReceiver) == objReceivers_.end()) {
                return;
            }

            editor = std::shared_ptr<Editor>(p, [this, absPath, cb](Editor *p) {
                deleteEditor(p);
            });

            editors_.insert({ absPath, editor });
        }

        cb(editor);
    });
}

void EditorManager::registerReciver(void *objReceiver)
{
    Lock lock(mtx_);
    objReceivers_.insert(objReceiver);
}

void EditorManager::removeReciver(void *objReceiver)
{
    Lock lock(mtx_);
    objReceivers_.erase(objReceiver);
}

void EditorManager::deleteEditor(Editor *p) {
    Lock lock(mtx_);
    editors_.erase(p->document().path());
    delete p;
}

}
