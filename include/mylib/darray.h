#ifndef MYLIB_DARRAY_H
#define MYLIB_DARRAY_H

#include "compat.h"

#define MYLIB_DARRAY_FOREACH(arr, var) \
    for (c_lib_size_t c_lib_concat(_i_, __LINE__) = 0; \
         c_lib_concat(_i_, __LINE__) < c_lib_darray_size(arr) && ((var) = c_lib_darray_get(arr, c_lib_concat(_i_, __LINE__)), 1); \
         c_lib_concat(_i_, __LINE__)++)

#define MYLIB_MACRO_CONCATENATE(a, b) c_lib_concat(a, b)

#endif