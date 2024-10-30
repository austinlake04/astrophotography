// Copyright (c) 2024 Austin Lucas Lake
// <git@austinlucaslake.com>
#ifndef ASTROSIGHT_IO_H
#define ASTROSIGHT_IO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include "astrosight/image.h"

#ifndef ASTROSIGHT_NAMESPACE
#define ASTROSIGHT_NAMESPACE as_

#define C_(a,b) a##b
#define C(a,b) C_(a,b)
#define N(a) C(ASTROSIGHT_NAMESPACE, a)

extern uint8_t N(verify_file)(char * filepath);

extern uint8_t N(select_files)(const char pattern[], char ** files, size_t * frames_count);

extern uint8_t N(load_images)(char * filepath, N(image_t) * frames, size_t * frames_count);

#endif // ASTROSIGHT_NAMESPACE

#ifdef __cplusplus
}
#endif

#endif // ASTROSIGHT_IO_H
