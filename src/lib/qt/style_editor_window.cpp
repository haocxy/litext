#include "style_editor_window.h"

#include <QAction>
#include <QStyle>

#include "gen.style_editor.ui.h"


namespace gui::qt
{

StyleEditorWindow::StyleEditorWindow(Application &application)
    : ui_(new Ui::StyleEditor)
    , application_(application) {

    ui_->setupUi(this);

    initToolBar();

    ui_->applyButton->setIcon(style()->standardIcon(QStyle::StandardPixmap::SP_DialogApplyButton));
}

StyleEditorWindow::~StyleEditorWindow()
{
    delete ui_;
    ui_ = nullptr;
}

void StyleEditorWindow::initToolBar()
{
    QToolBar *bar = ui_->toolBar;

    QStyle *style = this->style();

    using Pix = QStyle::StandardPixmap;

    auto bind = [bar](const QString &name, const QIcon &icon) {
        QAction *action = bar->addAction(name);
        action->setIcon(icon);
    };

    auto qtIcon = [style](QStyle::StandardPixmap id) {
        return style->standardIcon(id);
    };

    bind(tr("New"), qtIcon(Pix::SP_FileIcon));

    bind(tr("Open"), qtIcon(Pix::SP_DialogOpenButton));

    bind(tr("Save"), qtIcon(Pix::SP_DialogSaveButton));

    bind(tr("SaveAs"), qtIcon(Pix::SP_DriveFDIcon));

    bar->addSeparator();

    bind(tr("Undo"), qtIcon(Pix::SP_MediaSeekBackward));

    bind(tr("Redo"), qtIcon(Pix::SP_MediaSeekForward));

    bar->addSeparator();

    bind(tr("Reset"), qtIcon(Pix::SP_DialogCancelButton));

    
}

void StyleEditorWindow::openFileAction()
{
}

}
