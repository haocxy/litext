#pragma once

#include <QWidget>

#include "gui/declare_text_area.h"


class StatusBarWidget : public QWidget
{
public:
	explicit StatusBarWidget(gui::TextArea *view, QWidget * parent = nullptr);

	virtual QSize sizeHint() const override;
	virtual void paintEvent(QPaintEvent *e) override;

private:
	gui::TextArea &view_;
};
