// Copyright 2018 Adrien Guinet <adrien@guinet.me>
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
//     http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef PYDFFI_DISPATCHER_H
#define PYDFFI_DISPATCHER_H

#include <dffi/composite_type.h>
#include <dffi/types.h>
#include <dffi/casting.h>

template <class T>
struct TypeDispatcher
{
  template <class... Args>
  static auto switch_(dffi::Type const* Ty, Args&& ... args)
    -> decltype(T::template case_basic<int>((dffi::BasicType const*)Ty, std::forward<Args>(args)...))
  {
    assert(Ty != nullptr && "conversion of void type requested!");

    // TODO: convert this to a switch!
    if (auto* BTy = dffi::dyn_cast<dffi::BasicType>(Ty)) {
#define HANDLE_BASICTY(DTy, CTy)\
      case dffi::BasicType::DTy:\
        return T::template case_basic<CTy>(BTy, std::forward<Args>(args)...);

      switch (BTy->getBasicKind()) {
        HANDLE_BASICTY(Char, char);
        HANDLE_BASICTY(UInt8, uint8_t);
        HANDLE_BASICTY(UInt16, uint16_t);
        HANDLE_BASICTY(UInt32, uint32_t);
        HANDLE_BASICTY(UInt64, uint64_t);
        HANDLE_BASICTY(UInt128, __uint128_t);
        HANDLE_BASICTY(Int8, int8_t);
        HANDLE_BASICTY(Int16, int16_t);
        HANDLE_BASICTY(Int32, int32_t);
        HANDLE_BASICTY(Int64, int64_t);
        HANDLE_BASICTY(Int128, __int128_t);
        static_assert(sizeof(float) == 4, "float must be 4 bytes");
        static_assert(sizeof(double) == 8, "double must be 8 bytes");
        static_assert(sizeof(long double) == 16, "double must be 16 bytes");
        HANDLE_BASICTY(Float32, float);
        HANDLE_BASICTY(Float64, double);
        HANDLE_BASICTY(Float128, long double);
        HANDLE_BASICTY(ComplexFloat32, _Complex float);
        HANDLE_BASICTY(ComplexFloat64, _Complex double);
        HANDLE_BASICTY(ComplexFloat128, _Complex long double);
        default:
          break;
#undef HANDLE_BASICTY
      };
    }
    else
    if (auto* STy = dffi::dyn_cast<dffi::StructType>(Ty)) {
      return T::case_composite(STy, std::forward<Args>(args)...);
    }
    else
    if (auto* UTy = dffi::dyn_cast<dffi::UnionType>(Ty)) {
      return T::case_composite(UTy, std::forward<Args>(args)...);
    }
    else
    if (auto* UTy = dffi::dyn_cast<dffi::EnumType>(Ty)) {
      return T::case_enum(UTy, std::forward<Args>(args)...);
    }
    else
    if (auto* PTy = dffi::dyn_cast<dffi::PointerType>(Ty)) {
      return T::case_pointer(PTy, std::forward<Args>(args)...);
    }
    else
    if (auto* ATy = dffi::dyn_cast<dffi::ArrayType>(Ty)) {
      return T::case_array(ATy, std::forward<Args>(args)...);
    }
    else
    if (auto* FTy = dffi::dyn_cast<dffi::FunctionType>(Ty)) {
      return T::case_func(FTy, std::forward<Args>(args)...);
    }
    assert(false && "unknown type!");
  }
};

#endif
