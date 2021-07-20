#pragma once

#include <boost/signals2/signal.hpp>
#include <boost/signals2/connection.hpp>

template <typename F>
using Signal = boost::signals2::signal<F>;

using SigConn = boost::signals2::connection;
