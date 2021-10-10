#pragma once

#include <QWidget>

namespace gui::qt
{

class FontSizeChooserWidget : public QWidget {
    Q_OBJECT
public:
    explicit FontSizeChooserWidget(QWidget *parent = nullptr);

    virtual ~FontSizeChooserWidget();

private:

};

}
