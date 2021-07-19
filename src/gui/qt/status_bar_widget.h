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

	void updateContent(QString &&content);

private:
signals:
    void qtSigAllLoaded();

private slots:
    void qtSlotAllLoaded();

private:
	TextArea &textArea_;
	QString content_;
    Slot slotAllLoaded_;
};


}
