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
    void qtSigUpdateStatus(const QString &status);

private slots:
    void qtSlotCharsetDetect(const QString &charset);
    void qtSlotUpdateStatus(const QString &status);

private:
	TextArea &textArea_;
    QString status_{"Loading"};
    QString charset_;
    QString filesize_;
	QString content_;
    Slot slotCharsetDetected_;
    Slot slotPartLoaded_;
    Slot slotAllLoaded_;
};


}
