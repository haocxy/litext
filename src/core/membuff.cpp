#include "membuff.h"

#include <algorithm>


void MemBuff::resize(i64 len) {
    ensureCapacity(len);
    size_ = len;
}

i64 MemBuff::read(i64 offset, void *to, i64 len) const {
    if (size_ <= offset) {
        return 0;
    }

    const i64 readable = std::min(size_ - offset, len);
    std::memcpy(to, beg_ + offset, readable);
    return readable;
}

i64 MemBuff::remove(i64 offset, i64 len) {
    if (size_ <= offset) {
        return 0;
    }

    const i64 removable = std::min(size_ - offset, len);
    const i64 endOffset = offset + removable;
    if (endOffset < size_) {
        std::memmove(beg_ + offset, beg_ + endOffset, size_ - endOffset);
    }
    size_ -= removable;
    return removable;
}

i64 MemBuff::take(i64 offset, void *to, i64 len) {
    if (size_ <= offset) {
        return 0;
    }

    const i64 takable = std::min(size_ - offset, len);
    const i64 endOffset = offset + takable;
    std::memcpy(to, beg_ + offset, takable);
    if (endOffset < size_) {
        std::memmove(beg_ + offset, beg_ + endOffset, size_ - endOffset);
    }
    size_ -= takable;
    return takable;
}

void MemBuff::write(i64 offset, const void *data, i64 len) {
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

void MemBuff::append(const void *from, i64 len) {
    const i64 newbytecount = size_ + len;
    ensureCapacity(newbytecount);
    std::memcpy(beg_ + size_, from, len);
    size_ = newbytecount;
}

i64 MemBuff::calcRealNewCapacity(i64 curCapacity, i64 newCapacity) {
	constexpr i64 kInit = 256;
    i64 n = curCapacity == 0 ? kInit : curCapacity;
	while (n < newCapacity) {
		n *= 2;
	}
	return n;
}

void MemBuff::realloc(i64 newcapacity) {
    capacity_ = newcapacity;
    byte *ptr = new byte[capacity_];
    std::memcpy(ptr, beg_, size_);
    delete beg_;
    beg_ = ptr;
}
