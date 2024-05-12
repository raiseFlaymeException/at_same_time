#include <stdio.h>

#define AT_SAME_TIME_IMPL // include the implementation (see stb style library:
                          // https://github.com/nothings/stb)
#include "at_same_time/at_same_time.h"

// constants
#define CAPACITY_FUNCTION_ARRAY 16
#define SLEEP_TIME   10

// arguments struct
typedef struct {
    size_t     nb_thread;
    useconds_t sleep_time_ms;
} AsyncTestArgs;

// the function to execute
void *func(void *args) {
    AsyncTestArgs *arg = args; // get args
    // do a lot of work
    usleep(arg->sleep_time_ms);
    printf("thread: %d\n", arg->nb_thread);
    // send the return value (can be any pointer but be carefull the pointer is steal valid)
    return &arg->nb_thread;
}

int main() {
    // init the library (will allocate memory using malloc)
    AstAsync *async = AstAsync_init(CAPACITY_FUNCTION_ARRAY, SLEEP_TIME);

    AsyncTestArgs args1;
    args1.nb_thread     = 1;
    args1.sleep_time_ms = 1000 * 1000; // NOLINT
    // register the function to be run asynchronously
    AstPromise promise1 = AstAsync_register(async, func, &args1);

    AsyncTestArgs args2;
    args2.nb_thread     = 2;
    args2.sleep_time_ms = 2000 * 1000; // NOLINT
    // register the function to be run asynchronously
    AstPromise promise2 = AstAsync_register(async, func, &args2);

    AsyncTestArgs args3;
    args3.nb_thread     = 3;
    args3.sleep_time_ms = 3000 * 1000; // NOLINT
    // register the function to be run asynchronously
    AstPromise promise3 = AstAsync_register(async, func, &args3);

    // get the result and wait infinitly
    size_t *result1 = AstAsync_get_result(async, promise1, NULL);
    // check if the function finished
    printf("result1 finished: %s\n", (AstAsync_is_func_finished(async, promise1)) ? "true" : "false");
    printf("result2 finished: %s\n", (AstAsync_is_func_finished(async, promise2)) ? "true" : "false");
    printf("result3 finished: %s\n", (AstAsync_is_func_finished(async, promise3)) ? "true" : "false");

    useconds_t timeout = 500; // NOLINT
    // get the result and wait around 500 microseconds
    size_t *result2 = AstAsync_get_result(async, promise2, &timeout);
    printf("result2: %x\n", result2);
    // check if the function finished
    printf("result1 finished: %s\n", (AstAsync_is_func_finished(async, promise1)) ? "true" : "false");
    printf("result2 finished: %s\n", (AstAsync_is_func_finished(async, promise2)) ? "true" : "false");
    printf("result3 finished: %s\n", (AstAsync_is_func_finished(async, promise3)) ? "true" : "false");

    // get the result and wait infinitly
    result2 = AstAsync_get_result(async, promise2, NULL);
    // check if the function finished
    printf("result1 finished: %s\n", (AstAsync_is_func_finished(async, promise1)) ? "true" : "false");
    printf("result2 finished: %s\n", (AstAsync_is_func_finished(async, promise2)) ? "true" : "false");
    printf("result3 finished: %s\n", (AstAsync_is_func_finished(async, promise3)) ? "true" : "false");

    // print result
    printf("result1: %d\n", *result1);
    printf("result2: %d\n", *result2);

    // deallocate memory and join every threads
    AstAsync_quit(async);
    return 0;
}
