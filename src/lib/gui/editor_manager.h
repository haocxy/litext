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

    Editor &operator[](const fs::path &file) {
        auto it = editors_.find(file);
        if (it != editors_.end()) {
            return *(it->second);
        }

        Editor *editor = new Editor(file);
        editors_.insert({ file, uptr<Editor>(editor) });
        return *editor;
    }

private:
    std::map<fs::path, uptr<Editor>> editors_;
};

}
