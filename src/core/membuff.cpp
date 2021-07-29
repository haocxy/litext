#include "membuff.h"

#include <algorithm>


void MemBuff::resize(i32 len) {
    ensureCapacity(len);
    size_ = len;
}

i32 MemBuff::read(i32 offset, void *to, i32 len) const {
    if (size_ <= offset) {
        return 0;
    }

    const i32 readable = std::min(size_ - offset, len);
    std::memcpy(to, beg_ + offset, readable);
    return readable;
}

i32 MemBuff::remove(i32 offset, i32 len) {
    if (size_ <= offset) {
        return 0;
    }

    const i32 removable = std::min(size_ - offset, len);
    const i32 endOffset = offset + removable;
    if (endOffset < size_) {
        std::memmove(beg_ + offset, beg_ + endOffset, size_ - endOffset);
    }
    size_ -= removable;
    return removable;
}

i32 MemBuff::take(i32 offset, void *to, i32 len) {
    if (size_ <= offset) {
        return 0;
    }

    const i32 takable = std::min(size_ - offset, len);
    const i32 endOffset = offset + takable;
    std::memcpy(to, beg_ + offset, takable);
    if (endOffset < size_) {
        std::memmove(beg_ + offset, beg_ + endOffset, size_ - endOffset);
    }
    size_ -= takable;
    return takable;
}

void MemBuff::write(i32 offset, const void *data, i32 len) {
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

void MemBuff::append(const void *from, i32 len) {
    const i32 newbytecount = size_ + len;
    ensureCapacity(newbytecount);
    std::memcpy(beg_ + size_, from, len);
    size_ = newbytecount;
}

i32 MemBuff::calcRealNewCapacity(i32 curCapacity, i32 newCapacity) {
	constexpr i32 kInit = 256;
    i32 n = curCapacity == 0 ? kInit : curCapacity;
	while (n < newCapacity) {
		n *= 2;
	}
	return n;
}

void MemBuff::realloc(i32 newcapacity) {
    capacity_ = newcapacity;
    byte *ptr = new byte[capacity_];
    std::memcpy(ptr, beg_, size_);
    delete beg_;
    beg_ = ptr;
}
