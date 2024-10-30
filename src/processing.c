// Copyright (c) 2024 Austin Lucas Lake
// <git@austinlucaslake.com>
#include "astrosight/processing.h"
#include "astrosight/image.h"
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#ifndef ASTROSIGHT_NAMESPACE
#define ASTROSIGHT_NAMESPACE as_

#define C_(a,b) a##b
#define C(a,b) C_(a,b)
#define N(a) C(ASTROSIGHT_NAMESPACE, a)

float N(fast_inv_sqrt)(float number) {
    const float threehalves = 1.5f;
    float x = number * 0.5f;
    float y = number;
    long i = *(long*) &y;
    i = 0x5f3759df - (i>>1);
    y = *(float*) &i;
    y = y * (threehalves - x*y*y);    
    return y;
}

#define malloc_(size, type) (type*)malloc(size)
#define realloc_(ptr, size, type) (type*)realloc(ptr, size)
#define N(convert_dtype_)(N(image_t) * input_frame, N(image_t) output_frame, type)

uint8_t N(convert_dtype)(N(image_t) * input_frame, N(image_t) * output_frame, const N(DATA_TYPE) dtype) {
    if (input_frame == NULL) { return 1; }
    else if (input_frame->data = NULL) { return 1; }
    else if (output_frame == NULL) { return 1; }
    
    output_frame->dtype = dtype;
    output_frame->height = input_frame->height;
    output_frame->width = input_frame->width; 
    if (output_frame->data == NULL) {
        output_frame->data = (float*)malloc(output_frame->height * output_frame->height * sizeof(float));
    } else {
        output_frame->data = (float*)realloc(output_frame->data, output_frame->height * output_frame->height * sizeof(float));
    }

    for (int i = 0; i < input_frame->width * input_frame->height; ++i;) {
        *output_frame->data++ = (float) *(input_frame->data++);
    }

    return 0;
}

uint8_t calibrate_frames(N(image_t) * frames, const size_t frames_count, N(image_t) * master_dark, N(image_t) * master_flat, N(image_t) * master_dark_flat, N(image_t) * master_bias) {
    if (master_dark != NULL) {
        if ((master_dark->data == NULL) ||
            (master_dark->height != frames->height) ||
            (master_dark->width != frames->width) ||
            (master_dark->channels != frames->channels)) { return 1; }
    }

    if (master_flat != NULL) {
        if ((master_flat->data == NULL) ||
            (master_flat->height != frames->height) ||
            (master_flat->width != frames->width) ||
            (master_flat->channels != frames->channels)) { return 1; }
    }

    if (master_dark_flat != NULL) {
        if ((master_dark_flat->data == NULL) ||
            (master_dark_flat->height != frames->height) ||
            (master_dark_flat->width != frames->width) ||
            (master_dark_flat->channels != frames->channels)) { return 1; }
    }

    if (master_bias != NULL) {
        if ((master_bias->data == NULL) ||
            (master_bias->height != frames->height) ||
            (master_bias->width != frames->width) ||
            (master_bias->channels != frames->channels)) { return 1; }
    }

    const size_t pixel_count = frames->height * frames->width * frames->channels;

    if (master_dark != NULL && master_dark->data != NULL) {
        for (size_t i = 0; i < frames_count; ++i) {
            for (size_t j = 0; j < pixel_count; ++j) {
                frames[i].data[j] -= master_dark->data[j];
            }
        }
    }

    if (master_flat != NULL && master_flat->data != NULL) {
        if (master_dark_flat != NULL && master_dark_flat->data != NULL) {
            for (size_t i = 0; i < frames_count; ++i) {
                for (size_t j = 0; j < pixel_count; ++j) {
                    frames[i].data[j] /= master_flat->data[j] - master_dark_flat->data[j];
                }
            }
        } else if (master_bias != NULL && master_bias->data != NULL) {
            for (size_t i = 0; i < frames_count; ++i) {
                for (size_t j = 0; j < pixel_count; ++j) {
                    frames[i].data[j] /= master_flat->data[j] - master_bias->data[j];
                }
            }
        } else {
            for (size_t i = 0; i < frames_count; ++i) {
                for (size_t j = 0; j < height*width*channels; ++j) {
                    frames[i].data[j] /= master_flat->data[j];
                }
            }
        }
    }

    return 0;
}

uint8_t N(demosaic_frames)(N(image_t) * frames, const size_t frames_count) {
    for (size_t i = 0; i < frames_count; ++i) {
        if (frames[i].channels != 1) { return 1; }
    }

    const size_t pixel_count = frames->height * frames->width * frames->channels;
    const size_t border_pixel_count = 6 * (frames->height + frames->width) + 12; 
    float * buffer = (float*) calloc(3 * (pixel_count + border_pixel_count) * sizeof(float));

    for (size_t i = 0; i < frames_count; ++i) {
        for (size_t j = 0; j < pixel_count; ++j) {
            int channel = 1;
            if ((j % height) % 2 == 0 && ((j % width) % 2) == 0) { channel = 2; }
            else if ((j % height) % 2 == 1 && ((j % width) % 2) == 1) { channel = 0; }
            
            const size_t indx = 1 + j + 2*(j / width) + (width + 2)*(2*channel + 1);
            buffer[indx] = frames[i].data[j];
        }
       
        for (size_t j = 0; j < pixel_count; ++j) {
            const size_t red_indx = 3 + j + 2*(j / width) + width;
            const size_t green_indx = 7 + j + 2*(j / width) + width*3;
            const size_t blue_indx = 11 + j + 2*(j / width) + width*5;
            const uint8_t not_top_bonus = ((j % height) != 0);
            const uint8_t not_bottom_bonus = ((j % height) != (height - 1));
            const uint8_t not_left_bonus = ((j % width) != 0);
            const uint8_t not_right_bonus = ((j % width) != (width - 1));

            if ((j % height) % 2 == 1 && ((j % width) % 2) == 1) { // red pixel
                // interpolate green
                float sum = buffer[green_indx+1] + buffer[green_indx-1] + buffer[green_indx+(width+2)] + buffer[green_indx-(width+2)]; 
                uint8_t count = 2 + not_bottom_bonus + not_right_bonus;
                buffer[green_indx] = sum / count;
                
                // interpolate blue
                sum = buffer[blue_indx+(width+2)+1] + buffer[blue_indx+(width+2)-1] + buffer[blue_indx-(width+2)+1] + buffer[blue_indx-(width+2)-1]; 
                count = pow(2, not_bottom_bonus + not_right_bonus);
                buffer[blue_indx] = sum / count;
            } else if ((j % height) % 2 == 0 && ((j % width) % 2) == 0) { // blue pixel
                // interpolate red
                float sum = buffer[red_indx+(width+2)+1] + buffer[red_indx+(width+2)-1] + buffer[red_indx-(width+2)+1] + buffer[red_indx-(width+2)-1]; 
                uint8_t count = pow(2, not_top_bonus + not_left_bonus);
                buffer[red_indx] = sum / count;
                
                // interpolate green
                sum = buffer[green_indx+1] + buffer[green_indx-1] + buffer[green_indx+(width+2)] + buffer[indx-(width+2)]; 
                count = 2 + not_top_bonus + not_left_bonus;
                buffer[green_indx] = sum / count;
            } else { // green pixel
                if ((j % height) % 2 == 0) {
                    // interpolate red
                    float sum = buffer[red_indx+(width+2)] + buffer[red_indx-(width+2)];
                    uint8_t count = 1 + not_top_bonus;
                    buffer[red_indx] = sum / count;
                    
                    // interpolate blue
                    sum = buffer[blue_indx-1] + buffer[blue_indx+1];
                    count = 1 + not_right_bonus;
                    buffer[blue_indx] = sum / count;
                } else {
                    // interpolate red
                    float sum = buffer[red_indx-1] + buffer[red_indx+1];
                    uint8_t count = 1 + not_left_bonus;
                    buffer[red_indx] = sum / count;
                    
                    // interpolate blue
                    sum = buffer[blue_indx+(width+2)] + buffer[blue_indx-(width+2)];
                    count = 1 + not_bottom_bonus;
                    buffer[blue_indx] = sum / count;
                }
            }
        }

        frames[i].data = (float*)realloc(frames[i].data, pixel_count * sizeof(float));
        for (size_t j = 0; i < 3*height; ++j) {
            size_t offset = 3 + width + 2*(j%height) + (width+2)*(height+2)*(j/height);
            memcpy(frames[i].data + j*width, buffer+offset, width * sizeof(float));
        }
        if (i != frames_count - 1) { memset(buffer, 0.0f, ); }
    }
    free(buffer);

    return 0;
}

uint8_t N(register_frames)(N(image_t) * frames, const size_t frames_count, N(image_t) * reference_frame) {
    if (reference_frame == NULL) { return 1; }
    bool reference_frame_found = false;
    for (size_t i = 0; i < frames_count; ++i) {
        if ((frames + i) == reference_frame) { reference_frame_found = true; }
    }
    if (!reference_frame_found) ( return 1; )

    const size_t pixel_count = frames->height * frames->width * frames->channels;
}

N(image_t) * N(mean_frame)(N(image_t) * frames, const size_t frames_count) {
    if (N(validate_frames)(frames, frames_count) != 0) { return NULL; }
    
    N(image_t) * output = N(create_image_ptr)(height, width, channels);
    
    const size_t pixel_count = frames->height * frames->width * frames->channels;

    for (size_t i = 0; i < pixel_count; ++i) {
        float sum = 0.0f;
        for (size_t j = 0; j < frames_count; ++j) {
            sum += frames[j]->data[i];
        }
        output->data[i] = sum / frames_count;
    }

    return output;
}

N(image_t) * N(median_frame)(N(image_t) * frames, const size_t frames_count, ) {
    N(image_t) * output = N(create_image_ptr)(frames->height, frames->width, frames->channels);

    const size_t pixel_count = frames->height * frames->width * frames->channels;
    
    for (size_t i = 0; i < pixel_count; ++i) {
        float arr[frames_count];
        arr[0] = frames[0].data[i]
        for (size_t j = 1; j < frames_count; ++j) {
            arr[j] = frames[j].data[i];
            float key = arr[j];
            size_t k = j - 1;

            while (k >= 0 && arr[k] < key) {
                arr[k+1] = arr[k];
                --k;
            }

            arr[k+1] = key;
        }
        
        output->data[i] = arr[frames_count/2] + (frames_count % 2 == 0) * 0.5f * (arr[(frames_count/2)-1] - arr[frames_count/2]);
    }
}

N(image_t) * N(clipped_mean_frame)(N(image_t) * frames, const size_t frames_count, const float cutoff_threshold) {
    N(image_t) * output = N(create_image_ptr)(frames->height, frames->width, frames->channels);
    
    const size_t pixel_count = frames->height * frames->width * frames->channels;

    for (size_t i = 0; i < pixel_count; ++i) {
        float mean = 0.0f;
        for (size_t j = 0; j < frames_count; ++j) {
            mean += frames[j].data[i];
        }
        mean /= frames_count;
        
        float sigma = 0.0f;
        for (size_t j = 0; j < frames_count; ++j) {
            sigma += power(frames[j].data[i] - mean, 2);
        }
        sigma = sqrt(sigma / frames_count);

        float clipped_sum = 0.0f;
        for (size_t j = 0; j < frames_count; ++j) {
            if (abs((frames[i].data[j] - mean) / sigma) <= cutoff_threshold) {
                clipped_sum += frames[i].data[j];
            }
        }
        output->data[i] = clipped_sum / frames_count;
    }

    return output;
}

N(image_t) * N(clipped_median_frame)(N(image_t) * frames, const size_t frames_count, const float cutoff_threshold) {
    N(image_t) * output = N(create_image_ptr)(frames->height, frames->width, frames->channels);
    
    const size_t pixel_count = frames->height * frames->width * frames->channels;

    for (size_t i = 0; i < pixel_count; ++i) {
        float mean = 0.0f;
        for (size_t j = 0; j < frames_count; ++j) {
            mean += frames[j].data[i];
        }
        mean /= frames_count;
        
        float sigma = 0.0f;
        for (size_t j = 0; j < frames_count; ++j) {
            sigma += power(frames[j].data[i] - mean, 2);
        }
        sigma = sqrt(sigma/frames_count);

        size_t not_outlier_count = 0;
        for (size_t j = 0; j < frames_count; ++j) {
            if (abs((frames[j].data[i] - mean) / sigma) <= cutoff_threshold) {
                ++not_outlier_count;
            }
        }

        // find median whilst rejecting outliers
        float arr[not_outlier_count];
        for (size_t j = 0; j < frames_count; ++j) {
            if (abs((frames[j].data[i] - mean) / sigma) <= cutoff_threshold) {
                arr[j] = frames[j].data[i];
                float key = arr[j];
                size_t k = j - 1;

                while (k >= 0 && arr[k] < key) {
                    arr[k+1] = arr[k];
                    --k;
                }

                arr[k+1] = key;
            }
        }
        
        output->data[i] = arr[not_outlier_count/2] + (not_outlier_count % 2 == 0) * 0.5f * (arr[(not_outlier_count/2)-1] - arr[not_outlier_count/2]);
    }

uint8_t N(validate_frames)(N(image_t) * frames, const size_t frames_count) {
    size_t height, width;
    uint8_t channels;
    for (size_t i = 0; i < frames_count; ++i) {
        if ((frames + i) == NULL) { return 1; }
        if ((frames[i].data) == NULL) { return 1; }
        if (!height) { height = frames[i].height; }
        else if (frames[i].height != height) { return 1; }
        if (!width) { width = frames[i].width; }
        else if (frames[i].width != width) { return 1; }
        if (!channels) { channels = frames[i].channels; }
        else if (frames[i].channels != channels) { return 1; }
    }
    return 0;
}

#endif // ASTROSIGHT_NAMESPACE
