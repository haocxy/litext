#pragma once

#include <cstdint>
#include <string>
#include <memory>
#include <optional>

#include "scoped_container.h"
#include "ustr.h"


using i8 = int8_t;
using u8 = uint8_t;

using i16 = int16_t;
using u16 = uint16_t;

using i32 = int32_t;
using u32 = uint32_t;
using f32 = float;
static_assert(sizeof(float) == 4);

using i64 = int64_t;
using u64 = uint64_t;
using f64 = double;
static_assert(sizeof(double) == 8);

using cstr = const char *;

using str = std::string;

template <typename T>
using uptr = std::unique_ptr<T>;

template <typename T>
using sptr = std::shared_ptr<T>;

template <typename T>
using opt = std::optional<T>;
