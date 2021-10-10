#pragma once

#include "popup_widget.h"


namespace gui::qt
{

class FontSizeChooserWidget : public PopupWidget {
    Q_OBJECT
public:
    explicit FontSizeChooserWidget(QWidget *parent = nullptr);

    virtual ~FontSizeChooserWidget();

private:

};

}
