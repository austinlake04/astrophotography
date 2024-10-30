// Copyright (c) 2024 Austin Lucas Lake
// <git@austinlucaslake.com>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dlfcn.h>

#ifndef ASTROSIGHT_NAMESPACE
#define ASTROSIGHT_NAMESPACE as_

#define C_(a,b) a##b
#define C(a,b) C_(a,b)
#define N(a) C(ASTROSIGHT_NAMESPACE, a)

typedef void* hotreload_func(void* data);

int main(void) {
    void* state = NULL;

    while (true) {
        while (system("make") != 0) {
            fprintf(stderr, "Failed to compile: (%s)\n", dlerror());
            fprintf(stderr, "Press any key to try again.\n");
            getchar();
        }

        void * module = dlopen("./astrosight.o", RTLD_NOW);
        while (module == NULL) {
            fprintf(stderr, "Failed to run executable: (%s)\n", dlerror());
            fprintf(stderr, "Press any key to try again.\n");
            getchar();
        }

        hotrealod_func* hotreload = dlsym(module, "astrosight");
        state = hotreload(state);
        
        dlclose(module);
    }

    free(state);
    
    return EXIT_SUCCESS;
}

#endif // ASTROSIGHT_NAMESPACE
