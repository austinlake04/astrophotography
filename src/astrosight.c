// Copyright (c) 2024 Austin Lucas Lake
// <git@austinlucaslake.com>
#include <stdio.h>
#include <stdlib.h>

#ifndef ASTROSIGHT_NAMESPACE
#define ASTROSIGHT_NAMESPACE as_

#define C_(a,b) a##b
#define C(a,b) C_(a,b)
#define N(a) C(ASTROSIGHT_NAMESPACE, a)

void* astrosight(void* saved_state) {
    int * state = (int*) saved_state;
    
    if (state == NULL) {
        state = (int*)malloc(sizeof(int));
        *state = 1;
    }

    printf("Loaded main function! Press any key to hotreload.\n");
    getchar();
    
    return state; 
}
