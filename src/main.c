// Copyright (c) 2024 Austin Lucas Lake
// <git@austinlucaslake.com>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>

int main(void) {
    const bool quiet = true; 
    clock_t start_time, end_time;
    double duration;

    char * folders[5] = { "light", "dark", "flat", "dark_flat", "bias" };
    for (uint8_t i = 0; i < 5; ++i) {
        char * full_pattern = "/media/ubuntu/512MicroSSD/test_data/cocoon/" + folders[0] + "/*.CR2";
        char ** select_files(full_pattern);
        start_time = clock();
        load_frames(files, image_type);
        end_time = clock(); 
        duration = ((double) (end_time - start_time) / CLOCKS_PER_SEC); 
        if (!quiet) {
            printf("time (s) to load %u frames:\t %s\n", frames_count, duration);
        }
    };

    start_time = clock();
    N(master_frames_t) * master_frames;
    if (N(generate_masters)(files, master_frames) != 0) { return 1; }
    end_time = clock(); 
    duration = ((double) (end_time - start_time) / CLOCKS_PER_SEC); 
    if (!quiet) {
        printf("time (s) to generate master frames:\t %d\n", duration);
    }

    start_time = clock(); 
    if (N(calibrate_frames)(light_frames, light_frames_count, master_dark, master_flat, master_dark_flat, master_bias) != 0) { return 1; }
    end_time = clock(); 
    duration = ((double) (end_time - start_time) / CLOCKS_PER_SEC); 
    if (!quiet) {
        printf("time (s) to calibrate %u frames:\t %d\n", light_frames_count, duration);
    }
    
    start_time = clock(); 
    if (N(demosaic_frames)(light_frames, light_frames_count) != 0) { return 1; }
    end_time = clock(); 
    duration = ((double) (end_time - start_time) / CLOCKS_PER_SEC); 
    if (!quiet) {
        printf("time (s) to demosaic %u frames:\t %d\n", light_frames_count, duration);
    } 

    start_time = clock(); 
    if (N(register_frames)(light_frames, light_frames_count) != 0) { return 1; }
    end_time = clock(); 
    duration = ((double) (end_time - start_time) / CLOCKS_PER_SEC); 
    if (!quiet) {
        printf("time (s) to register %u frames:\t %d\n", light_frames_count, duration);
    }
    
    start_time = clock();
    N(image_t) * stacked_image; 
    if (N(stack_frames)(light_frames, light_frames_count, stacked_image) != 0) { return 1; }
    end_time = clock(); 
    duration = ((double) (end_time - start_time) / CLOCKS_PER_SEC); 
    if (!quiet) {
        printf("time (s) to stack %u frames:\t %d\n", light_frames_count, duration);
    }
   
    start_time = clock(); 
    if (N(save_frames)(light_frames, light_frames_count) != 0) { return 1; };
    end_time = clock(); 
    duration = ((double) (end_time - start_time) / CLOCKS_PER_SEC); 
    if (!quiet) {
        printf("time (s) to save %u frames:\t %d\n", light_frames_count, duration);
    }

    return 0;

}
