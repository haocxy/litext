#pragma once


template <typename T>
class IdGen {
public:
    IdGen(T first)
        : id_(first) {}

    T next() {
        return id_++;
    }

private:
    T id_;
};
