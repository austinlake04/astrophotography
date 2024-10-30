// Copyright (c) 2024 Austin Lucas Lake
// <git@austinlucaslake.com>
#ifndef ASTROSIGHT_HOTRELOAD_H
#define ASTROSIGHT_HOTRELOAD_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ASTROSIGHT_NAMESPACE
#define ASTROSIGHT_NAMESPACE as_

#define C_(a,b) a##b
#define C(a,b) C_(a,b)
#define N(a) C(ASTROSIGHT_NAMESPACE, a)

extern int hotreload(void);

#endif // ASTROSIGHT_NAMESPACE

#ifdef __cplusplus
}
#endif

#endif // ASTROSIGHT_HOTRELOAD_H
