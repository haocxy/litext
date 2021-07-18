#pragma once

#include <boost/signals2.hpp>


template <typename F>
using Callbacks = boost::signals2::signal<F>;

using CallbackHandle = boost::signals2::scoped_connection;
