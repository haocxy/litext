#pragma once

#include <algorithm>
#include <cstring>


class MemBuff
{
public:

    MemBuff() {}

    explicit MemBuff(size_t capacity) { reverse(capacity); }

    MemBuff(const void *buff, size_t len) { write(buff, len); }

    MemBuff(const MemBuff &b) { write(b.data(), b.size()); }

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

    size_t size() const { return size_; }

    void resize(size_t len);

    size_t read(size_t offset, void *to, size_t len) const;

    size_t read(void *to, size_t len) const { return read(0, to, len); }

    size_t remove(size_t offset, size_t len);

    size_t popFront(size_t len) {
        return remove(0, len);
    }

    size_t take(size_t offset, void *to, size_t len);

    void write(size_t offset, const void *data, size_t len);

    void write(const void *data, size_t len) { write(0, data, len); }

    void reverse(size_t len) { ensureCapacity(len); }

    void append(const void *from, size_t len);

private:
    using byte = unsigned char;

    static_assert(sizeof(unsigned char) == 1);
    static_assert(sizeof(byte) == 1);

    static size_t calcRealNewCapacity(
        size_t curCapacity,
        size_t newCapacity);

    void ensureCapacity(size_t required) {
        if (capacity_ >= required) {
            return;
        }
        realloc(calcRealNewCapacity(capacity_, required));
    }

    void realloc(size_t newcapacity);

private:
    byte *beg_ = nullptr;
    size_t size_ = 0;
    size_t capacity_ = 0;
};

