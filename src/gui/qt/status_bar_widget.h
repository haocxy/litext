#pragma once

#include <QWidget>


class TextArea;

class StatusBarWidget : public QWidget
{
public:
	explicit StatusBarWidget(TextArea *view, QWidget * parent = nullptr);

	virtual QSize sizeHint() const override;
	virtual void paintEvent(QPaintEvent *e) override;

private:
	TextArea &view_;
};
