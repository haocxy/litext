#include "style_editor_window.h"


#include "gen.style_editor.ui.h"


namespace gui::qt
{

StyleEditorWindow::StyleEditorWindow() {
    ui_ = new Ui::StyleEditor;
    ui_->setupUi(this);
}

StyleEditorWindow::~StyleEditorWindow()
{
    delete ui_;
    ui_ = nullptr;
}

}
