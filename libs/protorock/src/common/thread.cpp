#include "common/common.h"

#if defined(unix) || defined(__unix) || defined(__unix__) || defined(__APPLE__)
#include <errno.h>
#include <pthread.h>
#include <sched.h>
#include <time.h>
#elif defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#include <winternl.h>
#undef Yield
#endif

void ProtoRock::Common::Yield() noexcept {
#if defined(unix) || defined(__unix) || defined(__unix__) || defined(__APPLE__)
    sched_yield();
#elif defined(_WIN32) || defined(_WIN64)
    SwitchToThread();
#endif
}