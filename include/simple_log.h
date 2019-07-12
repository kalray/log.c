// SPDX-License-Identifier: MIT
/*
 * Copyright (c) 2017 rxi
 * Copyright (c) 2019 Kalray
 */

#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <stdarg.h>

#define LOG_VERSION "0.1.0"

typedef void (*log_lock_fn)(void *udata, int lock);

/**
 * Internal use only
 */
void simple_log_pr(int logger, int level, const char *file, int line, const char *fmt, ...);

/**
 * Log level used for loggers
 */
enum simple_log_loglevel {
	LOG_FATAL,
	LOG_ERROR,
	LOG_WARN,
	LOG_INFO,
	LOG_DEBUG,
	LOG_TRACE
};

/**
 * \brief Logging function
 * \param __logger Logger to log string on
 */
#define simple_log_trace(__logger, ...) simple_log_pr(__logger, LOG_TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define simple_log_debug(__logger, ...) simple_log_pr(__logger, LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define simple_log_info(__logger, ...)  simple_log_pr(__logger, LOG_INFO,  __FILE__, __LINE__, __VA_ARGS__)
#define simple_log_warn(__logger, ...)  simple_log_pr(__logger, LOG_WARN,  __FILE__, __LINE__, __VA_ARGS__)
#define simple_log_error(__logger,...) simple_log_pr(__logger, LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define simple_log_fatal(__logger, ...) simple_log_pr(__logger, LOG_FATAL, __FILE__, __LINE__, __VA_ARGS__)

/**
 * \brief Initialize loggers
 * 
 * \param name Array of logger names (indexed by logger ids)
 * \param loggers_count Count of loggers in this array
 * \return 0 on success
 * \return A negative value on error (too much logger, etc)
 */
int simple_log_set_loggers(const char *name[], int loggers_count);

/**
 * \brief Initialize the logging using the env var value given
 * The env var format should be the following:
 * ENV_VAR=<digit>[:<logger>.<loglevel>]
 * If a digit is given, then it will be used as the global loglevel. 
 * Then if subsequent logger level are given, they will be applied independently
 * To setup a logging file, an environnement variable named ENV_VAR_OUTPUT
 * must be set and point to a file which will be created or truncated if it
 * already exists.
 * 
 * \param env_var environnement variable name to parse value.
 */
void simple_log_init(const char *env_var);

/**
 * \brief Set a logger logging level independently
 * 
 * \param logger Logger to operate on
 * \param level Level of this logger
 * \return 0 on success
 * \return A negative value on error (invalid logger id)
 */
int simple_log_set_logger_level(int logger, int level);

/**
 * \brief Set the global logger value (all loggers will have this value)
 * 
 * \param level Log level to set
 */
void simple_log_set_level(int level);

void simple_log_set_udata(void *udata);
void simple_log_set_lock(log_lock_fn fn);

#endif
