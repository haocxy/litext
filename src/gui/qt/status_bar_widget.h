#pragma once

#include <QWidget>

#include "core/signal.h"
#include "gui/declare_text_area.h"


namespace gui::qt
{


class StatusBarWidget : public QWidget {
	Q_OBJECT
public:
	explicit StatusBarWidget(TextArea &view, QWidget *parent = nullptr);

	virtual QSize sizeHint() const override;

	virtual void paintEvent(QPaintEvent *e) override;

	void setContent(QString &&content);

private:
	TextArea &view_;
	QString content_;
	Slot cbhCharsetDetected_;
};


}
