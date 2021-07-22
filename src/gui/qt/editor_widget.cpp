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

static const int ProgressTotal = 1000000;

EditorWidget::EditorWidget(const TextAreaConfig &textAreaConfig, const fs::path &file)
    : file_(file)
    , doc_(file)
    , editor_(&doc_, file)
    , textArea_(editor_, textAreaConfig, gui::Size(800, 600))
{
    doc::Document &document = editor_.document();

    ruler_ = new RulerWidget(textArea_);

    textAreaWidget_ = new TextAreaWidget(textArea_);

    vScrollBar_ = new QScrollBar;
    vScrollBar_->setRange(0, 0);
    sigConns_ += document.sigPartLoaded().connect([this](const doc::PartLoadedEvent &p) {
        const double loaded = static_cast<double>(p.partOffset()) + static_cast<double>(p.partSize());
        const double total = static_cast<double>(p.fileSize());
        emit qtSigPartLoaded(loaded / total);
    });

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
}

EditorWidget::~EditorWidget()
{
    ruler_ = nullptr;
    textAreaWidget_ = nullptr;
    vScrollBar_ = nullptr;
    statusBar_ = nullptr;
}

void EditorWidget::qtSlotPartLoaded(double percent)
{

}

}
