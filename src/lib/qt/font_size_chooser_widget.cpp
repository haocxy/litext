#include "font_size_chooser_widget.h"

#include "gen.font_size_chooser_popup.ui.h"


namespace gui::qt
{

FontSizeChooserWidget::FontSizeChooserWidget(QWidget *parent)
    : PopupWidget(parent)
    , ui_(new Ui::FontSizeChooser)
{
    ui_->setupUi(this);
}

FontSizeChooserWidget::~FontSizeChooserWidget()
{
    delete ui_;
    ui_ = nullptr;
}

}
