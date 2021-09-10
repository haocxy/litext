#pragma once

#include <map>

#include "core/fs.h"
#include "core/basetype.h"
#include "editor/editor.h"


namespace gui
{

// 统一管理为每个文档构造的Editor对象
// 每个文档同时只能构造一个Editor对象
class EditorManager {
public:
    EditorManager() {}

    sptr<Editor> get(const fs::path &file) {
        const fs::path absPath = fs::absolute(file);
        auto it = editors_.find(absPath);
        if (it != editors_.end()) {
            return it->second.lock();
        }

        sptr<Editor> editor(new Editor(absPath), [this](Editor *p) {
            deleteEditor(p);
        });
        editors_.insert({ absPath, editor });
        return editor;
    }

private:
    void deleteEditor(Editor *p) {
        editors_.erase(p->document().path());
        delete p;
    }

private:
    std::map<fs::path, std::weak_ptr<Editor>> editors_;
};

}
