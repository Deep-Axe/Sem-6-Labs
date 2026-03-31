/*
 * logger.c — intentionally minimal.
 *
 * All logging is implemented as preprocessor macros in logger.h so the compiler
 * can inline the format strings at every call site with no function-call overhead.
 * This translation unit exists only to satisfy the Makefile SRCS list and prevent
 * an "empty translation unit" diagnostic from strict compilers.
 */
#include "logger.h"
