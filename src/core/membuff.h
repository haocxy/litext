#pragma once

#include "basetype.h"

#include <cstring>


class MemBuff
{
public:

    MemBuff() {}

    explicit MemBuff(i32 capacity) { reverse(capacity); }

    MemBuff(const void *buff, i32 len) { write(buff, len); }

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

    i32 size() const { return size_; }

    void resize(i32 len);

    i32 read(i32 offset, void *to, i32 len) const;

    i32 read(void *to, i32 len) const { return read(0, to, len); }

    i32 remove(i32 offset, i32 len);

    i32 popFront(i32 len) {
        return remove(0, len);
    }

    i32 take(i32 offset, void *to, i32 len);

    void write(i32 offset, const void *data, i32 len);

    void write(const void *data, i32 len) { write(0, data, len); }

    void reverse(i32 len) { ensureCapacity(len); }

    void append(const void *from, i32 len);

private:
    using byte = unsigned char;

    static_assert(sizeof(unsigned char) == 1);
    static_assert(sizeof(byte) == 1);

    static i32 calcRealNewCapacity(
        i32 curCapacity,
        i32 newCapacity);

    void ensureCapacity(i32 required) {
        if (capacity_ >= required) {
            return;
        }
        realloc(calcRealNewCapacity(capacity_, required));
    }

    void realloc(i32 newcapacity);

private:
    byte *beg_ = nullptr;
    i32 size_ = 0;
    i32 capacity_ = 0;
};

