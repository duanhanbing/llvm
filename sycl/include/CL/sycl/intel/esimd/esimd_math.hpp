﻿//==-------------- esimd_math.hpp - DPC++ Explicit SIMD API   ------------==//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
// Implement Explicit SIMD math APIs.
//===----------------------------------------------------------------------===//

#pragma once

#include <CL/sycl/intel/esimd/detail/esimd_math_intrin.hpp>
#include <CL/sycl/intel/esimd/detail/esimd_types.hpp>
#include <CL/sycl/intel/esimd/detail/esimd_util.hpp>
#include <CL/sycl/intel/esimd/esimd.hpp>
#include <CL/sycl/intel/esimd/esimd_enum.hpp>
#include <cstdint>

__SYCL_INLINE_NAMESPACE(cl) {
namespace sycl {
namespace intel {
namespace gpu {

template <typename T0, typename T1, int SZ>
ESIMD_NODEBUG ESIMD_INLINE simd<T0, SZ> esimd_sat(simd<T1, SZ> src) {
  if constexpr (std::is_floating_point<T0>::value)
    return __esimd_satf<T0, T1, SZ>(src.data());
  else if constexpr (std::is_floating_point<T1>::value) {
    if constexpr (std::is_unsigned<T0>::value)
      return __esimd_fptoui_sat<T0, T1, SZ>(src.data());
    else
      return __esimd_fptosi_sat<T0, T1, SZ>(src.data());
  } else if constexpr (std::is_unsigned<T0>::value) {
    if constexpr (std::is_unsigned<T1>::value)
      return __esimd_uutrunc_sat<T0, T1, SZ>(src.data());
    else
      return __esimd_ustrunc_sat<T0, T1, SZ>(src.data());
  } else {
    if constexpr (std::is_signed<T1>::value)
      return __esimd_sutrunc_sat<T0, T1, SZ>(src.data());
    else
      return __esimd_sstrunc_sat<T0, T1, SZ>(src.data());
  }
}

// esimd_abs
namespace details {

template <typename T0, typename T1, int SZ>
ESIMD_NODEBUG ESIMD_INLINE simd<T0, SZ>
__esimd_abs_common_internal(simd<T1, SZ> src0, int flag = GENX_NOSAT) {
  simd<T1, SZ> Result = __esimd_abs<T1, SZ>(src0.data());
  if (flag != GENX_SAT)
    return Result;

  return esimd_sat<T0>(Result);
}

template <typename T0, typename T1>
ESIMD_NODEBUG ESIMD_INLINE
    typename std::enable_if<details::is_esimd_scalar<T0>::value &&
                                details::is_esimd_scalar<T1>::value,
                            typename std::remove_const<T0>::type>::type
    __esimd_abs_common_internal(T1 src0, int flag = GENX_NOSAT) {
  typedef typename std::remove_const<T0>::type TT0;
  typedef typename std::remove_const<T1>::type TT1;

  simd<TT1, 1> Src0 = src0;
  simd<TT0, 1> Result = __esimd_abs_common_internal<TT0>(Src0, flag);
  return Result[0];
}
} // namespace details

template <typename T0, typename T1, int SZ>
ESIMD_NODEBUG ESIMD_INLINE typename std::enable_if<
    !std::is_same<typename std::remove_const<T0>::type,
                  typename std::remove_const<T1>::type>::value,
    simd<T0, SZ>>::type
esimd_abs(simd<T1, SZ> src0, int flag = GENX_NOSAT) {
  return details::__esimd_abs_common_internal<T0, T1, SZ>(src0, flag);
}

template <typename T0, typename T1>
ESIMD_NODEBUG ESIMD_INLINE typename std::enable_if<
    !std::is_same<typename std::remove_const<T0>::type,
                  typename std::remove_const<T1>::type>::value &&
        details::is_esimd_scalar<T0>::value &&
        details::is_esimd_scalar<T1>::value,
    typename std::remove_const<T0>::type>::type
esimd_abs(T1 src0, int flag = GENX_NOSAT) {
  return details::__esimd_abs_common_internal<T0, T1>(src0, flag);
}

template <typename T1, int SZ>
ESIMD_NODEBUG ESIMD_INLINE simd<T1, SZ> esimd_abs(simd<T1, SZ> src0,
                                                  int flag = GENX_NOSAT) {
  return details::__esimd_abs_common_internal<T1, T1, SZ>(src0, flag);
}

template <typename T1>
ESIMD_NODEBUG ESIMD_INLINE
    typename std::enable_if<details::is_esimd_scalar<T1>::value,
                            typename std::remove_const<T1>::type>::type
    esimd_abs(T1 src0, int flag = GENX_NOSAT) {
  return details::__esimd_abs_common_internal<T1, T1>(src0, flag);
}

// esimd_shl
template <typename T0, typename T1, int SZ, typename U>
ESIMD_NODEBUG ESIMD_INLINE
    typename std::enable_if<std::is_integral<T0>::value &&
                                std::is_integral<T1>::value &&
                                std::is_integral<U>::value,
                            simd<T0, SZ>>::type
    esimd_shl(simd<T1, SZ> src0, U src1, int flag = GENX_NOSAT) {
  typedef typename computation_type<decltype(src0), U>::type ComputationTy;
  typename details::simd_type<ComputationTy>::type Src0 = src0;
  typename details::simd_type<ComputationTy>::type Src1 = src1;

  if (flag != GENX_SAT) {
    if constexpr (std::is_unsigned<T0>::value) {
      if constexpr (std::is_unsigned<T1>::value)
        return __esimd_uushl_sat<T0, T1, SZ>(Src0.data(), Src1.data());
      else
        return __esimd_usshl_sat<T0, T1, SZ>(Src0.data(), Src1.data());
    } else {
      if constexpr (std::is_signed<T1>::value)
        return __esimd_sushl_sat<T0, T1, SZ>(Src0.data(), Src1.data());
      else
        return __esimd_ssshl_sat<T0, T1, SZ>(Src0.data(), Src1.data());
    }
  } else {
    if constexpr (std::is_unsigned<T0>::value) {
      if constexpr (std::is_unsigned<T1>::value)
        return __esimd_uushl<T0, T1, SZ>(Src0.data(), Src1.data());
      else
        return __esimd_usshl<T0, T1, SZ>(Src0.data(), Src1.data());
    } else {
      if constexpr (std::is_signed<T1>::value)
        return __esimd_sushl<T0, T1, SZ>(Src0.data(), Src1.data());
      else
        return __esimd_ssshl<T0, T1, SZ>(Src0.data(), Src1.data());
    }
  }
}

template <typename T0, typename T1, typename T2>
ESIMD_NODEBUG ESIMD_INLINE typename std::enable_if<
    details::is_esimd_scalar<T0>::value &&
        details::is_esimd_scalar<T1>::value &&
        details::is_esimd_scalar<T2>::value && std::is_integral<T0>::value &&
        std::is_integral<T1>::value && std::is_integral<T2>::value,
    typename std::remove_const<T0>::type>::type
esimd_shl(T1 src0, T2 src1, int flag = GENX_NOSAT) {
  typedef typename computation_type<T1, T2>::type ComputationTy;
  typename details::simd_type<ComputationTy>::type Src0 = src0;
  typename details::simd_type<ComputationTy>::type Src1 = src1;
  simd<T0, 1> Result = esimd_shl<T0>(Src0, Src1, flag);
  return Result[0];
}

// esimd_shr
template <typename T0, typename T1, int SZ, typename U>
ESIMD_NODEBUG ESIMD_INLINE
    typename std::enable_if<std::is_integral<T0>::value &&
                                std::is_integral<T1>::value &&
                                std::is_integral<U>::value,
                            simd<T0, SZ>>::type
    esimd_shr(simd<T1, SZ> src0, U src1, int flag = GENX_NOSAT) {
  typedef typename computation_type<decltype(src0), U>::type ComputationTy;
  typename details::simd_type<ComputationTy>::type Src0 = src0;
  typename details::simd_type<ComputationTy>::type Src1 = src1;
  typename details::simd_type<ComputationTy>::type Result =
      Src0.data() >> Src1.data();

  if (flag != GENX_SAT)
    return Result;

  return esimd_sat<T0>(Result);
}

template <typename T0, typename T1, typename T2>
ESIMD_NODEBUG ESIMD_INLINE typename std::enable_if<
    details::is_esimd_scalar<T0>::value &&
        details::is_esimd_scalar<T1>::value &&
        details::is_esimd_scalar<T2>::value && std::is_integral<T0>::value &&
        std::is_integral<T1>::value && std::is_integral<T2>::value,
    typename std::remove_const<T0>::type>::type
esimd_shr(T1 src0, T2 src1, int flag = GENX_NOSAT) {
  typedef typename computation_type<T1, T2>::type ComputationTy;
  typename details::simd_type<ComputationTy>::type Src0 = src0;
  typename details::simd_type<ComputationTy>::type Src1 = src1;
  simd<T0, 1> Result = esimd_shr<T0>(Src0, Src1, flag);
  return Result[0];
}

// esimd_rol
template <typename T0, typename T1, int SZ>
ESIMD_NODEBUG ESIMD_INLINE
    typename std::enable_if<std::is_integral<T0>::value &&
                                std::is_integral<T1>::value,
                            simd<T0, SZ>>::type
    esimd_rol(simd<T1, SZ> src0, simd<T1, SZ> src1) {
  return __esimd_rol<T0, T1, SZ>(src0, src1);
}

template <typename T0, typename T1, int SZ, typename U>
ESIMD_NODEBUG ESIMD_INLINE
    typename std::enable_if<std::is_integral<T0>::value &&
                                std::is_integral<T1>::value &&
                                std::is_integral<U>::value,
                            simd<T0, SZ>>::type
    esimd_rol(simd<T1, SZ> src0, U src1) {
  typedef typename computation_type<decltype(src0), U>::type ComputationTy;
  typename details::simd_type<ComputationTy>::type Src0 = src0;
  typename details::simd_type<ComputationTy>::type Src1 = src1;
  return __esimd_rol<T0>(Src0.data(), Src1.data());
}

template <typename T0, typename T1, typename T2>
ESIMD_NODEBUG ESIMD_INLINE typename std::enable_if<
    details::is_esimd_scalar<T0>::value &&
        details::is_esimd_scalar<T1>::value &&
        details::is_esimd_scalar<T2>::value && std::is_integral<T0>::value &&
        std::is_integral<T1>::value && std::is_integral<T2>::value,
    typename std::remove_const<T0>::type>::type
esimd_rol(T1 src0, T2 src1) {
  typedef typename computation_type<T1, T2>::type ComputationTy;
  typename details::simd_type<ComputationTy>::type Src0 = src0;
  typename details::simd_type<ComputationTy>::type Src1 = src1;
  simd<T0, 1> Result = esimd_rol<T0>(Src0, Src1);
  return Result[0];
}

// esimd_ror
template <typename T0, typename T1, int SZ>
ESIMD_NODEBUG ESIMD_INLINE
    typename std::enable_if<std::is_integral<T0>::value &&
                                std::is_integral<T1>::value,
                            simd<T0, SZ>>::type
    esimd_ror(simd<T1, SZ> src0, simd<T1, SZ> src1) {
  return __esimd_ror<T0, T1, SZ>(src0, src1);
}

template <typename T0, typename T1, int SZ, typename U>
ESIMD_NODEBUG ESIMD_INLINE
    typename std::enable_if<std::is_integral<T0>::value &&
                                std::is_integral<T1>::value &&
                                std::is_integral<U>::value,
                            simd<T0, SZ>>::type
    esimd_ror(simd<T1, SZ> src0, U src1) {
  typedef typename computation_type<decltype(src0), U>::type ComputationTy;
  typename details::simd_type<ComputationTy>::type Src0 = src0;
  typename details::simd_type<ComputationTy>::type Src1 = src1;
  return __esimd_ror<T0>(Src0.data(), Src1.data());
}

template <typename T0, typename T1, typename T2>
ESIMD_NODEBUG ESIMD_INLINE typename std::enable_if<
    details::is_esimd_scalar<T0>::value &&
        details::is_esimd_scalar<T1>::value &&
        details::is_esimd_scalar<T2>::value && std::is_integral<T0>::value &&
        std::is_integral<T1>::value && std::is_integral<T2>::value,
    typename std::remove_const<T0>::type>::type
esimd_ror(T1 src0, T2 src1) {
  typedef typename computation_type<T1, T2>::type ComputationTy;
  typename details::simd_type<ComputationTy>::type Src0 = src0;
  typename details::simd_type<ComputationTy>::type Src1 = src1;
  simd<T0, 1> Result = esimd_ror<T0>(Src0, Src1);
  return Result[0];
}

// esimd_lsr
template <typename T0, typename T1, int SZ, typename U>
ESIMD_NODEBUG ESIMD_INLINE
    typename std::enable_if<std::is_integral<T0>::value &&
                                std::is_integral<T1>::value &&
                                std::is_integral<U>::value,
                            simd<T0, SZ>>::type
    esimd_lsr(simd<T1, SZ> src0, U src1, int flag = GENX_NOSAT) {
  typedef typename computation_type<T1, T1>::type IntermedTy;
  typedef typename std::make_unsigned<IntermedTy>::type ComputationTy;
  simd<ComputationTy, SZ> Src0 = src0;
  simd<ComputationTy, SZ> Result = Src0.data() >> src1.data();

  if (flag != GENX_SAT)
    return Result;

  return esimd_sat<T0>(Result);
}

template <typename T0, typename T1, typename T2>
ESIMD_NODEBUG ESIMD_INLINE typename std::enable_if<
    details::is_esimd_scalar<T0>::value &&
        details::is_esimd_scalar<T1>::value &&
        details::is_esimd_scalar<T2>::value && std::is_integral<T0>::value &&
        std::is_integral<T1>::value && std::is_integral<T2>::value,
    typename std::remove_const<T0>::type>::type
esimd_lsr(T1 src0, T2 src1, int flag = GENX_NOSAT) {
  typedef typename computation_type<T1, T2>::type ComputationTy;
  typename details::simd_type<ComputationTy>::type Src0 = src0;
  typename details::simd_type<ComputationTy>::type Src1 = src1;
  simd<T0, 1> Result = esimd_lsr<T0>(Src0, Src1, flag);
  return Result[0];
}

template <typename T0, typename T1, typename T2>
ESIMD_NODEBUG ESIMD_INLINE typename std::enable_if<
    details::is_esimd_scalar<T1>::value &&
        details::is_esimd_vector<T2>::value && std::is_integral<T0>::value &&
        std::is_integral<T1>::value && std::is_integral<T2>::value,
    decltype(esimd_lsr<T0>(T2(), T1()))>::type
esimd_lsr(T1 src0, T2 src1, int flag = GENX_NOSAT) {
  return esimd_lsr<T0>(src1, src0, flag);
}

// esimd_asr
template <typename T0, typename T1, int SZ, typename U>
ESIMD_NODEBUG ESIMD_INLINE
    typename std::enable_if<std::is_integral<T0>::value &&
                                std::is_integral<T1>::value &&
                                std::is_integral<U>::value,
                            simd<T0, SZ>>::type
    esimd_asr(simd<T1, SZ> src0, U src1, int flag = GENX_NOSAT) {
  typedef typename computation_type<T1, T1>::type IntermedTy;
  typedef typename std::make_signed<IntermedTy>::type ComputationTy;
  simd<ComputationTy, SZ> Src0 = src0;
  simd<ComputationTy, SZ> Result = Src0 >> src1;

  if (flag != GENX_SAT)
    return Result;

  return esimd_sat<T0>(Result);
}

template <typename T0, typename T1, typename T2>
ESIMD_NODEBUG ESIMD_INLINE typename std::enable_if<
    details::is_esimd_scalar<T0>::value &&
        details::is_esimd_scalar<T1>::value &&
        details::is_esimd_scalar<T2>::value && std::is_integral<T0>::value &&
        std::is_integral<T1>::value && std::is_integral<T2>::value,
    typename std::remove_const<T0>::type>::type
esimd_asr(T1 src0, T2 src1, int flag = GENX_NOSAT) {
  typedef typename computation_type<T1, T2>::type ComputationTy;
  typename details::simd_type<ComputationTy>::type Src0 = src0;
  typename details::simd_type<ComputationTy>::type Src1 = src1;
  simd<T0, 1> Result = esimd_asr<T0>(Src0, Src1, flag);
  return Result[0];
}

template <typename T0, typename T1, typename T2>
ESIMD_NODEBUG ESIMD_INLINE typename std::enable_if<
    details::is_esimd_scalar<T1>::value &&
        details::is_esimd_vector<T2>::value && std::is_integral<T0>::value &&
        std::is_integral<T1>::value && std::is_integral<T2>::value,
    decltype(esimd_asr<T0>(T2(), T1()))>::type
esimd_asr(T1 src0, T2 src1, int flag = GENX_NOSAT) {
  return esimd_asr<T0>(src1, src0, flag);
}

// esimd_imul
#ifndef ESIMD_HAS_LONG_LONG
// use mulh instruction for high half
template <typename T0, typename T1, typename U, int SZ>
ESIMD_NODEBUG ESIMD_INLINE
    typename std::enable_if<details::is_dword_type<T0>::value &&
                                details::is_dword_type<T1>::value &&
                                details::is_dword_type<U>::value,
                            simd<T0, SZ>>::type
    esimd_imul(simd<T0, SZ> &rmd, simd<T1, SZ> src0, U src1) {
  typedef typename computation_type<decltype(src0), U>::type ComputationTy;
  typename details::simd_type<ComputationTy>::type Src0 = src0;
  typename details::simd_type<ComputationTy>::type Src1 = src1;
  rmd = Src0 * Src1;
  if constexpr (std::is_unsigned<T0>::value)
    return __esimd_umulh(Src0.data(), Src1.data());
  else
    return __esimd_smulh(Src0.data(), Src1.data());
}

#else
// esimd_imul bdw+ version: use qw=dw*dw multiply.
// We need to special case SZ==1 to avoid "error: when select size is 1, the
// stride must also be 1" on the selects.
template <typename T0, typename T1, typename U, int SZ>
ESIMD_NODEBUG ESIMD_INLINE
    typename std::enable_if<details::is_dword_type<T0>::value &&
                                details::is_dword_type<T1>::value &&
                                details::is_dword_type<U>::value && SZ == 1,
                            simd<T0, SZ>>::type
    esimd_imul(simd<T0, SZ> &rmd, simd<T1, SZ> src0, U src1) {
  typedef
      typename computation_type<decltype(rmd), long long>::type ComputationTy;
  ComputationTy Product = convert<long long>(src0);
  Product *= src1;
  rmd = Product.format<T0>().select<1, 1>[0];
  return Product.format<T0>().select<1, 1>[1];
}

template <typename T0, typename T1, typename U, int SZ>
ESIMD_NODEBUG ESIMD_INLINE
    typename std::enable_if<details::is_dword_type<T0>::value &&
                                details::is_dword_type<T1>::value &&
                                details::is_dword_type<U>::value && SZ != 1,
                            simd<T0, SZ>>::type
    esimd_imul(simd<T0, SZ> &rmd, simd<T1, SZ> src0, U src1) {
  typedef
      typename computation_type<decltype(rmd), long long>::type ComputationTy;
  ComputationTy Product = convert<long long>(src0);
  Product *= src1;
  rmd = Product.format<T0>().select<SZ, 2>(0);
  return Product.format<T0>().select<SZ, 2>(1);
}
#endif

// esimd_imul wrappers

template <typename T0, typename T1, typename U, int SZ>
ESIMD_NODEBUG ESIMD_INLINE
    typename std::enable_if<details::is_esimd_scalar<U>::value,
                            simd<T0, SZ>>::type
    esimd_imul(simd<T0, SZ> &rmd, U src0, simd<T1, SZ> src1) {
  return esimd_imul(rmd, src1, src0);
}

template <typename T0, typename T, typename U>
ESIMD_NODEBUG ESIMD_INLINE
    typename std::enable_if<details::is_esimd_scalar<T>::value &&
                                details::is_esimd_scalar<U>::value &&
                                details::is_esimd_scalar<T0>::value,
                            T0>::type
    esimd_imul(simd<T0, 1> &rmd, T src0, U src1) {
  simd<T, 1> src_0 = src0;
  simd<U, 1> src_1 = src1;
  simd<T0, 1> res = esimd_imul(rmd, src_0.select_all(), src_1.select_all());
  return res[0];
}

// esimd_quot
template <typename T, int SZ, typename U>
ESIMD_NODEBUG ESIMD_INLINE typename std::enable_if<
    std::is_integral<T>::value && std::is_integral<U>::value, simd<T, SZ>>::type
esimd_quot(simd<T, SZ> src0, U src1) {
  return src0 / src1;
}

template <typename T0, typename T1>
ESIMD_NODEBUG ESIMD_INLINE
    typename std::enable_if<details::is_esimd_scalar<T0>::value &&
                                details::is_esimd_scalar<T1>::value &&
                                std::is_integral<T0>::value &&
                                std::is_integral<T1>::value,
                            typename std::remove_const<T0>::type>::type
    esimd_quot(T0 src0, T1 src1) {
  return src0 / src1;
}

// esimd_mod
template <typename T, int SZ, typename U>
ESIMD_NODEBUG ESIMD_INLINE typename std::enable_if<
    std::is_integral<T>::value && std::is_integral<U>::value, simd<T, SZ>>::type
esimd_mod(simd<T, SZ> src0, U src1) {
  return src0 % src1;
}

template <typename T0, typename T1>
ESIMD_NODEBUG ESIMD_INLINE
    typename std::enable_if<details::is_esimd_scalar<T0>::value &&
                                details::is_esimd_scalar<T1>::value &&
                                std::is_integral<T0>::value &&
                                std::is_integral<T1>::value,
                            typename std::remove_const<T0>::type>::type
    esimd_mod(T0 src0, T1 src1) {
  return src0 % src1;
}

// esimd_div, compute quotient and remainder of division.
template <typename T, int SZ, typename U>
ESIMD_NODEBUG ESIMD_INLINE typename std::enable_if<
    std::is_integral<T>::value && std::is_integral<U>::value, simd<T, SZ>>::type
esimd_div(simd<T, SZ> &remainder, simd<T, SZ> src0, U src1) {
  remainder = src0 % src1;
  return src0 / src1;
}

template <typename T, int SZ, typename U>
ESIMD_NODEBUG ESIMD_INLINE
    typename std::enable_if<std::is_integral<T>::value &&
                                std::is_integral<U>::value &&
                                details::is_esimd_scalar<U>::value,
                            simd<T, SZ>>::type
    esimd_div(simd<T, SZ> &remainder, U src0, simd<T, SZ> src1) {
  remainder = src0 % src1;
  return src0 / src1;
}

template <typename RT, typename T0, typename T1>
ESIMD_NODEBUG ESIMD_INLINE
    typename std::enable_if<details::is_esimd_scalar<RT>::value &&
                                details::is_esimd_scalar<T0>::value &&
                                details::is_esimd_scalar<T1>::value,
                            typename std::remove_const<RT>::type>::type
    esimd_div(simd<typename std::remove_const<RT>::type, 1> &remainder, T0 src0,
              T1 src1) {
  remainder[0] = src0 % src1;
  return src0 / src1;
}

// esimd_min and esimd_max
//
// Restriction:
//
// The source operands must be both of integer or both of floating-point type.
//
template <typename T, int SZ>
ESIMD_NODEBUG ESIMD_INLINE simd<T, SZ>
esimd_max(simd<T, SZ> src0, simd<T, SZ> src1, int flag = GENX_NOSAT) {
  if constexpr (std::is_floating_point<T>::value) {
    auto Result = __esimd_fmax<T, SZ>(src0.data(), src1.data());
    return (flag == GENX_NOSAT) ? Result : __esimd_satf<T, T, SZ>(Result);
  } else if constexpr (std::is_unsigned<T>::value) {
    auto Result = __esimd_umax<T, SZ>(src0.data(), src1.data());
    return (flag == GENX_NOSAT) ? Result
                                : __esimd_uutrunc_sat<T, T, SZ>(Result);
  } else {
    auto Result = __esimd_smax<T, SZ>(src0.data(), src1.data());
    return (flag == GENX_NOSAT) ? Result
                                : __esimd_sstrunc_sat<T, T, SZ>(Result);
  }
}

template <typename T, int SZ>
ESIMD_NODEBUG ESIMD_INLINE
    typename std::enable_if<details::is_esimd_scalar<T>::value,
                            simd<T, SZ>>::type
    esimd_max(simd<T, SZ> src0, T src1, int flag = GENX_NOSAT) {
  simd<T, SZ> Src1 = src1;
  simd<T, SZ> Result = esimd_max<T>(src0, Src1, flag);
  return Result;
}

template <typename T, int SZ>
ESIMD_NODEBUG ESIMD_INLINE
    typename std::enable_if<details::is_esimd_scalar<T>::value,
                            simd<T, SZ>>::type
    esimd_max(T src0, simd<T, SZ> src1, int flag = GENX_NOSAT) {
  simd<T, SZ> Src0 = src0;
  simd<T, SZ> Result = esimd_max<T>(Src0, src1, flag);
  return Result;
}

template <typename T>
ESIMD_NODEBUG ESIMD_INLINE
    typename std::enable_if<details::is_esimd_scalar<T>::value, T>::type
    esimd_max(T src0, T src1, int flag = GENX_NOSAT) {
  simd<T, 1> Src0 = src0;
  simd<T, 1> Src1 = src1;
  simd<T, 1> Result = esimd_max<T>(Src0, Src1, flag);
  return Result[0];
}

template <typename T, int SZ>
ESIMD_NODEBUG ESIMD_INLINE simd<T, SZ>
esimd_min(simd<T, SZ> src0, simd<T, SZ> src1, int flag = GENX_NOSAT) {
  if constexpr (std::is_floating_point<T>::value) {
    auto Result = __esimd_fmin<T, SZ>(src0.data(), src1.data());
    return (flag == GENX_NOSAT) ? Result : __esimd_satf<T, T, SZ>(Result);
  } else if constexpr (std::is_unsigned<T>::value) {
    auto Result = __esimd_umin<T, SZ>(src0.data(), src1.data());
    return (flag == GENX_NOSAT) ? Result
                                : __esimd_uutrunc_sat<T, T, SZ>(Result);
  } else {
    auto Result = __esimd_smin<T, SZ>(src0.data(), src1.data());
    return (flag == GENX_NOSAT) ? Result
                                : __esimd_sstrunc_sat<T, T, SZ>(Result);
  }
}

template <typename T, int SZ>
ESIMD_NODEBUG ESIMD_INLINE
    typename std::enable_if<details::is_esimd_scalar<T>::value,
                            simd<T, SZ>>::type
    esimd_min(simd<T, SZ> src0, T src1, int flag = GENX_NOSAT) {
  simd<T, SZ> Src1 = src1;
  simd<T, SZ> Result = esimd_min<T>(src0, Src1, flag);
  return Result;
}

template <typename T, int SZ>
ESIMD_NODEBUG ESIMD_INLINE
    typename std::enable_if<details::is_esimd_scalar<T>::value,
                            simd<T, SZ>>::type
    esimd_min(T src0, simd<T, SZ> src1, int flag = GENX_NOSAT) {
  simd<T, SZ> Src0 = src0;
  simd<T, SZ> Result = esimd_min<T>(Src0, src1, flag);
  return Result;
}
template <typename T>
ESIMD_NODEBUG ESIMD_INLINE
    typename std::enable_if<details::is_esimd_scalar<T>::value, T>::type
    esimd_min(T src0, T src1, int flag = GENX_NOSAT) {
  simd<T, 1> Src0 = src0;
  simd<T, 1> Src1 = src1;
  simd<T, 1> Result = esimd_min<T>(Src0, Src1, flag);
  return Result[0];
}

// Dot product builtins
#if defined(ESIMD_GEN7_5) || defined(ESIMD_GEN8) || defined(ESIMD_GEN8_5) ||   \
    defined(ESIMD_GEN9) || defined(ESIMD_GEN9_5)
template <typename T0, typename T1, int SZ, typename U>
ESIMD_NODEBUG ESIMD_INLINE simd<T0, SZ> esimd_dp2(simd<T1, SZ> src0, U src1,
                                                  int flag = GENX_NOSAT) {
  static_assert(SZ % 4 == 0, "result size is not a multiple of 4");
  simd<float, SZ> Src0 = src0;
  simd<float, SZ> Src1 = src1;
  simd<float, SZ> Result = __esimd_dp2(Src0.data(), Src1.data());
  if (flag != GENX_SAT)
    return Result;

  return esimd_sat<T0>(Result);
}

template <typename T0, typename T1, int SZ, typename U>
ESIMD_NODEBUG ESIMD_INLINE simd<T0, SZ> esimd_dp3(simd<T1, SZ> src0, U src1,
                                                  int flag = GENX_NOSAT) {
  static_assert(SZ % 4 == 0, "result size is not a multiple of 4");
  simd<float, SZ> Src0 = src0;
  simd<float, SZ> Src1 = src1;
  simd<float, SZ> Result = __esimd_dp3(Src0.data(), Src1.data());
  if (flag != GENX_SAT)
    return Result;

  return esimd_sat<T0>(Result);
}

template <typename T0, typename T1, int SZ, typename U>
ESIMD_NODEBUG ESIMD_INLINE simd<T0, SZ> esimd_dp4(simd<T1, SZ> src0, U src1,
                                                  int flag = GENX_NOSAT) {
  static_assert(SZ % 4 == 0, "result size is not a multiple of 4");
  simd<float, SZ> Src0 = src0;
  simd<float, SZ> Src1 = src1;
  simd<float, SZ> Result = __esimd_dp4(Src0.data(), Src1.data());
  if (flag != GENX_SAT)
    return Result;

  return esimd_sat<T0>(Result);
}

template <typename T0, typename T1, typename U, int SZ>
ESIMD_NODEBUG ESIMD_INLINE simd<T0, SZ> esimd_dph(simd<T1, SZ> src0, U src1,
                                                  int flag = GENX_NOSAT) {
  static_assert(SZ % 4 == 0, "result size is not a multiple of 4");
  simd<float, SZ> Src0 = src0;
  simd<float, SZ> Src1 = src1;
  simd<float, SZ> Result = __esimd_dph(Src0.data(), Src1.data());
  if (flag != GENX_SAT)
    return Result;

  return esimd_sat<T0>(Result);
}

template <typename RT, typename T1, typename T2, int SZ>
ESIMD_NODEBUG ESIMD_INLINE simd<RT, SZ>
esimd_line(simd<T1, 4> src0, simd<T2, SZ> src1, int flag = GENX_NOSAT) {
  static_assert(SZ % 4 == 0, "result size is not a multiple of 4");

  simd<float, 4> Src0 = src0;
  simd<float, SZ> Src1 = src1;
  simd<float, SZ> Result = __esimd_line(Src0.data(), Src1.data());

  simd<RT, SZ> Result;
  if (flag == GENX_SAT)
    Result = esimd_sat<RT>(Result);
  else
    Result = Result;

  return Result;
}

template <typename RT, typename T, int SZ>
ESIMD_NODEBUG ESIMD_INLINE simd<RT, SZ>
esimd_line(float P, float Q, simd<T, SZ> src1, int flag = GENX_NOSAT) {
  simd<float, 4> Src0 = P;
  Src0(3) = Q;
  return esimd_line<RT>(Src0, src1, flag);
}

#else
// The old implementation is to generate vISA IRs for dp2/dp3/dp4/dph/line.
// Now We change to use direct mul/add, and hope to generate mad instructions
// at the end, to still get the performance as good as HW solution.
// We rely on "pragma unroll" to get better code.
// The only input and return types for these APIs are floats.
// In order to be able to use the old emu code, we keep the template argument
// for the type, although the type "T" can only be float.
// We use enable_if to force the float type only.
// If the gen is not specified we warn the programmer that they are potentially
// using a less efficient implementation if not on GEN10 or above.
template <typename T0, typename T1, int SZ, typename U>
ESIMD_NODEBUG ESIMD_INLINE
    typename std::enable_if<details::is_fp_or_dword_type<T1>::value &&
                                std::is_floating_point<T1>::value &&
                                details::is_fp_or_dword_type<U>::value &&
                                std::is_floating_point<U>::value,
                            simd<T0, SZ>>::type
    esimd_dp2(simd<T1, SZ> src0, U src1, int flag = GENX_NOSAT) {
  static_assert(SZ % 4 == 0, "result size is not a multiple of 4");

  simd<float, SZ> Src1 = src1;
  simd<float, SZ> Result;
#pragma unroll
  for (int i = 0; i < SZ; i += 4) {
    Result.select<4, 1>(i) = src0[i] * Src1[i] + src0[i + 1] * Src1[i + 1];
  }
  if (flag != GENX_SAT)
    return Result;

  return esimd_sat<T1>(Result);
}

template <typename T0, typename T1, int SZ, typename U>
ESIMD_NODEBUG ESIMD_INLINE
    typename std::enable_if<details::is_fp_or_dword_type<T1>::value &&
                                std::is_floating_point<T1>::value &&
                                details::is_fp_or_dword_type<U>::value &&
                                std::is_floating_point<U>::value,
                            simd<T0, SZ>>::type
    esimd_dp3(simd<T1, SZ> src0, U src1, int flag = GENX_NOSAT) {
  static_assert(SZ % 4 == 0, "result size is not a multiple of 4");

  simd<float, SZ> Src1 = src1;
  simd<float, SZ> Result;
#pragma unroll
  for (int i = 0; i < SZ; i += 4) {
    Result.select<4, 1>(i) = src0[i] * Src1[i] + src0[i + 1] * Src1[i + 1] +
                             src0[i + 2] * Src1[i + 2];
  }
  if (flag != GENX_SAT)
    return Result;

  return esimd_sat<T1>(Result);
}

template <typename T0, typename T1, int SZ, typename U>
ESIMD_NODEBUG ESIMD_INLINE
    typename std::enable_if<details::is_fp_or_dword_type<T1>::value &&
                                std::is_floating_point<T1>::value &&
                                details::is_fp_or_dword_type<U>::value &&
                                std::is_floating_point<U>::value,
                            simd<T0, SZ>>::type
    esimd_dp4(simd<T1, SZ> src0, U src1, int flag = GENX_NOSAT) {
  static_assert(SZ % 4 == 0, "result size is not a multiple of 4");

  simd<T1, SZ> Src1 = src1;
  simd<float, SZ> Result;
#pragma unroll
  for (int i = 0; i < SZ; i += 4) {
    Result.select<4, 1>(i) = src0[i] * Src1[i] + src0[i + 1] * Src1[i + 1] +
                             src0[i + 2] * Src1[i + 2] +
                             src0[i + 3] * Src1[i + 3];
  }
  if (flag != GENX_SAT)
    return Result;

  return esimd_sat<T1>(Result);
}

template <typename T, typename U, int SZ>
ESIMD_NODEBUG ESIMD_INLINE
    typename std::enable_if<details::is_fp_or_dword_type<T>::value &&
                                std::is_floating_point<T>::value &&
                                details::is_fp_or_dword_type<U>::value &&
                                std::is_floating_point<U>::value,
                            simd<T, SZ>>::type
    esimd_dph(simd<T, SZ> src0, U src1, int flag = GENX_NOSAT) {
  static_assert(SZ % 4 == 0, "result size is not a multiple of 4");

  simd<float, SZ> Src1 = src1;
  simd<float, SZ> Result;
#pragma unroll
  for (int i = 0; i < SZ; i += 4) {
    Result.select<4, 1>(i) = src0[i] * Src1[i] + src0[i + 1] * Src1[i + 1] +
                             src0[i + 2] * Src1[i + 2] + 1.0 * Src1[i + 3];
  }
  if (flag != GENX_SAT)
    return Result;

  return esimd_sat<T>(Result);
}

template <typename T, int SZ>
ESIMD_NODEBUG ESIMD_INLINE
    typename std::enable_if<details::is_fp_or_dword_type<T>::value &&
                                std::is_floating_point<T>::value,
                            simd<T, SZ>>::type
    esimd_line(simd<T, 4> src0, simd<T, SZ> src1, int flag = GENX_NOSAT) {
  static_assert(SZ % 4 == 0, "result size is not a multiple of 4");

  simd<T, SZ> Src1 = src1;
  simd<T, SZ> Result;
#pragma unroll
  for (int i = 0; i < SZ; i += 4) {
    Result.select<4, 1>(i) = src0[0] * src1[i] + src0[3];
  }

  if (flag == GENX_SAT)
    Result = esimd_sat<T>(Result);

  return Result;
}

template <typename T, int SZ>
ESIMD_NODEBUG ESIMD_INLINE
    typename std::enable_if<details::is_fp_or_dword_type<T>::value &&
                                std::is_floating_point<T>::value,
                            simd<T, SZ>>::type
    esimd_line(float P, float Q, simd<T, SZ> src1, int flag = GENX_NOSAT) {
  simd<T, 4> Src0 = P;
  Src0(3) = Q;
  return esimd_line<T>(Src0, src1, flag);
}

#endif

template <typename T, int SZ>
ESIMD_NODEBUG ESIMD_INLINE simd<T, SZ> esimd_frc(simd<T, SZ> src0) {
  simd<float, SZ> Src0 = src0;
  return __esimd_frc(Src0);
}

template <typename T> ESIMD_NODEBUG ESIMD_INLINE T esimd_frc(T src0) {
  simd<T, 1> Src0 = src0;
  simd<T, 1> Result = esimd_frc<T>(Src0);
  return Result[0];
}

// esimd_lzd
template <typename RT, typename T0, int SZ>
ESIMD_NODEBUG ESIMD_INLINE simd<RT, SZ> esimd_lzd(simd<T0, SZ> src0,
                                                  int flag = GENX_NOSAT) {
  // Saturation parameter ignored
  simd<uint, SZ> Src0 = src0;
  return __esimd_lzd<uint>(Src0);
}

template <typename RT, typename T0>
ESIMD_NODEBUG ESIMD_INLINE
    typename std::enable_if<details::is_esimd_scalar<RT>::value &&
                                details::is_esimd_scalar<T0>::value,
                            typename std::remove_const<RT>::type>::type
    esimd_lzd(T0 src0, int flag = GENX_NOSAT) {
  simd<T0, 1> Src0 = src0;
  simd<RT, 1> Result = esimd_lzd<RT>(Src0);
  return Result[0];
}

// esimd_lrp
#if defined(ESIMD_GEN7_5) || defined(ESIMD_GEN8) || defined(ESIMD_GEN8_5) ||   \
    defined(ESIMD_GEN9) || defined(ESIMD_GEN9_5)

template <int SZ, typename U, typename V>
ESIMD_NODEBUG ESIMD_INLINE simd<float, SZ>
esimd_lrp(simd<float, SZ> src0, U src1, V src2, int flag = GENX_NOSAT) {
  static_assert(SZ >= 4 && (SZ & 0x3) == 0,
                "vector size must be a multiple of 4");
  simd<float, SZ> Src1 = src1;
  simd<float, SZ> Src2 = src2;
  simd<float, SZ> Result = __esimd_lrp<SZ>(src0, Src1, Src2);

  if (flag != GENX_SAT)
    return Result;

  return esimd_sat<float>(Result);
}

#else

// The old implementation is to generate vISA IRs for lrp.
// Now We change to use direct mul/add, and hope to generate mad instructions
// at the end, to still get the performance as good as HW solution.
// The only input and return types for these APIs are floats.
// In order to be able to use the old emu code, we keep the template argument
// for the type, although the type "T" can only be float.
// We use enable_if to force the float type only.
// If the gen is not specified we warn the programmer that they are potentially
// using less efficient implementation.
template <typename T, int SZ, typename U, typename V>
ESIMD_NODEBUG ESIMD_INLINE
    typename std::enable_if<details::is_fp_or_dword_type<T>::value &&
                                std::is_floating_point<T>::value &&
                                details::is_fp_or_dword_type<U>::value &&
                                std::is_floating_point<U>::value,
                            simd<T, SZ>>::type
    esimd_lrp(simd<T, SZ> src0, U src1, V src2, int flag = GENX_NOSAT) {

  simd<float, SZ> Src1 = src1;
  simd<float, SZ> Src2 = src2;
  simd<float, SZ> Result;
  Result = Src1 * src0 + Src2 * (1.0f - src0);
  if (flag != GENX_SAT)
    return Result;
  return esimd_sat<T>(Result);
}
#endif

// esimd_pln
template <int SZ>
ESIMD_NODEBUG ESIMD_INLINE simd<float, SZ>
esimd_pln(simd<float, 4> src0, simd<float, SZ> src1, simd<float, SZ> src2,
          int flag = GENX_NOSAT) {
  static_assert(SZ >= 8 && (SZ & 0x7) == 0,
                "vector size must be a multiple of 8");

  // __esimd_intrinsic_impl_pln() requires src1 and src2 to be combined into
  // a single matrix, interleaving the values together in blocks of 8
  // items (ie, a block of 8 from src1, then a block of 8 from src2, then
  // the next block of 8 from src1, then the next block of 8 from src2,
  // and so-on.)
  simd<float, (SZ >> 3) * 16> Src12v;
  auto Src12 = Src12v.template format<float, (SZ >> 3), 16>();

  Src12.select<(SZ >> 3), 1, 8, 1>(0, 0) =
      src1.template format<float, (SZ >> 3), 8>();
  Src12.select<(SZ >> 3), 1, 8, 1>(0, 8) =
      src2.template format<float, (SZ >> 3), 8>();

  simd<float, SZ> Result = __esimd_pln<SZ>(src0, Src12.read());

  if (flag != GENX_SAT)
    return Result;

  return esimd_sat<float>(Result);
}

// esimd_bf_reverse
template <typename T0, typename T1, int SZ>
ESIMD_NODEBUG ESIMD_INLINE simd<T0, SZ> esimd_bf_reverse(simd<T1, SZ> src0) {
  simd<unsigned, SZ> Src0 = src0;
  return __esimd_bfrev<unsigned>(Src0);
}

template <typename T0, typename T1>
ESIMD_NODEBUG ESIMD_INLINE
    typename std::enable_if<details::is_esimd_scalar<T0>::value &&
                                details::is_esimd_scalar<T1>::value,
                            typename std::remove_const<T0>::type>::type
    esimd_bf_reverse(T1 src0) {
  simd<T1, 1> Src0 = src0;
  simd<T0, 1> Result = esimd_bf_reverse<T0>(Src0);
  return Result[0];
}

// esimd_bf_insert
template <typename T0, typename T1, int SZ, typename U, typename V, typename W>
ESIMD_NODEBUG ESIMD_INLINE
    typename std::enable_if<std::is_integral<T1>::value, simd<T0, SZ>>::type
    esimd_bf_insert(U src0, V src1, W src2, simd<T1, SZ> src3) {
  typedef typename details::dword_type<T1>::type DT1;
  static_assert(std::is_integral<DT1>::value && sizeof(DT1) == sizeof(int),
                "operand conversion failed");
  simd<DT1, SZ> Src0 = src0;
  simd<DT1, SZ> Src1 = src1;
  simd<DT1, SZ> Src2 = src2;
  simd<DT1, SZ> Src3 = src3;

  return __esimd_bfins<DT1>(Src0, Src1, Src2, Src3);
}

template <typename T0, typename T1, typename T2, typename T3, typename T4>
ESIMD_NODEBUG ESIMD_INLINE
    typename std::enable_if<details::is_esimd_scalar<T0>::value &&
                                details::is_esimd_scalar<T4>::value,
                            typename std::remove_const<T0>::type>::type
    esimd_bf_insert(T1 src0, T2 src1, T3 src2, T4 src3) {
  simd<T4, 1> Src3 = src3;
  simd<T0, 1> Result = esimd_bf_insert<T0>(src0, src1, src2, Src3);
  return Result[0];
}

// esimd_bf_extract
template <typename T0, typename T1, int SZ, typename U, typename V>
ESIMD_NODEBUG ESIMD_INLINE
    typename std::enable_if<std::is_integral<T1>::value, simd<T0, SZ>>::type
    esimd_bf_extract(U src0, V src1, simd<T1, SZ> src2) {
  typedef typename details::dword_type<T1>::type DT1;
  static_assert(std::is_integral<DT1>::value && sizeof(DT1) == sizeof(int),
                "operand conversion failed");
  simd<DT1, SZ> Src0 = src0;
  simd<DT1, SZ> Src1 = src1;
  simd<DT1, SZ> Src2 = src2;

  return __esimd_bfext<DT1>(Src0, Src1, Src2);
}

template <typename T0, typename T1, typename T2, typename T3>
ESIMD_NODEBUG ESIMD_INLINE
    typename std::enable_if<details::is_esimd_scalar<T0>::value &&
                                details::is_esimd_scalar<T3>::value,
                            typename std::remove_const<T0>::type>::type
    esimd_bf_extract(T1 src0, T2 src1, T3 src2) {
  simd<T3, 1> Src2 = src2;
  simd<T0, 1> Result = esimd_bf_extract<T0>(src0, src1, Src2);
  return Result[0];
}

////////////////////////////////////////////////////////////////////////////////
// ESIMD arithmetic intrinsics:
//
// inv, log, exp, sqrt, rsqrt, sin, cos
//
// share the same requirements.
//
// template <int SZ>
// simd<float, SZ>
// ESIMD_INLINE esimd_inv(simd<float, SZ> src0, int flag = GENX_NOSAT) {
//   simd<float, SZ> Result = __esimd_inv(src0);
//   if (flag != GENX_SAT)
//     return Result;
//   return __esimd_sat<float>(Result);
// }
//
// template <int N1, int N2>
// ESIMD_NODEBUG ESIMD_INLINE
// simd<float, N1 * N2>
// esimd_inv(matrix<float, N1, N2> src0, int flag = GENX_NOSAT) {
//   simd<float, N1 * N2> Src0 = src0;
//   return esimd_inv(Src0, flag);
// }
//
// ESIMD_INLINE float esimd_inv(float src0, int flag = GENX_NOSAT) {
//   simd<float, 1> Src0 = src0;
//   simd<float, 1> Result = esimd_inv(Src0, flag);
//   return Result[0];
// }
//
// we also make the scalar version template-based by adding
// a "typename T". Since the type can only be float, we hack it
// by defining T=void without instantiating it to be float.

#define ESIMD_INTRINSIC_DEF(type, name)                                        \
  template <int SZ>                                                            \
  ESIMD_NODEBUG ESIMD_INLINE simd<type, SZ> esimd_##name(                      \
      simd<type, SZ> src0, int flag = GENX_NOSAT) {                            \
    simd<type, SZ> Result = __esimd_##name<SZ>(src0.data());                   \
    if (flag != GENX_SAT)                                                      \
      return Result;                                                           \
    return esimd_sat<type>(Result);                                            \
  }                                                                            \
  template <typename T = void>                                                 \
  ESIMD_NODEBUG ESIMD_INLINE type esimd_##name(type src0,                      \
                                               int flag = GENX_NOSAT) {        \
    simd<type, 1> Src0 = src0;                                                 \
    simd<type, 1> Result = esimd_##name(Src0, flag);                           \
    return Result[0];                                                          \
  }

ESIMD_INTRINSIC_DEF(float, inv)
ESIMD_INTRINSIC_DEF(float, log)
ESIMD_INTRINSIC_DEF(float, exp)
ESIMD_INTRINSIC_DEF(float, sqrt)
ESIMD_INTRINSIC_DEF(float, sqrt_ieee)
ESIMD_INTRINSIC_DEF(float, rsqrt)
ESIMD_INTRINSIC_DEF(float, sin)
ESIMD_INTRINSIC_DEF(float, cos)

ESIMD_INTRINSIC_DEF(double, sqrt_ieee)

#undef ESIMD_INTRINSIC_DEF

#define ESIMD_INTRINSIC_DEF(ftype, name)                                       \
  template <int SZ, typename U>                                                \
  ESIMD_NODEBUG ESIMD_INLINE simd<ftype, SZ> esimd_##name(                     \
      simd<ftype, SZ> src0, U src1, int flag = GENX_NOSAT) {                   \
    simd<ftype, SZ> Src1 = src1;                                               \
    simd<ftype, SZ> Result = __esimd_##name<SZ>(src0.data(), Src1.data());     \
    if (flag != GENX_SAT)                                                      \
      return Result;                                                           \
                                                                               \
    return esimd_sat<ftype>(Result);                                           \
  }                                                                            \
  template <int SZ, typename U>                                                \
  ESIMD_NODEBUG ESIMD_INLINE                                                   \
      typename std::enable_if<details::is_esimd_scalar<U>::value,              \
                              simd<ftype, SZ>>::type                           \
          esimd_##name(U src0, simd<ftype, SZ> src1, int flag = GENX_NOSAT) {  \
    simd<ftype, SZ> Src0 = src0;                                               \
    return esimd_##name(Src0, src1, flag);                                     \
  }                                                                            \
  ESIMD_NODEBUG ESIMD_INLINE ftype esimd_##name(ftype src0, ftype src1,        \
                                                int flag = GENX_NOSAT) {       \
    simd<ftype, 1> Src0 = src0;                                                \
    simd<ftype, 1> Src1 = src1;                                                \
    simd<ftype, 1> Result = esimd_##name(Src0, Src1, flag);                    \
    return Result[0];                                                          \
  }

ESIMD_INTRINSIC_DEF(float, pow)

ESIMD_INTRINSIC_DEF(float, div_ieee)
ESIMD_INTRINSIC_DEF(double, div_ieee)

#undef ESIMD_INTRINSIC_DEF

// esimd_sincos
template <int SZ, typename U>
ESIMD_NODEBUG ESIMD_INLINE simd<float, SZ>
esimd_sincos(simd<float, SZ> &dstcos, U src0, int flag = GENX_NOSAT) {
  dstcos = esimd_cos(src0, flag);
  return esimd_sin(src0, flag);
}

// esimd_atan

#define ESIMD_HDR_CONST_PI 3.1415926535897932384626433832795

template <typename T, int SZ>
ESIMD_NODEBUG ESIMD_INLINE
    typename std::enable_if<std::is_floating_point<T>::value, simd<T, SZ>>::type
    esimd_atan(simd<T, SZ> src0, int flag = GENX_NOSAT) {
  simd<T, SZ> Src0 = esimd_abs(src0);

  simd<ushort, SZ> Neg = src0 < T(0.0);
  simd<ushort, SZ> Gt1 = Src0 > T(1.0);

  Src0.merge(esimd_inv(Src0), Gt1);

  simd<T, SZ> Src0P2 = Src0 * Src0;
  simd<T, SZ> Src0P4 = Src0P2 * Src0P2;

  simd<T, SZ> Result = (Src0P4 * T(0.185696) +
                        ((Src0 * T(0.787997) + T(0.63693)) * Src0P2) + Src0) /
                       (((((Src0 * -T(0.000121387) + T(0.00202308)) * Src0P2) +
                          (Src0 * -T(0.0149145)) + T(0.182569)) *
                         Src0P4) +
                        ((Src0 * T(0.395889) + T(1.12158)) * Src0P2) +
                        (Src0 * T(0.636918)) + T(1.0));

  Result.merge(Result - T(ESIMD_HDR_CONST_PI / 2.0), Gt1);
  Result.merge(Result, Neg);

  if (flag != GENX_SAT)
    return Result;

  return esimd_sat<T>(Result);
}

template <typename T>
ESIMD_NODEBUG ESIMD_INLINE
    typename std::enable_if<std::is_floating_point<T>::value, T>::type
    esimd_atan(T src0, int flag = GENX_NOSAT) {
  simd<T, 1> Src0 = src0;
  simd<T, 1> Result = esimd_atan(Src0, flag);
  return Result[0];
}

// esimd_acos

template <typename T, int SZ>
ESIMD_NODEBUG ESIMD_INLINE
    typename std::enable_if<std::is_floating_point<T>::value, simd<T, SZ>>::type
    esimd_acos(simd<T, SZ> src0, int flag = GENX_NOSAT) {
  simd<T, SZ> Src0 = esimd_abs(src0);

  simd<ushort, SZ> Neg = src0 < T(0.0);
  simd<ushort, SZ> TooBig = Src0 >= T(0.999998);

  // Replace oversized values to ensure no possibility of sqrt of
  // a negative value later
  Src0.merge(T(0.0), TooBig);

  simd<T, SZ> Src01m = T(1.0) - Src0;

  simd<T, SZ> Src0P2 = Src01m * Src01m;
  simd<T, SZ> Src0P4 = Src0P2 * Src0P2;

  simd<T, SZ> Result =
      (((Src01m * T(0.015098965761299077) - T(0.005516443930088506)) * Src0P4) +
       ((Src01m * T(0.047654245891495528) + T(0.163910606547823220)) * Src0P2) +
       Src01m * T(2.000291665285952400) - T(0.000007239283986332)) *
      esimd_rsqrt(Src01m * T(2.0));

  Result.merge(T(0.0), TooBig);
  Result.merge(T(ESIMD_HDR_CONST_PI) - Result, Neg);

  if (flag != GENX_SAT)
    return Result;

  return esimd_sat<T>(Result);
}

template <typename T>
ESIMD_NODEBUG ESIMD_INLINE
    typename std::enable_if<std::is_floating_point<T>::value, T>::type
    esimd_acos(T src0, int flag = GENX_NOSAT) {
  simd<T, 1> Src0 = src0;
  simd<T, 1> Result = esimd_acos(Src0, flag);
  return Result[0];
}

// esimd_asin

template <typename T, int SZ>
ESIMD_NODEBUG ESIMD_INLINE
    typename std::enable_if<std::is_floating_point<T>::value, simd<T, SZ>>::type
    esimd_asin(simd<T, SZ> src0, int flag = GENX_NOSAT) {
  simd<ushort, SZ> Neg = src0 < T(0.0);

  simd<T, SZ> Result =
      T(ESIMD_HDR_CONST_PI / 2.0) - esimd_acos(esimd_abs(src0));

  Result.merge(-Result, Neg);

  if (flag != GENX_SAT)
    return Result;

  return esimd_sat<T>(Result);
}

template <typename T>
ESIMD_NODEBUG ESIMD_INLINE
    typename std::enable_if<std::is_floating_point<T>::value, T>::type
    esimd_asin(T src0, int flag = GENX_NOSAT) {
  simd<T, 1> Src0 = src0;
  simd<T, 1> Result = esimd_asin(Src0, flag);
  return Result[0];
}

////////////////////////////////////////////////////////////////////////////////
// Rounding intrinsics.
////////////////////////////////////////////////////////////////////////////////

#define ESIMD_INTRINSIC_DEF(name)                                              \
  template <typename T, int SZ>                                                \
  ESIMD_NODEBUG ESIMD_INLINE simd<T, SZ> esimd_##name(simd<float, SZ> src0,    \
                                                      int flag = GENX_NOSAT) { \
    simd<float, SZ> Result = __esimd_##name(src0.data());                      \
    if (flag != GENX_SAT)                                                      \
      return Result;                                                           \
    return esimd_sat<T>(Result);                                               \
  }                                                                            \
  template <typename T>                                                        \
  ESIMD_NODEBUG ESIMD_INLINE T esimd_##name(float src0,                        \
                                            int flag = GENX_NOSAT) {           \
    simd<float, 1> Src0 = src0;                                                \
    simd<T, 1> Result = esimd_##name<T>(Src0, flag);                           \
    return Result[0];                                                          \
  }

ESIMD_INTRINSIC_DEF(rndd)
ESIMD_INTRINSIC_DEF(rndu)
ESIMD_INTRINSIC_DEF(rnde)
ESIMD_INTRINSIC_DEF(rndz)

#undef ESIMD_INTRINSIC_DEF

template <int N>
ESIMD_NODEBUG ESIMD_INLINE
    typename std::enable_if<(N == 8 || N == 16 || N == 32), uint>::type
    esimd_pack_mask(simd<ushort, N> src0) {
  return __esimd_pack_mask<N>(src0.data());
}

template <int N>
ESIMD_NODEBUG ESIMD_INLINE
    typename std::enable_if<(N == 8 || N == 16 || N == 32),
                            simd<ushort, N>>::type
    esimd_unpack_mask(uint src0) {
  return __esimd_unpack_mask<N>(src0);
}

template <int N>
ESIMD_NODEBUG ESIMD_INLINE
    typename std::enable_if<(N != 8 && N != 16 && N < 32), uint>::type
    esimd_pack_mask(simd<ushort, N> src0) {
  simd<ushort, (N < 8 ? 8 : N < 16 ? 16 : 32)> src_0 = 0;
  src_0.template select<N, 1>() = src0.template format<ushort>();
  return esimd_pack_mask(src_0);
}

/// Count component-wise the total bits set in source operand.
template <typename T, int N>
ESIMD_NODEBUG ESIMD_INLINE
    typename std::enable_if<std::is_integral<T>::value, simd<uint, N>>::type
    esimd_cbit(simd<T, N> src0) {
  return __esimd_cbit(src0.data());
}

template <typename T>
ESIMD_NODEBUG ESIMD_INLINE
    typename std::enable_if<std::is_integral<T>::value &&
                                details::is_esimd_scalar<T>::value,
                            uint>::type
    esimd_cbit(T src) {
  simd<T, 1> Src = src;
  simd<uint, 1> Result = esimd_cbit(Src);
  return Result[0];
}

/// Find component-wise the first bit from LSB side
template <int N>
ESIMD_NODEBUG ESIMD_INLINE simd<unsigned, N> esimd_fbl(simd<unsigned, N> src) {
  return __esimd_fbl(src.data());
}

template <typename T = void>
ESIMD_NODEBUG ESIMD_INLINE unsigned esimd_fbl(unsigned src) {
  simd<unsigned, 1> Src = src;
  simd<unsigned, 1> Result = esimd_fbl(Src);
  return Result[0];
}

/// Find component-wise the first bit from MSB side.
template <int N>
ESIMD_NODEBUG ESIMD_INLINE simd<int, N> esimd_fbh(simd<int, N> src) {
  return __esimd_sfbh(src.data());
}

template <int N>
ESIMD_NODEBUG ESIMD_INLINE simd<unsigned int, N>
esimd_fbh(simd<unsigned int, N> src) {
  return __esimd_ufbh(src.data());
}

template <typename T>
ESIMD_NODEBUG ESIMD_INLINE
    typename std::enable_if<details::is_dword_type<T>::value &&
                                details::is_esimd_scalar<T>::value,
                            T>::type
    esimd_fbh(T src) {
  simd<T, 1> Src = src;
  simd<T, 1> Result = esimd_fbh(Src);
  return Result[0];
}

template <typename T = void> simd<uint, 4> esimd_rdtsc();

template <typename T1, typename T2, typename T3, typename T4, int N>
ESIMD_NODEBUG ESIMD_INLINE typename std::enable_if<
    details::is_dword_type<T1>::value && details::is_dword_type<T2>::value &&
        details::is_dword_type<T3>::value && details::is_dword_type<T4>::value,
    simd<T1, N>>::type
esimd_dp4a(simd<T2, N> src0, simd<T3, N> src1, simd<T4, N> src2,
           int flag = GENX_NOSAT) {
  simd<T2, N> Src0 = src0;
  simd<T3, N> Src1 = src1;
  simd<T4, N> Src2 = src2;
  simd<T1, N> Result = __esimd_dp4a<T1>(Src0.data(), Src1.data(), Src2.data());
  if (flag != GENX_SAT)
    return Result;
  return esimd_sat<T1>(Result);
}

static auto constexpr ESIMD_CONST_E = 2.71828f;
static auto constexpr ESIMD_CONST_PI = 3.14159f;
static auto constexpr ESIMD_CONST_2PI = 6.28318f;

/* smallest such that 1.0+ESIMD_DBL_EPSILON != 1.0 */
static auto constexpr ESIMD_DBL_EPSILON = 0.00001f;

template <typename RT, int SZ>
ESIMD_INLINE simd<RT, SZ> esimd_floor(const simd<float, SZ> src0,
                                      const uint flags = 0) {
  return esimd_rndd<RT, SZ>(src0, flags);
}

template <typename RT>
ESIMD_INLINE RT esimd_floor(const float &src0, const uint flags = 0) {
  return esimd_rndd<RT, 1U>(src0, flags)[0];
}

template <typename RT, int SZ>
ESIMD_INLINE simd<RT, SZ> esimd_ceil(const simd<float, SZ> src0,
                                     const uint flags = 0) {
  return esimd_rndu<RT, SZ>(src0, flags);
}

template <typename RT>
ESIMD_INLINE RT esimd_ceil(const float &src0, const uint flags = 0) {
  return esimd_rndu<RT, 1U>(src0, flags);
}

/* esimd_atan2_fast - a fast atan2 implementation */
/* vector input */
template <int N>
simd<float, N> esimd_atan2_fast(simd<float, N> y, simd<float, N> x,
                                const uint flags = 0);
/* scalar input */
template <typename T> float esimd_atan2_fast(T y, T x, const uint flags = 0);

/* esimd_atan2 - atan2 implementation */
/* For Vector input */
template <int N>
simd<float, N> esimd_atan2(simd<float, N> y, simd<float, N> x,
                           const uint flags = 0);
/* scalar Input */
template <typename T> float esimd_atan2(T y, T x, const uint flags = 0);

/* esimd_fmod: */
/* vector input */
template <int N>
simd<float, N> esimd_fmod(simd<float, N> y, simd<float, N> x,
                          const uint flags = 0);
/* scalar Input */
template <typename T> float esimd_fmod(T y, T x, const uint flags = 0);

/* esimd_sin_emu - EU emulation for sin(x) */
/* For Vector input */
template <int N>
simd<float, N> esimd_sin_emu(simd<float, N> x, const uint flags = 0);
/* scalar Input */
template <typename T> float esimd_sin_emu(T x, const uint flags = 0);

/* esimd_cos_emu - EU emulation for cos(x) */
/* For Vector input */
template <int N>
simd<float, N> esimd_cos_emu(simd<float, N> x, const uint flags = 0);

/* scalar Input */
template <typename T> float esimd_cos_emu(T x, const uint flags = 0);

/* esimd_tanh_cody_waite - Cody-Waite implementation for tanh(x) */
/* float input */
float esimd_tanh_cody_waite(float x);
/* vector input */
template <int N> simd<float, N> esimd_tanh_cody_waite(simd<float, N> x);
/* esimd_tanh - opencl like implementation for tanh(x) */
/* float input */
float esimd_tanh(float x);
/* vector input */
template <int N> simd<float, N> esimd_tanh(simd<float, N> x);

/* ------------------------- Extended Math Routines
 * -------------------------------------------------*/

// For vector input
template <int N>
ESIMD_INLINE simd<float, N> esimd_atan2_fast(simd<float, N> y, simd<float, N> x,
                                             const uint flags) {
  simd<float, N> a0;
  simd<float, N> a1;
  simd<float, N> atan2;

  simd<unsigned short, N> mask = (y >= 0.0f);
  a0.merge(ESIMD_CONST_PI * 0.5f, ESIMD_CONST_PI * 1.5f, mask);
  a1.merge(0, ESIMD_CONST_PI * 2.0f, mask);

  a1.merge(ESIMD_CONST_PI, x < 0.0f);

  simd<float, N> xy = x * y;
  simd<float, N> x2 = x * x;
  simd<float, N> y2 = y * y;

  a0 -= (xy / (y2 + x2 * 0.28f + ESIMD_DBL_EPSILON));
  a1 += (xy / (x2 + y2 * 0.28f + ESIMD_DBL_EPSILON));

  atan2.merge(a1, a0, y2 <= x2);
  if (flags & GENX_SAT)
    atan2 = esimd_sat<float>(atan2);
  return atan2;
}

//   For Scalar Input
template <>
ESIMD_INLINE float esimd_atan2_fast(float y, float x, const uint flags) {
  simd<float, 1> vy = y;
  simd<float, 1> vx = x;
  simd<float, 1> atan2 = esimd_atan2_fast(vy, vx, flags);
  return atan2[0];
}

// esimd_atan2
// For Vector input
template <int N>
ESIMD_INLINE simd<float, N> esimd_atan2(simd<float, N> y, simd<float, N> x,
                                        const uint flags) {
  simd<float, N> v_distance;
  simd<float, N> v_y0;
  simd<float, N> atan2;
  simd<unsigned short, N> mask;

  mask = (x < 0);
  v_y0.merge(ESIMD_CONST_PI, 0, mask);
  v_distance = esimd_sqrt(x * x + y * y);
  mask = (esimd_abs<float>(y) < 0.000001f);
  atan2.merge(v_y0, (2 * esimd_atan((v_distance - x) / y)), mask);
  if (flags & GENX_SAT)
    atan2 = esimd_sat<float>(atan2);

  return atan2;
}

// For Scalar Input
template <> ESIMD_INLINE float esimd_atan2(float y, float x, const uint flags) {
  float v_distance;
  float v_y0;
  simd<float, 1> atan2;
  unsigned short mask;

  mask = (x < 0);
  v_y0 = mask ? ESIMD_CONST_PI : 0;
  v_distance = esimd_sqrt<float>(x * x + y * y);
  mask = (esimd_abs<float>(y) < 0.000001f);
  atan2.merge(v_y0, (2 * esimd_atan((v_distance - x) / y)), mask);
  if (flags & GENX_SAT)
    atan2 = esimd_sat<float>(atan2);

  return atan2[0];
}

// esimd_fmod:
// For Vector input
template <int N>
ESIMD_INLINE simd<float, N> esimd_fmod(simd<float, N> y, simd<float, N> x,
                                       const uint flags) {
  simd<int, N> v_quot;
  simd<float, N> fmod;

  v_quot = convert<int>(y / x);
  fmod = y - x * convert<float>(v_quot);
  if (flags & GENX_SAT)
    fmod = esimd_sat<float>(fmod);

  return fmod;
}

//     For Scalar Input
template <> ESIMD_INLINE float esimd_fmod(float y, float x, const uint flags) {
  int v_quot;
  simd<float, 1> fmod;

  v_quot = (int)(y / x);
  fmod = y - x * v_quot;
  if (flags & GENX_SAT)
    fmod = esimd_sat<float>(fmod);

  return fmod[0];
}

static auto constexpr CMPI = 3.14159265f;

// esimd_sin_emu - EU emulation for sin(x)
// For Vector input
template <int N>
ESIMD_INLINE simd<float, N> esimd_sin_emu(simd<float, N> x, const uint flags) {
  simd<float, N> x1;
  simd<float, N> x2;
  simd<float, N> t3;

  simd<float, N> sign;
  simd<float, N> fTrig;
  simd<float, N> TwoPI(6.2831853f);
  simd<float, N> CmpI(CMPI);
  simd<float, N> OneP(1.f);
  simd<float, N> OneN(-1.f);

  x = esimd_fmod(x, TwoPI);

  x1.merge(CmpI - x, x - CmpI, (x <= CMPI));
  x1.merge(x, (x <= CMPI * 0.5f));
  x1.merge(CmpI * 2 - x, (x > CMPI * 1.5f));

  sign.merge(OneN, OneP, (x > CMPI));

  x2 = x1 * x1;
  t3 = x2 * x1 * 0.1666667f;

  fTrig =
      x1 + t3 * (OneN + x2 * 0.05f *
                            (OneP + x2 * 0.0238095f *
                                        (OneN + x2 * 0.0138889f *
                                                    (OneP - x2 * 0.0090909f))));
  fTrig *= sign;

  if (flags & GENX_SAT)
    fTrig = esimd_sat<float>(fTrig);

  return fTrig;
}

// scalar Input
template <typename T> ESIMD_INLINE float esimd_sin_emu(T x0, const uint flags) {
  simd<float, 1> x1;
  simd<float, 1> x2;
  simd<float, 1> t3;

  simd<float, 1> sign;
  simd<float, 1> fTrig;
  float TwoPI = CMPI * 2.0f;

  simd<float, 1> x = esimd_fmod(x0, TwoPI);

  simd<float, 1> CmpI(CMPI);
  simd<float, 1> OneP(1.f);
  simd<float, 1> OneN(-1.f);

  x1.merge(CmpI - x, x - CmpI, (x <= CMPI));
  x1.merge(x, (x <= CMPI * 0.5f));
  x1.merge(CmpI * 2 - x, (x > CMPI * 1.5f));

  sign.merge(OneN, OneP, (x > CMPI));

  x2 = x1 * x1;
  t3 = x2 * x1 * 0.1666667f;

  fTrig =
      x1 + t3 * (OneN + x2 * 0.05f *
                            (OneP + x2 * 0.0238095f *
                                        (OneN + x2 * 0.0138889f *
                                                    (OneP - x2 * 0.0090909f))));
  fTrig *= sign;

  if (flags & GENX_SAT)
    fTrig = esimd_sat<float>(fTrig);

  return fTrig[0];
}

// esimd_cos_emu - EU emulation for sin(x)
// For Vector input
template <int N>
ESIMD_INLINE simd<float, N> esimd_cos_emu(simd<float, N> x, const uint flags) {
  simd<float, N> x1;
  simd<float, N> x2;
  simd<float, N> t2;
  simd<float, N> t3;

  simd<float, N> sign;
  simd<float, N> fTrig;
  simd<float, N> TwoPI(6.2831853f);
  simd<float, N> CmpI(CMPI);
  simd<float, N> OneP(1.f);
  simd<float, N> OneN(-1.f);

  x = esimd_fmod(x, TwoPI);

  x1.merge(x - CMPI * 0.5f, CmpI * 1.5f - x, (x <= CMPI));
  x1.merge(CmpI * 0.5f - x, (x <= CMPI * 0.5f));
  x1.merge(x - CMPI * 1.5f, (x > CMPI * 1.5f));

  sign.merge(1, -1, ((x < CMPI * 0.5f) | (x >= CMPI * 1.5f)));

  x2 = x1 * x1;
  t3 = x2 * x1 * 0.1666667f;
  fTrig =
      x1 + t3 * (OneN + x2 * 0.05f *
                            (OneP + x2 * 0.0238095f *
                                        (OneN + x2 * 0.0138889f *
                                                    (OneP - x2 * 0.0090909f))));
  fTrig *= sign;

  if (flags & GENX_SAT)
    fTrig = esimd_sat<float>(fTrig);

  return fTrig;
}

// scalar Input
template <typename T> ESIMD_INLINE float esimd_cos_emu(T x0, const uint flags) {
  simd<float, 1> x1;
  simd<float, 1> x2;
  simd<float, 1> t3;

  simd<float, 1> sign;
  simd<float, 1> fTrig;
  float TwoPI = CMPI * 2.0f;

  simd<float, 1> x = esimd_fmod(x0, TwoPI);

  simd<float, 1> CmpI(CMPI);
  simd<float, 1> OneP(1.f);
  simd<float, 1> OneN(-1.f);

  x1.merge(x - CMPI * 0.5f, CmpI * 1.5f - x, (x <= CMPI));
  x1.merge(CmpI * 0.5f - x, (x <= CMPI * 0.5f));
  x1.merge(x - CMPI * 1.5f, (x > CMPI * 1.5f));

  sign.merge(OneP, OneN, ((x < CMPI * 0.5f) | (x >= CMPI * 1.5f)));

  x2 = x1 * x1;
  t3 = x2 * x1 * 0.1666667f;
  fTrig =
      x1 + t3 * (OneN + x2 * 0.05f *
                            (OneP + x2 * 0.0238095f *
                                        (OneN + x2 * 0.0138889f *
                                                    (OneP - x2 * 0.0090909f))));
  fTrig *= sign;

  if (flags & GENX_SAT)
    fTrig = esimd_sat<float>(fTrig);

  return fTrig[0];
}

namespace detail {

template <int N>
ESIMD_INLINE simd<float, N> esimd_tanh_cody_waite_impl(simd<float, N> x) {
  /*
   *      0           x_small             x_medium            x_large
   *  |   x   | rational polynomial | 1 - 2/(1 + exp(2*x)) |  1
   *
   * rational polynomial for single precision = x + x * (g * (p[1] * g + p[0]) /
   * (g + q[0]) g = x^2 p0 = −0.82377 28127 E+00 p1 = −0.38310 10665 E-02 q0 =
   * 0.24713 19654 E+01 q1 = 1.00000 00000 E+00
   *
   */

  constexpr float p0 = -0.8237728127E+00f;
  constexpr float p1 = -0.3831010665E-02f;
  constexpr float q0 = 0.2471319654E+01f;
  constexpr float q1 = 1.0000000000E+00f;
  constexpr float xsmall = 4.22863966691620432990E-04f;
  constexpr float xmedium = 0.54930614433405484570f;
  constexpr float xlarge = 8.66433975699931636772f;
  constexpr float log2E = 1.442695f; // same as esimd_log(e)

  using RT = simd<float, N>;

  RT absX = esimd_abs(x);
  RT g = absX * absX;

  RT sign;
  sign.merge(-1.f, 1.f, x < 0.f);

  auto isLarge = absX > xlarge;
  auto minor = absX <= xlarge;
  auto isGtMed = minor & (absX > xmedium);
  auto isGtSmall = (absX > xsmall) & (absX <= xmedium);

  RT res;
  res.merge(sign, x, isLarge);
  auto temp = esimd_exp(absX * 2.0f * log2E) + 1.f;
  temp = ((temp - 2.f) / temp) * sign;
  res.merge(temp, isGtMed);
  res.merge((absX + absX * g * (g * p1 + p0) / (g + q0)) * sign, isGtSmall);

  return res;
}

template <int N> ESIMD_INLINE simd<float, N> esimd_tanh_impl(simd<float, N> x) {
  /*
   *      0                       x_small                          x_large
   * |    x    |  ( exp(x) - exp(-x) ) / ( exp(x) + exp(-x) )  |      1
   *
   */

  constexpr float xsmall = 0.000045f; // same as exp(-10.0f)
  constexpr float xlarge = 88.f;
  constexpr float log2E = 1.442695f; // same as esimd_log(e)

  using RT = simd<float, N>;

  RT absX = esimd_abs(x);

  RT sign;
  sign.merge(-1.f, 1.f, x < 0.f);

  auto isLarge = (absX > xlarge);
  auto isLessE = (absX <= xlarge);

  RT res;
  res.merge(sign, x, isLarge);

  RT exp;
  exp = esimd_exp(absX * 2.f * log2E);

  res.merge(((exp - 1.f) / (exp + 1.f)) * sign, (absX > xsmall) & isLessE);

  return res;
}
} // namespace detail

/* esimd_tanh_cody_waite - Cody-Waite implementation for tanh(x) */
/* float input */
ESIMD_INLINE float esimd_tanh_cody_waite(float x) {
  return detail::esimd_tanh_cody_waite_impl(simd<float, 1>(x))[0];
}
/* vector input */
template <int N>
ESIMD_INLINE simd<float, N> esimd_tanh_cody_waite(simd<float, N> x) {
  return detail::esimd_tanh_cody_waite_impl(x);
}

/* esimd_tanh - opencl like implementation for tanh(x) */
/* float input */
ESIMD_INLINE float esimd_tanh(float x) {
  return detail::esimd_tanh_impl(simd<float, 1>(x))[0];
}
/* vector input */
template <int N> ESIMD_INLINE simd<float, N> esimd_tanh(simd<float, N> x) {
  return detail::esimd_tanh_impl(x);
}

// reduction functions
namespace detail {

template <typename T0, typename T1, int SZ> struct esimd_apply_sum {
  template <typename... T>
  simd<T0, SZ> operator()(simd<T1, SZ> v1, simd<T1, SZ> v2) {
    return v1 + v2;
  }
};

template <typename T0, typename T1, int SZ> struct esimd_apply_prod {
  template <typename... T>
  simd<T0, SZ> operator()(simd<T1, SZ> v1, simd<T1, SZ> v2) {
    return v1 * v2;
  }
};

template <typename T0, typename T1, int SZ> struct esimd_apply_reduced_max {
  template <typename... T>
  simd<T0, SZ> operator()(simd<T1, SZ> v1, simd<T1, SZ> v2) {
    if constexpr (std::is_floating_point<T1>::value) {
      return __esimd_reduced_fmax<T1, SZ>(v1, v2);
    } else if constexpr (std::is_unsigned<T1>::value) {
      return __esimd_reduced_umax<T1, SZ>(v1, v2);
    } else {
      return __esimd_reduced_smax<T1, SZ>(v1, v2);
    }
  }
};

template <typename T0, typename T1, int SZ> struct esimd_apply_reduced_min {
  template <typename... T>
  simd<T0, SZ> operator()(simd<T1, SZ> v1, simd<T1, SZ> v2) {
    if constexpr (std::is_floating_point<T1>::value) {
      return __esimd_reduced_fmin<T1, SZ>(v1, v2);
    } else if constexpr (std::is_unsigned<T1>::value) {
      return __esimd_reduced_umin<T1, SZ>(v1, v2);
    } else {
      return __esimd_reduced_smin<T1, SZ>(v1, v2);
    }
  }
};

template <typename T0, typename T1, int SZ,
          template <typename RT, typename T, int N> class OpType>
T1 esimd_reduce_single(simd<T1, SZ> v) {
  if constexpr (SZ == 1) {
    return v[0];
  } else {
    static_assert(__esimd::isPowerOf2(SZ),
                  "Invaid input for esimd_reduce_single - the vector size must "
                  "be power of two.");
    constexpr int N = SZ / 2;
    simd<T0, N> tmp = OpType<T0, T1, N>()(v.template select<N, 1>(0),
                                          v.template select<N, 1>(N));
    return esimd_reduce_single<T0, T0, N, OpType>(tmp);
  }
}

template <typename T0, typename T1, int N1, int N2,
          template <typename RT, typename T, int N> class OpType>
T1 esimd_reduce_pair(simd<T1, N1> v1, simd<T1, N2> v2) {
  if constexpr (N1 == N2) {
    simd<T0, N1> tmp = OpType<T0, T1, N1>()(v1, v2);
    return esimd_reduce_single<T0, T0, N1, OpType>(tmp);
  } else if constexpr (N1 < N2) {
    simd<T0, N1> tmp1 = OpType<T0, T1, N1>()(v1, v2.template select<N1, 1>(0));
    constexpr int N = N2 - N1;
    using NT = simd<T0, N>;
    NT tmp2 = convert<T0>(v2.template select<N, 1>(N1).read());
    return esimd_reduce_pair<T0, T0, N1, N, OpType>(tmp1, tmp2);
  } else {
    static_assert(
        __esimd::isPowerOf2(N1),
        "Invaid input for esimd_reduce_pair - N1 must be power of two.");
    constexpr int N = N1 / 2;
    simd<T0, N> tmp = OpType<T0, T1, N>()(v1.template select<N, 1>(0),
                                          v1.template select<N, 1>(N));
    using NT = simd<T0, N2>;
    NT tmp2 = convert<T0>(v2);
    return esimd_reduce_pair<T0, T0, N, N2, OpType>(tmp, tmp2);
  }
}

template <typename T0, typename T1, int SZ,
          template <typename RT, typename T, int N> class OpType>
T1 esimd_reduce(simd<T1, SZ> v) {
  constexpr bool isPowerOf2 = __esimd::isPowerOf2(SZ);
  if constexpr (isPowerOf2) {
    return esimd_reduce_single<T0, T1, SZ, OpType>(v);
  } else {
    constexpr unsigned N1 = 1u << __esimd::log2(SZ);
    constexpr unsigned N2 = SZ - N1;

    simd<T1, N1> v1 = v.template select<N1, 1>(0);
    simd<T1, N2> v2 = v.template select<N2, 1>(N1);
    return esimd_reduce_pair<T0, T1, N1, N2, OpType>(v1, v2);
  }
};

template <typename T0, typename T1, int SZ>
ESIMD_INLINE ESIMD_NODEBUG T0 esimd_sum(simd<T1, SZ> v) {
  using TT = compute_type_t<simd<T1, SZ>>;
  using RT = typename TT::element_type;
  T0 retv = esimd_reduce<RT, T1, SZ, esimd_apply_sum>(v);
  return retv;
}

template <typename T0, typename T1, int SZ>
ESIMD_INLINE ESIMD_NODEBUG T0 esimd_prod(simd<T1, SZ> v) {
  using TT = compute_type_t<simd<T1, SZ>>;
  using RT = typename TT::element_type;
  T0 retv = esimd_reduce<RT, T1, SZ, esimd_apply_prod>(v);
  return retv;
}

} // namespace detail

template <typename T0, typename T1, int SZ>
ESIMD_INLINE ESIMD_NODEBUG T0 hmax(simd<T1, SZ> v) {
  T0 retv =
      detail::esimd_reduce<T1, T1, SZ, detail::esimd_apply_reduced_max>(v);
  return retv;
}

template <typename T0, typename T1, int SZ>
ESIMD_INLINE ESIMD_NODEBUG T0 hmin(simd<T1, SZ> v) {
  T0 retv =
      detail::esimd_reduce<T1, T1, SZ, detail::esimd_apply_reduced_min>(v);
  return retv;
}

template <typename T0, typename T1, int SZ, typename BinaryOperation>
ESIMD_INLINE ESIMD_NODEBUG T0 reduce(simd<T1, SZ> v, BinaryOperation op) {
  if constexpr (std::is_same<remove_cvref_t<BinaryOperation>,
                             std::plus<>>::value) {
    T0 retv = detail::esimd_sum<T0>(v);
    return retv;
  } else if constexpr (std::is_same<remove_cvref_t<BinaryOperation>,
                                    std::multiplies<>>::value) {
    T0 retv = detail::esimd_prod<T0>(v);
    return retv;
  }
}

template <typename T, int N>
simd<T, N> esimd_dp4(simd<T, N> v1, simd<T, N> v2) {
  auto retv = __esimd_dp4<T, N>(v1, v2);
  return retv;
}

} // namespace gpu
} // namespace intel
} // namespace sycl
} // __SYCL_INLINE_NAMESPACE(cl)
