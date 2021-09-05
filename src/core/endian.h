#pragma once

#include <boost/endian/conversion.hpp>


namespace endian
{

template <typename T>
inline T netToNative(T value) {
    return boost::endian::big_to_native(value);
}

template <typename T>
inline T nativeToNet(T value) {
    return boost::endian::native_to_big(value);
}

}
