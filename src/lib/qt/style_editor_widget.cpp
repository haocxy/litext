#include "style_editor_widget.h"

#include "gen.style_editor.ui.h"


namespace gui::qt
{

StyleEditorWidget::StyleEditorWidget(QWidget *parent)
    : QWidget(parent) {

    ui_ = new Ui::StyleEditor;
    ui_->setupUi(this);
}

StyleEditorWidget::~StyleEditorWidget()
{
    delete ui_;
    ui_ = nullptr;
}

}
