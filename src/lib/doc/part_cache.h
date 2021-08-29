#pragma once

#include <map>
#include <mutex>
#include <string>
#include <fstream>
#include <vector>

#include "core/fs.h"
#include "core/memory.h"
#include "text/utf16row.h"

#include "doc_define.h"
#include "line_manger.h"



namespace doc
{

// 文档的一个片段的段落数通常是比实际需要用于展示的段落数多很多的，
// 为了在IO和字符编码转换这两者逻辑间取得平衡，使用UTF-16本机字节序编码
//
// 问：
//  问什么不适用UTF-8编码？
// 答：
//  相比于UTF-16，UTF-8无法快速地转换为UTF-32，因为需要从前往后逐个字节处理
//  虽然UTF-16转UTF-32也需要考虑代理字符的问题，但从概率的角度来说，
//  在多数情况下UTF-16字符不是代理字符，基于这一前提就可以对一个UTF-16字符串只检查一次是否有代理字符，
//  这样就可以针对不存在代理字符的情况（即多数情况）单独写一个更快的逻辑，同时也能减少内存分配次数
//  而只有存在代理字符时才一个字符一个字符地处理。
//  综上，使用UTF-16编码可以在大多数时候有很好地性能，同时有不那么糟糕的空间占用情况。
class PartCache {
public:
    using Part = std::pmr::vector<text::UTF16Row>;

    PartCache(LineManager &lineManager, const fs::path &file);

    PartCache(const PartCache &) = delete;

    PartCache(PartCache &&) = delete;

    PartCache &operator=(const PartCache &) = delete;

    PartCache &operator=(PartCache &&) = delete;

    sptr<Part> partAt(PartId partId);

    Charset charset() const {
        Lock lock(mtx_);
        return charset_;
    }

    void updateCharset(Charset charset) {
        Lock lock(mtx_);
        if (charset_ == charset) {
            return;
        }

        charset_ = charset;
        clearAllCache();
    }

private:
    void clearAllCache() {
        parts_.clear();
    }

private:
    using Mtx = std::mutex;
    using Lock = std::unique_lock<Mtx>;

    LineManager &lineManager_;

    mutable Mtx mtx_;

    std::pmr::monotonic_buffer_resource monoMemBuffer_;

    std::pmr::synchronized_pool_resource mempool_;

    CountedMemResource memres_;

    std::ifstream ifs_;

    Charset charset_ = Charset::Unknown;

    // 为了避免外部逻辑使用时片段缓存被释放，使用共享指针指向字符串对象
    std::pmr::map<PartId, sptr<Part>> parts_;

    // 记录各个片段最后一次使用时间
    std::pmr::map<PartId, std::chrono::time_point<std::chrono::steady_clock>> partToLastUse_;
};

}
