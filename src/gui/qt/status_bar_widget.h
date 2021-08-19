#pragma once

#include <QWidget>

#include "core/sigconns.h"
#include "core/basetype.h"
#include "gui/declare_text_area.h"


namespace gui::qt
{


class StatusBarWidget : public QWidget {
	Q_OBJECT
public:
	explicit StatusBarWidget(TextArea &view);

	virtual QSize sizeHint() const override;

	virtual void paintEvent(QPaintEvent *e) override;

private:
    void updateContent();

private:
signals:
    void qtSigCharsetDetected(const QString &charset);
    void qtSigUpdateStatus(const QString &status);
    void qtSigRowCountUpdated(const QString &rowCount);

private slots:
    void qtSlotCharsetDetect(const QString &charset);
    void qtSlotUpdateStatus(const QString &status);
    void qtSlotRowCountUpdated(const QString &rowCount);

private:
	TextArea &textArea_;
    QString status_;
    QString charset_;
    QString filesize_;
    QString rowCount_;
	QString content_;
    SigConns sigConns_;
    // 因为多线程处理，进度的报告也是乱序的，
    // 所以需要记录收到的最大偏移量
    i64 maxOffset_ = -1;
};


}
