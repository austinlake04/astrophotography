// Copyright (c) 2024 Austin Lucas Lake
// <git@austinlucaslake.com>
#ifndef ASTROSIGHT_IMAGE_H
#define ASTROSIGHT_IMAGE_H

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
    float * data;
    size_t height;
    size_t width;
    uint8_t channels;
} N(image_t);

extern N(image_t) N(create_image)(const size_t height, const size_t width, const uint8_t channels);

extern N(image_t) * N(create_image_ptr)(const size_t height, const size_t width, const uint8_t channels);

extern void N(destory_image)(N(image_t) image);

extern void N(destory_image_ptr)(N(image_t) * image);

extern void N(destory_images)(N(image_t) * image, const size_t frames_count);

#endif // ASTROSIGHT_NAMESPACE

#ifdef __cplusplus
}
#endif

#endif // ASTROSIGHT_IMAGE_H
