#pragma once

#include <functional>

#include "block_queue.h"


template <typename F>
using TaskQueue = BlockQueue<std::function<F>>;
