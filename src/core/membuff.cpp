#include "membuff.h"

#include <algorithm>


void MemBuff::resize(size_t len) {
    ensureCapacity(len);
    size_ = len;
}

size_t MemBuff::read(size_t offset, void *to, size_t len) const {
    if (size_ <= offset) {
        return 0;
    }

    const size_t readable = std::min(size_ - offset, len);
    std::memcpy(to, beg_ + offset, readable);
    return readable;
}

size_t MemBuff::remove(size_t offset, size_t len) {
    if (size_ <= offset) {
        return 0;
    }

    const size_t removable = std::min(size_ - offset, len);
    const size_t endOffset = offset + removable;
    if (endOffset < size_) {
        std::memmove(beg_ + offset, beg_ + endOffset, size_ - endOffset);
    }
    size_ -= removable;
    return removable;
}

size_t MemBuff::take(size_t offset, void *to, size_t len) {
    if (size_ <= offset) {
        return 0;
    }

    const size_t takable = std::min(size_ - offset, len);
    const size_t endOffset = offset + takable;
    std::memcpy(to, beg_ + offset, takable);
    if (endOffset < size_) {
        std::memmove(beg_ + offset, beg_ + endOffset, size_ - endOffset);
    }
    size_ -= takable;
    return takable;
}

void MemBuff::write(size_t offset, const void *data, size_t len) {
    ensureCapacity(offset + len);

    // 若当前字节数小于指定偏移,先用0字节把这段空白初始化
    if (size_ < offset) {
        std::memset(beg_ + size_, 0, offset - size_);
    }

    // 拷贝到目标内存区
    std::memcpy(beg_ + offset, data, len);

    // 如果目标区结束位置超出了当前字节数，则更新当前字节数
    size_ = std::max(size_, offset + len);
}

void MemBuff::append(const void *from, size_t len) {
    const size_t newbytecount = size_ + len;
    ensureCapacity(newbytecount);
    std::memcpy(beg_ + size_, from, len);
    size_ = newbytecount;
}

size_t MemBuff::calcRealNewCapacity(size_t curCapacity, size_t newCapacity) {
	constexpr size_t kInit = 256;
	size_t n = curCapacity == 0 ? kInit : curCapacity;
	while (n < newCapacity) {
		n *= 2;
	}
	return n;
}

void MemBuff::realloc(size_t newcapacity) {
    capacity_ = newcapacity;
    byte *ptr = new byte[capacity_];
    std::memcpy(ptr, beg_, size_);
    delete beg_;
    beg_ = ptr;
}
