#pragma once

#include <QWidget>

namespace gui
{
class TextArea;
}

class StatusBarWidget : public QWidget
{
public:
	explicit StatusBarWidget(gui::TextArea *view, QWidget * parent = nullptr);

	virtual QSize sizeHint() const override;
	virtual void paintEvent(QPaintEvent *e) override;

private:
	gui::TextArea &view_;
};
