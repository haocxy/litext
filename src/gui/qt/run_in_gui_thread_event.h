#pragma once

#include <cassert>
#include <functional>

#include <QEvent>


namespace gui::qt
{


class RunInGuiThreadEvent : public QEvent {
public:
	static QEvent::Type g_type;

	RunInGuiThreadEvent(std::function<void()> &&action)
		: QEvent(g_type)
		, action_(std::move(action)) {

		assert(g_type != QEvent::None);
	}

	virtual ~RunInGuiThreadEvent() {

	}

	void call() {
		if (action_) {
			action_();
		}
	}

private:
	std::function<void()> action_;
};


}
