#pragma once

template <typename T>
class Range {
public:
    Range() {}

    template <typename U>
    Range(const Range<U> &b)
        : off_(b.off()), len_(b.len()) {}

    template <typename U>
    Range &operator=(const Range<U> &b) {
        off_ = b.off();
        len_ = b.len();
        return *this;
    }

    static Range byOffAndLen(T off, T len) {
        return Range(off, len);
    }

    static Range byBegAndEnd(T beg, T end) {
        return Range(beg, end - beg);
    }

    static Range byLeftAndRight(T left, T right) {
        return Range(left, right - left + 1);
    }

    static Range byCount(T count) {
        return Range(0, count);
    }

    Range shift(T delta) {
        return Range(off_ + delta, len_);
    }

    bool empty() const {
        return len_ <= 0;
    }

    bool operator==(const Range &b) const {
        return off_ == b.off_ && len_ == b.len_;
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

    T count() const {
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

    bool contains(T n) const {
        return left() <= n && n <= right();
    }

private:
    Range(T off, T len)
        : off_(off), len_(len) {}

private:
    T off_ = T();
    T len_ = T();

    friend class Ranges;
};


class Ranges {
public:

    template <typename T>
    static Range<T> byOffAndLen(T off, T len) {
        return Range<T>(off, len);
    }

    template <typename T>
    static Range<T> byBegAndEnd(T beg, T end) {
        return Range<T>(beg, end - beg);
    }

    template <typename T>
    static Range<T> byLeftAndRight(T left, T right) {
        return Range<T>(left, right - left + 1);
    }

    template <typename T>
    static Range<T> byCount(T count) {
        return Range<T>(0, count);
    }

};
