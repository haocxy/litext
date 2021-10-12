#include "font_size_chooser_widget.h"

#include <QIntValidator>

#include "gui/text_area.h"

#include "gen.font_size_chooser_popup.ui.h"


namespace gui::qt
{

using Class = FontSizeChooserWidget;

FontSizeChooserWidget::FontSizeChooserWidget(TextArea &area, QWidget *parent)
    : PopupWidget(parent)
    , area_(area)
    , ui_(new Ui::FontSizeChooser)
{
    ui_->setupUi(this);

    ui_->fontSizeRangeMin->setText(QString::number(fontSizeMin_));
    ui_->fontSizeRangeMax->setText(QString::number(fontSizeMax_));
    ui_->fontSizeSlider->setRange(fontSizeMin_, fontSizeMax_);

    const int pt = area_.fontSizeByPoint();

    ui_->fontSizeSlider->setValue(pt);

    sigConns_ += area_.sigFontSizeUpdated().connect([this](int pt) {
        emit qtSigFontSizeUpdated(pt);
    });

    connect(this, &Class::qtSigFontSizeUpdated, this, [this](int pt) {
        setCurrentFontSize(pt);
    });

    connect(ui_->fontSizeSlider, &QSlider::valueChanged, this, [this](int pt) {
        area_.setFontSizeByPoint(pt);
    });

    ui_->fontSizeEdit->setText(QString::number(pt));

    ui_->fontSizeEdit->setValidator(new QIntValidator(fontSizeMin_, fontSizeMax_));

    connect(ui_->fontSizeEdit, &QLineEdit::returnPressed, this, [this] {
        const QString text = ui_->fontSizeEdit->text();
        setFontSizeByText(text);
    });

    connect(ui_->fontSizeOkButton, &QPushButton::clicked, this, [this] {
        const QString text = ui_->fontSizeEdit->text();
        setFontSizeByText(text);
    });
}

FontSizeChooserWidget::~FontSizeChooserWidget()
{
    delete ui_;
    ui_ = nullptr;
}

void FontSizeChooserWidget::setCurrentFontSize(int pt)
{
    // 在向每个控件设置值时,必须先判断,只有值不同才设置,避免信号的循环触发


    if (pt != ui_->fontSizeSlider->value()) {
        ui_->fontSizeSlider->setValue(pt);
    }

    {
        const QString text = ui_->fontSizeEdit->text();
        if (text.isEmpty() || text.toInt() != pt) {
            ui_->fontSizeEdit->setText(QString::number(pt));
        }
    }
}

void FontSizeChooserWidget::setFontSizeByText(const QString &text)
{
    if (!text.isEmpty()) {
        const int value = text.toInt();
        if (fontSizeMin_ <= value && value <= fontSizeMax_) {
            area_.setFontSizeByPoint(value);
        }
    }
}

}
