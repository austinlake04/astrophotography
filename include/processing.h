// Copyright (c) 2024 Austin Lucas Lake
// <git@austinlucaslake.com>
#ifndef ASTROSIGHT_PROCESSING_H
#define ASTROSIGHT_PROCESSING_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>
#include "astrosight/image.h"

#ifndef ASTROSIGHT_NAMESPACE
#define ASTROSIGHT_NAMESPACE as_

#define C_(a,b) a##b
#define C(a,b) C_(a,b)
#define N(a) C(ASTROSIGHT_NAMESPACE, a)

typedef struct {
    size_t x;
    size_t y;
} N(keypoint_t);

extern float N(fast_inv_sqrt)(float number);

extern uint8_t N(convert_dtype)(N(image_t) input_frame, N(image_t) output_frame, N(DATA_TYPE) dtype);

extern uint8_t N(calibrate_frames)(N(image_t) * frames, const size_t frames_count, N(image_t) * master_dark, N(image_t) * master_flat, N(image_t) * master_dark_flat, N(image_t) * master_bias);
 
extern uint8_t N(demosaic_frames)(N(image_t) * frames, const size_t frames_count)

extern uint8_t N(register_frames)(N(image_t) * frames, const size_t frames_count, N(image_t) * reference_frame)

extern uint8_t N(detect_keypoints)(N(image_t) * frame);

extern uint8_t N(compute_descriptors)(N(image_t) * frame);

extern N(image_t) * N(mean_image)(N(image_t) * frames, const size_t frames_count);

extern N(image_t) * N(median_image)(N(image_t) * frames, const size_t frames_count);

extern N(image_t) * N(clipped_mean_image)(N(image_t) * frames, const size_t frames_count);

extern N(image_t) * N(clipped_median_image)(N(image_t) * frames, const size_t frames_count);

extern uint8_t N(validate_frames)(N(image_t) * frames, const size_t frames_count);

#endif // ASTROSIGHT_NAMESPACE

#ifdef __cplusplus
}
#endif

#endif // ASTROSIGHT_PROCESSING_H
