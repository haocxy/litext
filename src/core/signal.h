#pragma once

#include <boost/signals2/signal.hpp>

template <typename F>
using Signal = boost::signals2::signal<F>;


