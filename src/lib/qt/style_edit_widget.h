#pragma once

#include <QTextEdit>


namespace gui::qt
{

class StyleEditWidget : public QTextEdit {
    Q_OBJECT
public:
    explicit StyleEditWidget(QWidget *parent = nullptr);

    virtual ~StyleEditWidget();

    QString content() const;

public:
signals:
    void shouldApply();

protected:
    virtual void keyPressEvent(QKeyEvent *e) override;

private:
    void initFont();
};

}
