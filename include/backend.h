// Copyright (c) 2024 Austin Lucas Lake
// <git@austinlucaslake.com>
#ifndef ASTROSIGHT_BACKEND_H
#define ASTROSIGHT_BACKEND_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
//#include <stdbool.h>
#include <stdint.h>

#ifndef ASTROSIGHT_NAMESPACE
#define ASTROSIGHT_NAMESPACE as_

#define C_(a,b) a##b
#define C(a,b) C_(a,b)
#define N(a) C(ASTROSIGHT_NAMESPACE, a)

typedef struct {
    N(image_t) * light_images;
    N(image_t) master_dark;
    N(image_t) master_bias;
    N(image_t) master_dark_flat;
    N(image_t) master_bias;
    N(image_t) * preview;
} N(backend_t);

typedef enum {
    MONO = 0,
    OSC = 1,
} N(COLOR_MODE);

typedef enum {
    LIGHT = 0,
    DARK = 1,
    FLAT = 2,
    DARK_FLAT = 3,
    BIAS = 4,
    BLUE = 5,
    GREEN = 6,
    RED = 7
} N(IMAGE_TYPE);

#endif // ASTROSIGHT_NAMESPACE

#ifdef __cplusplus
}
#endif

#endif // ASTROSIGHT_BACKEND_H
