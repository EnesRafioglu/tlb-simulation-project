#pragma once

//? This file contains some macro vodoo to allow for easy logging in debug mode.
//? Through the check of the DEBUG define, the compiler will optimize out the
//? logging calls in release mode. This is done by the preprocessor, so the
//? compiler will not even see the logging calls in release mode.

#ifdef DEBUG
#include <iostream>
#include <ctime>
#include <iomanip>

#define _debug_log_impl(message) {                                                               \
  const time_t current_time = std::time(nullptr);                                               \
  const tm* local_time = std::localtime(&current_time);                                         \
                                                                                                \
  std::cout << std::put_time(local_time, "%Y-%m-%d %H:%M:%S") << " - " << message << std::endl; \
}
#define _debug_log_scope_impl(scope, message) _debug_log_impl("[" << scope << "] " << message)

#else

#define _debug_log_impl(message)
#define _debug_log_scope_impl(scope, message)

#endif

/// This macro will select the correct debug log implementation based on the number of arguments
#define _select__debug_log_impl(_1, _2, NAME, ...) NAME

/**
 * @brief Log a message when compiled in debug mode.
 *
 * @param scope The scope of the log message (optional) - used when two arguments are passed.
 * @param message The message to log.
 */
#define debug_log(...) _select__debug_log_impl(__VA_ARGS__, _debug_log_scope_impl, _debug_log_impl)(__VA_ARGS__)
