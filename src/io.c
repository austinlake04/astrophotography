// Copyright (c) 2024 Austin Lucas Lake
// <git@austinlucaslake.com>

#include "astrosight/io.h"
#include "astrosight/image.h"
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include "magic.h"
//#include <stdio.h>

#ifndef ASTROSIGHT_NAMESPACE
#define ASTROSIGHT_NAMESPACE as_

#define C_(a,b) a##b
#define C(a,b) C_(a,b)
#define N(a) C(ASTROSIGHT_NAMESPACE, a)

uint8_t N(verify_file)(char * filepath){
    const char * magic_full;
    magic_t magic_cookie;
    
    magic_cookie = magic_open(MAGIC_MIME);
    if (magic_cookie == NULL) {
        return 1;
    }
    
    if (magic_load(magic_cookie, NULL) != 0) {
        magic_close(magic_cookie);
        return 1;
    }
    
    magic_full = magic_file(magic_cookie, filepath);
    printf("%s\n", magic_full);
    magic_close(magic_cookie);
    return 0;
}

uint8_t N(select_files)(const char pattern[], char ** files, size_t * frames_count) {
    if (frames_count == NULL) { return 1; }
    glob_t globbuf;
    if (glob(pattern, 0, NULL, &globbuf) != 0) { return 1; }
    files = globbuf.gl_pathv;
    *frames_count = globbuf.gl_pathc;
    globfree(&globbuf);
    return 0;
}

uint8_t N(load_images)(char * filepath, N(image_t) * frames, size_t * frames_count) {
    if (filepath == NULL || char_count == 0) { return 1; }
    return 0;
}

#endif // ASTROSIGHT_NAMESPACE
