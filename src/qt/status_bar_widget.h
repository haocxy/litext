#pragma once

#include <QWidget>

class View;

class StatusBarWidget : public QWidget
{
public:
	explicit StatusBarWidget(View * view, QWidget * parent = nullptr);

	virtual QSize sizeHint() const override;
	virtual void paintEvent(QPaintEvent *e) override;

private:
	View &view_;
};
