#ifndef LOGGER_H
#define LOGGER_H

/*
 * Three logging macros — all output to the correct stream with a consistent prefix:
 *
 *   LOG_INFO(msg)            -> stdout  "[INFO] <msg>"
 *   LOG_ERROR(msg)           -> stderr  "[ERROR] <func>: <msg>"          (logic/API errors)
 *   LOG_SYSCALL_ERROR(name)  -> stderr  "[ERROR] <func>(<syscall>): <errno-string>"
 *
 * Use LOG_SYSCALL_ERROR immediately after a failing syscall, passing the syscall
 * name as a string literal so the message shows both WHERE in the source the error
 * occurred (__func__) and WHICH syscall failed, e.g.:
 *   "[ERROR] main(bind): Address already in use"
 *
 * Use LOG_ERROR for logic/protocol errors where errno is not meaningful.
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>

#define LOG_INFO(msg) \
    printf("[INFO] %s\n", (msg))

#define LOG_ERROR(msg) \
    fprintf(stderr, "[ERROR] %s: %s\n", __func__, (msg))

#define LOG_SYSCALL_ERROR(syscall) \
    fprintf(stderr, "[ERROR] %s(%s): %s\n", __func__, (syscall), strerror(errno))

#endif
