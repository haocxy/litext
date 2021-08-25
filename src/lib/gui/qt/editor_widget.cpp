#include "editor_widget.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

#include "gui/text_area.h"
#include "editor/editor.h"
#include "ruler_widget.h"
#include "text_area_widget.h"
#include "status_bar_widget.h"


namespace gui::qt
{

EditorWidget::EditorWidget(const TextAreaConfig &textAreaConfig, const fs::path &file, RowN row)
    : file_(file)
    , doc_(file)
    , editor_(&doc_, file)
    , textArea_(editor_, textAreaConfig)
{
    doc::Document &document = editor_.document();

    sigConns_ += document.sigFatalError().connect([this](doc::DocError err) {
        emit qtSigDocFatalError(docErrToStr(err));
    });

    ruler_ = new RulerWidget(textArea_);

    textAreaWidget_ = new TextAreaWidget(textArea_);

    vScrollBar_ = new QScrollBar;
    vScrollBar_->setRange(0, 0);

    statusBar_ = new StatusBarWidget(textArea_);

    QVBoxLayout *vlayout = new QVBoxLayout;
    vlayout->setMargin(0);
    vlayout->setSpacing(0);

    QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->setMargin(0);
    hlayout->setSpacing(0);
    hlayout->addWidget(ruler_);
    hlayout->addWidget(textAreaWidget_);
    hlayout->addWidget(vScrollBar_);

    vlayout->addLayout(hlayout);
    vlayout->addWidget(statusBar_);

    setLayout(vlayout);

    textArea_.open(Size(textAreaWidget_->width(), textAreaWidget_->height()), row);
}

EditorWidget::~EditorWidget()
{
    ruler_ = nullptr;
    textAreaWidget_ = nullptr;
    vScrollBar_ = nullptr;
    statusBar_ = nullptr;
}

void EditorWidget::jumpTo(RowN row)
{
    textArea_.jumpTo(row);
}

QString EditorWidget::docErrToStr(doc::DocError err) const
{
    switch (err) {
    case doc::DocError::RowTooBig:
        return tr("row too big");
    default:
        return QString();
    }
}

}
