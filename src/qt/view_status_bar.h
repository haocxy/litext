#pragma once

#include <QWidget>

class View;

class ViewStatusBar : public QWidget
{
public:
	explicit ViewStatusBar(View * view, QWidget * parent = nullptr);

	virtual QSize sizeHint() const override;
	virtual void paintEvent(QPaintEvent *e) override;

private:
	View & m_view;
};
