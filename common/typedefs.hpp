#ifndef AYTO_TYPEDEFS_HPP
#define AYTO_TYPEDEFS_HPP

#include <cstdint>

#include "ayto_detail_define_macros.hpp"

#define NS_BEGIN namespace AYTO_ROOT_NAMESPACE { AYTO_INLINE_NAMESPACE AYTO_CURRENT_ABI { namespace typedefs {
#define NS_END } } }



NS_BEGIN

using byte  = std::uint8_t;
using sbyte = std::int8_t;

using word = std::uint16_t;
using dword = std::uint32_t;
using qword = std::uint64_t;

NS_END


#undef NS_BEGIN
#undef NS_END

#include "ayto_detail_undefine_macros.hpp"
#endif