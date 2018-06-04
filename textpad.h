#pragma once


#include <QWidget>

class QFont;
class QFontMetrics;
class DocumentModel;

class TextPad : public QWidget
{
    Q_OBJECT

public:
    TextPad(DocumentModel &model, QWidget *parent = 0);
    ~TextPad();

    virtual void keyPressEvent(QKeyEvent *e) override;
    virtual void paintEvent(QPaintEvent* e) override;
    virtual void inputMethodEvent(QInputMethodEvent *e) override;

public:
    void test();
    void SetContent(const QString &s);

private:
    bool IsFixWidthFont() const;
    void paintBackground(QPainter &p);
    void paintTextContent(QPainter &p);
    void paintTextContentES(QPainter &p);

private:
    DocumentModel & model_;
    QFont *font_ = nullptr;
    QFontMetrics *font_metrix_ = nullptr;

private:
    QString content_;

private:
    int32_t hgap_ = 5;
    int64_t insert_line_row_ = 0;
    int64_t insert_line_col_ = 0;
    bool _wrapLine = true;
};

