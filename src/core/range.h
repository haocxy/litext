#pragma once


template <typename T>
class Range {
public:
    Range() {}

    static Range byOffAndLen(T off, T len) {
        return Range(off, len);
    }

    static Range byBegAndEnd(T beg, T end) {
        return Range(beg, end - beg);
    }

    static Range byLeftAndRight(T left, T right) {
        return Range(left, right - left + 1);
    }

    T off() const {
        return beg_;
    }

    T len() const {
        return len_;
    }

    T beg() const {
        return beg_;
    }

    T end() const {
        return beg_ + len_;
    }

    T left() const {
        return beg_;
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
    Range(T beg, T len)
        : beg_(beg), len_(len) {}

private:
    T beg_ = T();
    T len_ = T();
};
