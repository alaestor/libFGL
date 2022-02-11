#pragma once
#ifndef FGL_TYPES_CTYPES_HPP_INCLUDED
#define FGL_TYPES_CTYPES_HPP_INCLUDED

namespace fgl {

using cstring = const char*;
using cbyte = unsigned char;

template <typename T, std::size_t T_length>
using carray = T[T_length];

} // namespace fgl

#endif // FGL_TYPES_CTYPES_HPP_INCLUDED
