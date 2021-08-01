#pragma once

template <typename T>
class Range {
public:
    Range() {}

    Range(T off, T len)
        : off_(off), len_(len) {}

    template <typename U>
    Range(const Range<U> &b)
        : off_(b.off()), len_(b.len()) {}

    template <typename U>
    Range &operator=(const Range<U> &b) {
        off_ = b.off();
        len_ = b.len();
        return *this;
    }

    T off() const {
        return off_;
    }

    void setOff(T n) {
        off_ = n;
    }

    T len() const {
        return len_;
    }

    void setLen(T n) {
        len_ = n;
    }

    T beg() const {
        return off_;
    }

    T end() const {
        return off_ + len_;
    }

    T left() const {
        return off_;
    }

    T right() const {
        return end() - 1;
    }

    bool isIntersect(const Range &other) const {
        if (right() < other.left()) {
            return false;
        }
        if (left() > other.right()) {
            return false;
        }
        return true;
    }

private:
    T off_ = T();
    T len_ = T();
};


namespace Ranges
{

template <typename T>
inline Range<T> byOffAndLen(T off, T len) {
    return Range<T>(off, len);
}

template <typename T>
inline Range<T> byBegAndEnd(T beg, T end) {
    return Range<T>(beg, end - beg);
}

template <typename T>
inline Range<T> byLeftAndRight(T left, T right) {
    return Range<T>(left, right - left + 1);
}

}
