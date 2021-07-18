#pragma once

#include <functional>

#include "worker.h"


class Strand : public Worker {
public:
	virtual ~Strand() {}
};
