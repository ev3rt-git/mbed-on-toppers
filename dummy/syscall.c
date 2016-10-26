#include "mbed_debug.h"

void _sbrk() {}
void _exit() {}
void _kill() {}
void _getpid() {}
void osMutexCreate() {}
void osMutexDelete() {}
void osMutexWait() {}
void osMutexRelease() {}
void osMessageGet() {}
void osMessagePut() {}
void osMailCreate() {}
void osMailFree() {}
void osMailAlloc() {}
void osMailGet() {}
void osMailPut() {}
void osSemaphoreCreate() {}
void osSemaphoreDelete() {}
void osSemaphoreWait() {}
void osSemaphoreRelease() {}

// Error handling in mbed_error.c etc.
void error() {
    debug("%s called.", __FUNCTION__);
}
void mbed_assert_internal() {
    debug("%s called.", __FUNCTION__);
}
void mbed_die() {
    debug("%s called.", __FUNCTION__);
}

void main() {
    debug("%s called.", __FUNCTION__);
}

void toppers_mbed_start() {
    mbed_sdk_init();
    mbed_main();
}
