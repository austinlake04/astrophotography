// Copyright (c) 2024 Austin Lucas Lake
// <git@austinlucaslake.com>
#ifndef ASTROSIGHT_KERNEL_H
#define ASTROSIGHT_KERNEL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

#ifndef ASTROSIGHT_NAMESPACE
#define ASTROSIGHT_NAMESPACE as_

#define C_(a,b) a##b
#define C(a,b) C_(a,b)
#define N(a) C(ASTROSIGHT_NAMESPACE, a)

typedef struct {
    float * data;
    uint8_t size;
} N(kernel_t);

typedef enum {
    MEAN = 0,
    GAUSSIAN = 1,
} N(KERNEL_TYPE)

extern N(kernel_t) N(create_kernel)(const uint8_t size, N(KERNEL_TYPE) kernel_type);

extern N(kernel_t) * N(create_kernel_ptr)(const uint8_t size, N(KERNEL_TYPE) kernel_type);

extern void N(destory_kernel)(N(kernel_t) kernel);

extern void N(destory_kernel_ptr)(N(kernel_t) * kernel);

#endif // ASTROSIGHT_NAMESPACE

#ifdef __cplusplus
}
#endif

#endif // ASTROSIGHT_KERNEL_H
