#pragma once

#include <QWidget>


namespace gui::qt
{

class MyWidget : public QWidget {
	Q_OBJECT
public:
	explicit MyWidget(QWidget *parent = nullptr) : QWidget(parent) {}

	virtual ~MyWidget() {}

	virtual bool event(QEvent *ev) override;
};

}
