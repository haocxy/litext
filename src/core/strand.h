#pragma once

#include <functional>


class Strand {
public:
	virtual ~Strand() {}

	virtual void post(std::function<void()> &&work) = 0;
};
