#pragma once

#include "core/basetype.h"
#include "core/range.h"


typedef i64 PartId;
constexpr PartId InvalidPartId = -1;

typedef i64 RowN;
constexpr RowN InvalidRowN = -1;

typedef i64 LineN;
constexpr LineN InvalidLineN = -1;

typedef i64 CharN;
constexpr CharN InvalidCharN = -1;

using RowRange = Range<RowN>;

using ByteRange = Range<i64>;
