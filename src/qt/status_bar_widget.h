#pragma once

#include <QWidget>

class TextAreaView;

class StatusBarWidget : public QWidget
{
public:
	explicit StatusBarWidget(TextAreaView *view, QWidget * parent = nullptr);

	virtual QSize sizeHint() const override;
	virtual void paintEvent(QPaintEvent *e) override;

private:
	TextAreaView &view_;
};
