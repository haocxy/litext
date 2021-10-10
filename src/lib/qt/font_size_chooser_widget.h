#pragma once

#include "popup_widget.h"

#include "core/sigconns.h"
#include "gui/declare_text_area.h"


namespace Ui
{
class FontSizeChooser;
}

namespace gui::qt
{

class FontSizeChooserWidget : public PopupWidget {
    Q_OBJECT
public:
    explicit FontSizeChooserWidget(TextArea &area, QWidget *parent = nullptr);

    virtual ~FontSizeChooserWidget();

    void setCurrentFontSize(int pt);

public:
signals:
    void fontSizeChoosed(int pt);

private:
signals:
    void qtSigFontSizeUpdated(int pt);

private:
    TextArea &area_;
    Ui::FontSizeChooser *ui_ = nullptr;
    SigConns sigConns_;
    int fontSizeMin_ = 10;
    int fontSizeMax_ = 200;
};

}
