// Copyright (c) 2024 Austin Lucas Lake
// <git@austinlucaslake.com>
#include "astrosight/image.h"
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#ifndef ASTROSIGHT_NAMESPACE
#define ASTROSIGHT_NAMESPACE as_

#define C_(a,b) a##b
#define C(a,b) C_(a,b)
#define N(a) C(ASTROSIGHT_NAMESPACE, a)

N(image_t) N(create_image)(const size_t height, const size_t width, const uint8_t channels) {
    N(image_t) image;
    image.height = height;
    image.width = width;
    image.channels = channels;
    image.data = (float*) malloc(height * width * channels * sizeof(float));
    return image;
}

N(image_t) * N(create_image_ptr)(const size_t height, const size_t width, const uint8_t channels){
    N(image_t) * image = (N(image_t)*) malloc(sizeof(N(image_t)));
    image.height = height;
    image.width = width;
    image.channels = channels;
    image.data = (float*) malloc(height * width * channels * sizeof(float));
    return image;
}

void N(destory_image)(N(image_t) image){
    free(image.data);
}

void N(destory_image_ptr)(N(image_t) * image){
    free(image->data);
    free(image);
}

void N(destroy_images)(N(image_t) * images, const size_t frames_count) {
    for (size_t i = 0; i < frames_count; ++i) { free(images[i].data); }
    free(images);
}

#endif // ASTROSIGHT_NAMESPACE
