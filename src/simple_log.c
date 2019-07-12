// SPDX-License-Identifier: MIT
/**
 * Copyright (c) 2017 rxi
 * Copyright (c) 2019 Kalray
 */
#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <fcntl.h>
#include <time.h>

#include <simple_log.h>

#ifndef SIMPLE_LOG_MAX_LOGGERS
#define SIMPLE_LOG_MAX_LOGGERS 10
#endif

static struct {
	void *udata;
	log_lock_fn lock;
	FILE *fp;
	const char *logger_names[SIMPLE_LOG_MAX_LOGGERS];
	int level[SIMPLE_LOG_MAX_LOGGERS];
	int logger_count;
} g_logger;

static const char *level_names[] = {
	"FATAL",
	"ERROR",
	"WARN",
	"INFO",
	"DEBUG",
	"TRACE"
};

#ifdef SIMPLE_LOG_USE_COLOR
static const char *level_colors[] = {
	"\x1b[31m",
	"\x1b[38;5;208m",
	"\x1b[38;5;227m",
	"\x1b[32m",
	"\x1b[38;5;75m",
	"\x1b[38;5;62m",
};
#endif

int simple_log_set_loggers(const char *name[], int loggers_count)
{
	int i;
	if (loggers_count > SIMPLE_LOG_MAX_LOGGERS)
		return -1;

	for( i = 0; i < loggers_count; i++) {  
		g_logger.logger_names[i] = name[i];
	}
	g_logger.logger_count = loggers_count;

	return 0;
}

/**
 * parse a string according to the following format:
 * <logger>.<level>:<logger>.<level>:<logger>.<level>...
 * 
 */
static void log_parse_loggers_env(char *value)
{
	int i, log_level;
	char *cur_str = value, *token, *dot_tok;
	const char *log_name;

	while (cur_str != NULL) {
		token = strchr(cur_str, ':');
		if (token != NULL)
			*token = '\0';

		/* Check if a logger matches the name */
 		for (i = 0; i < g_logger.logger_count; i++) {
			log_name = g_logger.logger_names[i];
			if (strncmp(log_name, cur_str, strlen(log_name)) == 0) {
				/* Find the '.' for loglevel */
				dot_tok =  strchr(cur_str, '.');
				/* Invalid string ? */
				if (dot_tok == NULL)
					break;
				log_level = atoi(dot_tok + 1);
				simple_log_set_logger_level(i, log_level);
			}
		}

		if (token != NULL) {
			*token = '\0';
			cur_str = token + 1;
		} else {
			cur_str = NULL;
		}
	}
}


static void log_setup_file_output(const char *env_var)
{
	char *file;
	char *str = malloc(strlen(env_var) + strlen("_OUTPUT") + 1);
	if (!str)
		return;

	sprintf(str, "%s_OUTPUT", env_var);
	file = getenv(str);
	free(str);

	g_logger.fp = fopen(file, "w");
	if (!g_logger.fp)
		return;
}

static void log_list_loggers(void)
{	
	int i;

	printf("Available loggers:\n");
	for( i = 0; i < g_logger.logger_count; i++) {
		printf(" - %s\n", g_logger.logger_names[i]);
	}
}

void simple_log_init(const char *env_var)
{
	char *value = getenv(env_var);

	/* If the env var is set, parse it */
	if (value != NULL) {
		int val = atoi(value);
		if (strncmp(value, "help", 5) == 0) {
			log_list_loggers();
			simple_log_set_level(-1);
		}

		simple_log_set_level(val);
	} else {
		simple_log_set_level(-1);
	}

	/* Parse file output */
	log_setup_file_output(env_var);
	
	if (value != NULL)
		log_parse_loggers_env(value);
}

static void lock(void)  
{
	if (g_logger.lock)
		g_logger.lock(g_logger.udata, 1);
}


static void unlock(void)
{
	if (g_logger.lock)
		g_logger.lock(g_logger.udata, 0);
}

void simple_log_set_udata(void *udata)
{
	g_logger.udata = udata;
}


void simple_log_set_lock(log_lock_fn fn)
{
	g_logger.lock = fn;
}

void simple_log_set_fp(FILE *fp)
{
	g_logger.fp = fp;
}

int simple_log_set_logger_level(int logger, int level)
{
	if (logger > g_logger.logger_count)
		return 1;

	g_logger.level[logger] = level;
	return 0;
}

void simple_log_set_level(int level)
{
	int i;
	for (i = 0; i < g_logger.logger_count; i++)
		g_logger.level[i] = level;
}

void simple_log_pr(int logger, int level, const char *file, int line, const char *fmt, ...)
{
	if (logger >= g_logger.logger_count)
		return;

	if (level > g_logger.level[logger])
		return;

	/* Acquire lock */
	lock();

	char buf[32];
	va_list args;
	/* Get current time */
	time_t t = time(NULL);
	struct tm *lt = localtime(&t);
	const char *logger_name = g_logger.logger_names[logger];

	/* Log to stderr */
	buf[strftime(buf, sizeof(buf), "%H:%M:%S", lt)] = '\0';
#ifdef SIMPLE_LOG_USE_COLOR
	fprintf(
		stderr, "%s %s%-5s\x1b[0m \x1b[90m%s:%d:\x1b[0m [%s] ",
		buf, level_colors[level], level_names[level], file, line, logger_name);
#else
	fprintf(stderr, "%s %-5s %s:%d: [%s] ", buf, level_names[level], file, line, logger_name);
#endif
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
	fflush(stderr);

	/* Log to file */
	if (g_logger.fp) {
		buf[strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", lt)] = '\0';
		fprintf(g_logger.fp, "%s %-5s %s:%d: [%s] ", buf, level_names[level], file, line, logger_name);
		va_start(args, fmt);
		vfprintf(g_logger.fp, fmt, args);
		va_end(args);
		fflush(g_logger.fp);
	}

	/* Release lock */
	unlock();
}
