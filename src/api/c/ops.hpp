/*******************************************************
 * Copyright (c) 2014, ArrayFire
 * All rights reserved.
 *
 * This file is distributed under 3-clause BSD license.
 * The complete license agreement can be obtained at:
 * http://arrayfire.com/licenses/BSD-3-Clause
 ********************************************************/

#pragma once
#include <backend.hpp>
#include <common/half.hpp>
#include <math.hpp>

#ifndef __DH__
#define __DH__
#endif

#include "optypes.hpp"

using namespace detail;

// Because isnan(cfloat) and isnan(cdouble) is not defined
#define IS_NAN(val) !((val) == (val))

template<typename T, af_op_t op>
struct Binary {
    static __DH__ T init();

    __DH__ T operator()(T lhs, T rhs);
};

template<typename T>
struct Binary<T, af_add_t> {
    static __DH__ T init() { return detail::scalar<T>(0); }

    __DH__ T operator()(T lhs, T rhs) { return lhs + rhs; }
};

template<typename T>
struct Binary<T, af_mul_t> {
    static __DH__ T init() { return detail::scalar<T>(1); }

    __DH__ T operator()(T lhs, T rhs) { return lhs * rhs; }
};

template<typename T>
struct Binary<T, af_or_t> {
    static __DH__ T init() { return detail::scalar<T>(0); }

    __DH__ T operator()(T lhs, T rhs) { return lhs || rhs; }
};

template<typename T>
struct Binary<T, af_and_t> {
    static __DH__ T init() { return detail::scalar<T>(1); }

    __DH__ T operator()(T lhs, T rhs) { return lhs && rhs; }
};

template<typename T>
struct Binary<T, af_notzero_t> {
    static __DH__ T init() { return detail::scalar<T>(0); }

    __DH__ T operator()(T lhs, T rhs) { return lhs + rhs; }
};

template<typename T>
struct Binary<T, af_min_t> {
    static __DH__ T init() { return detail::maxval<T>(); }

    __DH__ T operator()(T lhs, T rhs) { return detail::min(lhs, rhs); }
};

template<>
struct Binary<char, af_min_t> {
    static __DH__ char init() { return 1; }

    __DH__ char operator()(char lhs, char rhs) {
        return detail::min(lhs > 0, rhs > 0);
    }
};

#define SPECIALIZE_COMPLEX_MIN(T, Tr)                                       \
    template<>                                                              \
    struct Binary<T, af_min_t> {                                            \
        static __DH__ T init() {                                            \
            return detail::scalar<T>(detail::maxval<Tr>());                 \
        }                                                                   \
                                                                            \
        __DH__ T operator()(T lhs, T rhs) { return detail::min(lhs, rhs); } \
    };

SPECIALIZE_COMPLEX_MIN(cfloat, float)
SPECIALIZE_COMPLEX_MIN(cdouble, double)

#undef SPECIALIZE_COMPLEX_MIN

template<typename T>
struct Binary<T, af_max_t> {
    static __DH__ T init() { return detail::minval<T>(); }

    __DH__ T operator()(T lhs, T rhs) { return detail::max(lhs, rhs); }
};

template<>
struct Binary<char, af_max_t> {
    static __DH__ char init() { return 0; }

    __DH__ char operator()(char lhs, char rhs) {
        return detail::max(lhs > 0, rhs > 0);
    }
};

#define SPECIALIZE_COMPLEX_MAX(T, Tr)                                       \
    template<>                                                              \
    struct Binary<T, af_max_t> {                                            \
        static __DH__ T init() {                                            \
            return detail::scalar<T>(detail::scalar<Tr>(0));                \
        }                                                                   \
                                                                            \
        __DH__ T operator()(T lhs, T rhs) { return detail::max(lhs, rhs); } \
    };

SPECIALIZE_COMPLEX_MAX(cfloat, float)
SPECIALIZE_COMPLEX_MAX(cdouble, double)

#undef SPECIALIZE_COMPLEX_MAX

template<typename Ti, typename To, af_op_t op>
struct Transform {
    __DH__ To operator()(Ti in) { return static_cast<To>(in); }
};

template<typename Ti, typename To>
struct Transform<Ti, To, af_min_t> {
    __DH__ To operator()(Ti in) {
        return IS_NAN(in) ? Binary<To, af_min_t>::init() : To(in);
    }
};

template<typename Ti, typename To>
struct Transform<Ti, To, af_max_t> {
    __DH__ To operator()(Ti in) {
        return IS_NAN(in) ? Binary<To, af_max_t>::init() : To(in);
    }
};

template<typename Ti, typename To>
struct Transform<Ti, To, af_or_t> {
    __DH__ To operator()(Ti in) { return (in != detail::scalar<Ti>(0.)); }
};

template<typename Ti, typename To>
struct Transform<Ti, To, af_and_t> {
    __DH__ To operator()(Ti in) { return (in != detail::scalar<Ti>(0.)); }
};

template<typename Ti, typename To>
struct Transform<Ti, To, af_notzero_t> {
    __DH__ To operator()(Ti in) { return (in != detail::scalar<Ti>(0.)); }
};
