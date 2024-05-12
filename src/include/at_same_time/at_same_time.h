#ifndef AT_SAME_TIME_H
#define AT_SAME_TIME_H

#include <assert.h>
#include <pthread.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

#include "easy_c_data_structure/array/carray_creator.h"

// type for a function
typedef void *(*ast_async_func_t)(void *);

// for now an AstPromise is a size_t
typedef size_t AstPromise;

// functions information struct
typedef struct {
    ast_async_func_t func;
    void            *result;
    pthread_t        thread_func;
    void            *args;
    bool             finished;
    bool             started;
} AstFunctionInfo;

// create a function info dynamic array (see
// https://github.com/raiseFlaymeException/easy-c-data-structure)
CARRAY_CREATE_DECLARATION(AstFuncInfoArr, AstFunctionInfo)
// informations about the current async pool
typedef struct {
    pthread_t       thread_async;
    pthread_mutex_t mut;
    useconds_t      sleep_time_micros;
    AstFuncInfoArr  functions_arr;
    bool            run;
} AstAsync;

///
/// @brief create and init a AstAsync
///
/// @param[in] cap_func_arr the capacity for the function array (try to guess how many function
/// you're gonna need to reduce allocation)
/// @param[in] sleep_time_micros the time sleeped between each check for the handler thread
/// @return the asy_async object created and inited
///
AstAsync *AstAsync_init(size_t cap_func_arr, useconds_t sleep_time_micros); // NOLINT
///
/// @brief deinit the AstAsync
///
/// @param[in, out] ast_async join the handle thread, join all thread not finished, and free
/// ast_async
///
void AstAsync_quit(AstAsync *ast_async); // NOLINT

///
/// @brief register a function to call asynchronously
///
/// @param[in, out] ast_async the pool of async to add a function from
/// @param[in] func the function to execute
/// @param[in] args the arguments to pass to the function
/// @return a promise to keep track of stuff
///
AstPromise AstAsync_register(AstAsync *ast_async, ast_async_func_t func, void *args); // NOLINT

///
/// @brief check if the function if finished
///
/// @param[in, out] ast_async the pool of async to search from
/// @param[in] promise the promise to check if is finished
/// @return if the function is finished
///
bool AstAsync_is_func_finished(AstAsync *ast_async, AstPromise promise); // NOLINT
///
/// @brief get the result from the function with an optional timeout
///
/// @param[in, out] ast_async the async pool to read the function from
/// @param[in] promise the promise to get the result
/// @param[in] timeout_micros a pointer to how much time in micro seconds to wait
/// or NULL for wait until we get a result
/// @return the result or NULL if the timeout is exceeded (even if the result==NULL
/// see AstAsync_is_func_finished)
///
void *AstAsync_get_result(AstAsync *ast_async, AstPromise promise, // NOLINT
                          const useconds_t *timeout_micros);

#endif                                                             // AT_SAME_TIME_H
#ifdef AT_SAME_TIME_IMPL

#define S_TO_MS_FACTOR      1000
#define MS_TO_MICROS_FACTOR 1000

size_t get_micros() {
    // TODO: better precision
    struct timeb time_b;

    ftime(&time_b);
    return (S_TO_MS_FACTOR * time_b.time + time_b.millitm) * MS_TO_MICROS_FACTOR;
}

CARRAY_CREATE_DEFINITION(AstFuncInfoArr, AstFunctionInfo)

void *thread_func_handler(void *param) {
    AstFunctionInfo *func_info = param;
    func_info->result          = func_info->func(func_info->args);
    func_info->finished        = true;

    pthread_exit(NULL);
    return NULL;
}

void *thread_async(void *param) {
    AstAsync *ast_async = param;
    bool      run       = true;
    while (run) {
        pthread_mutex_lock(&ast_async->mut);
        for (size_t i = 0; i < ast_async->functions_arr.size; i++) {
            AstFunctionInfo *func_info = ast_async->functions_arr.data + i;
            if (!func_info->started) {
                pthread_create(&func_info->thread_func, NULL, thread_func_handler, func_info);
                func_info->started = true;
            } else if (func_info->finished) {
                pthread_join(func_info->thread_func, NULL);
            }
        }
        run = ast_async->run;
        pthread_mutex_unlock(&ast_async->mut);
        usleep(ast_async->sleep_time_micros);
    }
    return NULL;
}

AstAsync *AstAsync_init(size_t cap_func_arr, useconds_t sleep_time_micros) {
    AstAsync *ast_async          = malloc(sizeof(AstAsync));
    ast_async->sleep_time_micros = sleep_time_micros;
    ast_async->run               = true;

    AstFuncInfoArr_alloc(&ast_async->functions_arr, cap_func_arr);

    pthread_mutex_init(&ast_async->mut, NULL);

    pthread_create(&ast_async->thread_async, NULL, thread_async, ast_async);
    return ast_async;
}
void AstAsync_quit(AstAsync *ast_async) {
    ast_async->run = false;
    pthread_join(ast_async->thread_async, NULL);
    pthread_mutex_destroy(&ast_async->mut);

    for (size_t i = 0; i < ast_async->functions_arr.size; i++) {
        (void)AstAsync_get_result(ast_async, i, NULL);
    }

    AstFuncInfoArr_free(&ast_async->functions_arr);

    free(ast_async);
}

AstPromise AstAsync_register(AstAsync *ast_async, ast_async_func_t func, void *args) {
    AstFunctionInfo info;
    info.func     = func;
    info.started  = false;
    info.finished = false;
    info.args     = args;

    pthread_mutex_lock(&ast_async->mut);
    AstPromise promise = (AstPromise)ast_async->functions_arr.size; // TODO: better index system
    AstFuncInfoArr_append(&ast_async->functions_arr, info);
    pthread_mutex_unlock(&ast_async->mut);

    return promise;
}

bool AstAsync_is_func_finished(AstAsync *ast_async, AstPromise promise) {
    pthread_mutex_lock(&ast_async->mut);

    assert(promise < ast_async->functions_arr.size);
    bool finished = ast_async->functions_arr.data[promise].finished;

    pthread_mutex_unlock(&ast_async->mut);
    return finished;
}
void *AstAsync_get_result(AstAsync *ast_async, AstPromise promise,
                          const useconds_t *timeout_micros) {
    useconds_t start = get_micros();
    while (!AstAsync_is_func_finished(ast_async, promise)) {
        if (timeout_micros && get_micros() - start >= *timeout_micros) { return NULL; }
    }

    pthread_mutex_lock(&ast_async->mut);

    assert(promise < ast_async->functions_arr.size);
    void *result = ast_async->functions_arr.data[promise].result;

    pthread_mutex_unlock(&ast_async->mut);
    return result;
}
#endif // AT_SAME_TIME_IMPL
