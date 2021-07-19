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

private:
    void updateContent();

private:
signals:
    void qtSigCharsetDetected(const QString &charset);
    void qtSigAllLoaded();

private slots:
    void qtSlotCharsetDetect(const QString &charset);
    void qtSlotAllLoaded();

private:
	TextArea &textArea_;
    QString status_{"loading"};
    QString charset_;
    QString filesize_;
	QString content_;
    Slot slotCharsetDetected_;
    Slot slotAllLoaded_;
};


}
