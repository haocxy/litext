#include "style_editor_window.h"

#include <QAction>
#include <QStyle>
#include <QShortcut>

#include "application.h"
#include "style_edit_widget.h"

#include "gen.style_editor.ui.h"


namespace gui::qt
{

using Class = StyleEditorWindow;

StyleEditorWindow::StyleEditorWindow(Application &application)
    : application_(application)
    , ui_(new Ui::StyleEditor)
    , styleEdit_(new StyleEditWidget) {

    ui_->setupUi(this);

    ui_->centralAreaLayout->insertWidget(0, styleEdit_);

    initToolBar();

    ui_->applyButton->setIcon(style()->standardIcon(QStyle::StandardPixmap::SP_DialogApplyButton));
    connect(ui_->applyButton, &QPushButton::clicked, this, &Class::applyAction);
    connect(styleEdit_, &StyleEditWidget::shouldApply, this, &Class::applyAction);

    styleEdit_->setText(application_.styleSheet());
}

StyleEditorWindow::~StyleEditorWindow()
{
    delete styleEdit_;
    styleEdit_ = nullptr;

    delete ui_;
    ui_ = nullptr;
}

void StyleEditorWindow::initToolBar()
{
    QToolBar *bar = ui_->toolBar;

    QStyle *style = this->style();

    using Pix = QStyle::StandardPixmap;

    using Class = StyleEditorWindow;

    auto bind = [bar, this](const QString &name, const QIcon &icon, void (Class::*f)()) {
        QAction *action = bar->addAction(name);
        action->setIcon(icon);
        connect(action, &QAction::triggered, this, f);
    };

    auto qtIcon = [style](QStyle::StandardPixmap id) {
        return style->standardIcon(id);
    };

    bind(tr("New"), qtIcon(Pix::SP_FileIcon), &Class::noAction);

    bind(tr("Open"), qtIcon(Pix::SP_DialogOpenButton), &Class::noAction);

    bind(tr("Save"), qtIcon(Pix::SP_DialogSaveButton), &Class::noAction);

    bind(tr("SaveAs"), qtIcon(Pix::SP_DriveFDIcon), &Class::noAction);

    bar->addSeparator();

    bind(tr("Undo"), qtIcon(Pix::SP_MediaSeekBackward), &Class::noAction);

    bind(tr("Redo"), qtIcon(Pix::SP_MediaSeekForward), &Class::noAction);

    bar->addSeparator();

    bind(tr("Reset"), qtIcon(Pix::SP_DialogCancelButton), &Class::noAction);

    
}

void StyleEditorWindow::openFileAction()
{
}

void StyleEditorWindow::applyAction()
{
    application_.setStyleSheet(styleEdit_->content());
}

}
