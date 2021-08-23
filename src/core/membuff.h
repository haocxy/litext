#pragma once

#include "basetype.h"

#include <cstring>


class MemBuff
{
public:

    MemBuff() {}

    explicit MemBuff(i64 capacity) { reverse(capacity); }

    MemBuff(const void *buff, i64 len) { write(buff, len); }

    MemBuff(const MemBuff &b) {
        write(b.data(), b.size());
    }

    MemBuff(MemBuff &&b) noexcept : beg_(b.beg_), size_(b.size_), capacity_(b.capacity_) {
        b.beg_ = nullptr;
        b.size_ = 0;
        b.capacity_ = 0;
    }

    MemBuff &operator=(const MemBuff &b) {
        clear();
        write(b.data(), b.size());
        return *this;
    }

    MemBuff &operator=(MemBuff &&b) noexcept {
        if (this != &b) {
            delete[] beg_;
            beg_ = b.beg_;
            b.beg_ = nullptr;

            size_ = b.size_;
            b.size_ = 0;

            capacity_ = b.capacity_;
            b.capacity_ = 0;
        }
        return *this;
    }

    ~MemBuff() {
        delete[] beg_;
        beg_ = nullptr;
        size_ = 0;
        capacity_ = 0;
    }

    void clear() { size_ = 0; }

    const void *data() const { return beg_; }

    void *data() { return beg_; }

    i64 size() const { return size_; }

    void resize(i64 len);

    i64 read(i64 offset, void *to, i64 len) const;

    i64 read(void *to, i64 len) const { return read(0, to, len); }

    i64 remove(i64 offset, i64 len);

    i64 popFront(i64 len) {
        return remove(0, len);
    }

    i64 take(i64 offset, void *to, i64 len);

    void write(i64 offset, const void *data, i64 len);

    void write(const void *data, i64 len) { write(0, data, len); }

    void reverse(i64 len) { ensureCapacity(len); }

    void append(const void *from, i64 len);

private:
    using byte = unsigned char;

    static_assert(sizeof(unsigned char) == 1);
    static_assert(sizeof(byte) == 1);

    static i64 calcRealNewCapacity(
        i64 curCapacity,
        i64 newCapacity);

    void ensureCapacity(i64 required) {
        if (capacity_ >= required) {
            return;
        }
        realloc(calcRealNewCapacity(capacity_, required));
    }

    void realloc(i64 newcapacity);

private:
    byte *beg_ = nullptr;
    i64 size_ = 0;
    i64 capacity_ = 0;
};

