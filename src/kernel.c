// Copyright (c) 2024 Austin Lucas Lake
// <git@austinlucaslake.com>
#include "astrosight/kernel.h"
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

#ifndef ASTROSIGHT_NAMESPACE
#define ASTROSIGHT_NAMESPACE as_

#define C_(a,b) a##b
#define C(a,b) C_(a,b)
#define N(a) C(ASTROSIGHT_NAMESPACE, a)

N(kernel_t) N(create_kernel)(const uint8_t size){
    N(kernel_t) kernel;
    kernel.size = size;
    const uint16_t resolution = size * size;
    kernel.data = (float*) malloc(resolution * sizeof(float));
    switch (kernel_type) {
        case GUASSIAN:
            const float inv_two_sigma_sq = 4 / resolution;
            const size_t offset = size / 2;
            for (uint16_t i = 0; i < resolution; ++i) {
                const int x = i % size - offset;
                const int y = i / size - offset;
                kernel.data[i] = exp(-(x*x + y*y) * inv_two_sigma_sq) * inv_two_sigma_sq;
            }
        case MEAN:
        default:
            const float mean = 1 / resolution;
            for (uint16_t i = 0; i < size*size; ++i) { kernel.data[i] = mean; }
    }
    return kernel; 
}

N(kernel_t) * N(create_kernel_ptr)(const uint8_t size, N(KERNEL_TYPE) kernel_type){
    N(kernel_t) * kernel = (N(kernel_t)*) malloc(sizeof(N(kernel_t)));
    kernel.size = size;
    const uint16_t resolution = size * size;
    kernel.data = (float*) malloc(resolution * sizeof(float));
    switch (kernel_type) {
        case GUASSIAN:
            const float inv_two_sigma_sq = 4 / resolution;
            const size_t offset = size / 2;
            for (uint16_t i = 0; i < resolution; ++i) {
                const int x = i % size - offset;
                const int y = i / size - offset;
                kernel->data[i] = exp(-(x*x + y*y) * inv_two_sigma_sq) * inv_two_sigma_sq;
            }
        case MEAN:
        default:
            const float mean = 1 / resolution;
            for (uint16_t i = 0; i < resolution; ++i) { kernel->data[i] = mean; }
    }
    return kernel; 
}

void N(destory_kernel)(N(kernel_t) kernel){
    free(kernel.data);
}

void N(destory_kernel_ptr)(N(kernel_t) * kernel){
    free(kernel->data);
}

#endif // ASTROSIGHT_NAMESPACE
