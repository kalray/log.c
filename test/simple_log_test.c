// SPDX-License-Identifier: MIT
/*
 * Copyright (c) 2019 Kalray
 */

#include <stdlib.h>
#include <simple_log.h>

enum my_logger {       
	LOGGER_1,
	LOGGER_2,
	LOGGER_3,
	LOGGER_COUNT,
};

static const char *logger_names[LOGGER_COUNT] =
{
	[LOGGER_1] = "logger1",
	[LOGGER_2] = "logger2",
	[LOGGER_3] = "logger3",
};

int main()
{

	/* Emulate manual settings of TEST_LOG env var */
	setenv("TEST_LOG", "6:logger1.3:logger2.4", 0);

	simple_log_set_loggers(logger_names, LOGGER_COUNT);
	simple_log_init("TEST_LOG");

	simple_log_fatal(LOGGER_1, "Logger 1 fatal\n");
	simple_log_error(LOGGER_1, "Logger 1 error\n");
	simple_log_warn(LOGGER_1, "Logger 1 warn\n");
	simple_log_info(LOGGER_1, "Logger 1 info\n");
	/* These messages wil not be displayed with default configuration */
	simple_log_debug(LOGGER_1, "Logger 1 debug\n");
	simple_log_trace(LOGGER_1, "Logger 1 trace\n");

	simple_log_fatal(LOGGER_2, "Logger 2 fatal\n");
	simple_log_error(LOGGER_2, "Logger 2 error\n");
	simple_log_warn(LOGGER_2, "Logger 2 warn\n");
	simple_log_info(LOGGER_2, "Logger 2 info\n");
	simple_log_debug(LOGGER_2, "Logger 2 debug\n");
	/* This message wil not be displayed with default configuration */
	simple_log_trace(LOGGER_2, "Logger 2 trace\n");

	simple_log_fatal(LOGGER_3, "Logger 3 fatal\n");
	simple_log_error(LOGGER_3, "Logger 3 error\n");
	simple_log_warn(LOGGER_3, "Logger 3 warn\n");
	simple_log_info(LOGGER_3, "Logger 3 info\n");
	simple_log_debug(LOGGER_3, "Logger 3 debug\n");
	simple_log_trace(LOGGER_3, "Logger 3 trace\n");

	return 0;
}