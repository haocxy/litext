#pragma once

#include <QEvent>
#include <functional>


namespace gui::qt
{


class RunOnQObjectEvent : public QEvent {
public:
	static QEvent::Type g_type;

	RunOnQObjectEvent(std::function<void()> &&action)
		: QEvent(g_type)
		, action_(std::move(action)) {}

	void call() {
		if (action_) {
			action_();
		}
	}

private:
	std::function<void()> action_;
};


}
