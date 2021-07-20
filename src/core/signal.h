#pragma once

#include <vector>
#include <boost/signals2/signal.hpp>
#include <boost/signals2/connection.hpp>


template <typename F>
using Signal = boost::signals2::signal<F>;

using Slot = boost::signals2::connection;

class SigConns {
public:
    SigConns() {}

    ~SigConns() {
        for (Slot &slot : slots_) {
            slot.disconnect();
        }
    }

    void operator+=(Slot &&slot) {
        slots_.push_back(std::move(slot));
    }

    void disconnectAll() {
        for (Slot &slot : slots_) {
            slot.disconnect();
        }
        slots_.clear();
    }

private:
    std::vector<Slot> slots_;
};
