#pragma once

#include <cstdint>
#include <istream>
#include <ostream>


// 定义强类型整数类型
// 包装整数的各种操作，防止不同整数类型的隐式转换，提升API的易用性
// 用宏定义而不是类模板的原因是，使用相同类型的类型参数定义的类是兼容的，
// 而通过宏定义的类型可以避免意外的兼容
//
// 使用示例：
// DEFINE_STRONG_INT_TYPE(UserID, int64_t);
// 这样即可定义一个名为UserID的类型，其内部使用int64_t
//
// TODO 目前提供的操作还不完整，用到了再加
#define DEFINE_STRONG_INT_TYPE(c, t)                                    \
class c                                                                 \
{                                                                       \
public:                                                                 \
    typedef t Raw;                                                      \
                                                                        \
    c() : n_(t()) {}                                                    \
                                                                        \
    template <typename T>                                               \
    explicit c(T n) : n_(n) {}                                          \
                                                                        \
    c(const c &b) : n_(b.n_) {}                                         \
                                                                        \
    c &operator=(const c &b) {                                          \
        n_ = b.n_;                                                      \
        return *this;                                                   \
    }                                                                   \
                                                                        \
    void set(t value) { n_ = value; }                                   \
                                                                        \
    t value() const { return n_; }                                      \
                                                                        \
    t &ref() { return n_; }                                             \
                                                                        \
    bool operator==(const c &b) const {                                 \
        return n_ == b.n_;                                              \
    }                                                                   \
                                                                        \
    bool operator<(const c &b) const {                                  \
        return n_ < b.n_;                                               \
    }                                                                   \
                                                                        \
    bool operator<=(const c &b) const {                                 \
        return n_ <= b.n_;                                              \
    }                                                                   \
                                                                        \
    bool operator>(const c &b) const {                                  \
        return n_ > b.n_;                                               \
    }                                                                   \
                                                                        \
    bool operator>=(const c &b) const {                                 \
        return n_ >= b.n_;                                              \
    }                                                                   \
                                                                        \
    c operator++(int) {                                                 \
        c old(*this);                                                   \
        ++n_;                                                           \
        return old;                                                     \
    }                                                                   \
                                                                        \
    c &operator++() {                                                   \
        ++n_;                                                           \
        return *this;                                                   \
    }                                                                   \
                                                                        \
    c &operator+=(c obj) {                                              \
        n_ += obj.n_;                                                   \
        return *this;                                                   \
    }                                                                   \
                                                                        \
    template <typename T>                                               \
    c &operator+=(T d) {                                                \
        n_ += d;                                                        \
        return *this;                                                   \
    }                                                                   \
                                                                        \
    c &operator-=(c obj) {                                              \
        n_ -= obj.n_;                                                   \
        return *this;                                                   \
    }                                                                   \
                                                                        \
    template <typename T>                                               \
    c &operator-=(T d) {                                                \
        n_ -= d;                                                        \
        return *this;                                                   \
    }                                                                   \
                                                                        \
    c operator+(c obj) const {                                          \
        return c(n_ + obj.n_);                                          \
    }                                                                   \
                                                                        \
    c operator-(c obj) const {                                          \
        return c(n_ - obj.n_);                                          \
    }                                                                   \
                                                                        \
    c operator*(c obj) const {                                          \
        return c(n_ * obj.n_);                                          \
    }                                                                   \
                                                                        \
    c operator/(c obj) const {                                          \
        return c(n_ / obj.n_);                                          \
    }                                                                   \
private:                                                                \
    t n_ = t();                                                         \
                                                                        \
    friend std::ostream &operator<<(std::ostream &s, const c &obj) {    \
        s << obj.n_;                                                    \
        return s;                                                       \
    }                                                                   \
                                                                        \
    friend std::istream &operator>>(std::istream &s, c &obj) {          \
        s >> obj.n_;                                                    \
        return s;                                                       \
    }                                                                   \
}
