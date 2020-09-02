#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

#include "wsv_error_log.h"
#include "webserv.h"

#define MAX_RECORD	4096

static char			g_error_buf[MAX_RECORD + 4];
static size_t		g_error_len;

static int			g_error_log_fd = -1;

static void
wsv_error_record(const char* m)
{
	char*			s;

	s = strerror(errno);

	g_error_len = 0;
	while ((g_error_buf[g_error_len] = *m) != '\0')
	{
		++g_error_len;
		++m;
	}
	while ((g_error_len < MAX_RECORD) &&
			((g_error_buf[g_error_len] = *s) != '\0'))
	{
		++g_error_len;
		++s;
	}

	if (g_error_len == MAX_RECORD)
	{
		g_error_buf[g_error_len++] = '.';
		g_error_buf[g_error_len++] = '.';
		g_error_buf[g_error_len++] = '.';
		g_error_buf[g_error_len++] = '\n';
	}
	else
		g_error_buf[g_error_len++] = '\n';
}

static void
wsv_logging_error(const char* m)
{
	wsv_error_record(m);
	write(STDERR_FILENO, (void*)g_error_buf, g_error_len);
	webserv_exit(WSV_EXIT_ERR_LOG);
}

void
wsv_error_log(const char* m)
{
	wsv_error_record(m);
	if (write(g_error_log_fd, (void*)g_error_buf, g_error_len) < 0)
		wsv_logging_error("[FATAL ERROR] wsv_error_log(): ");
}

void
wsv_error_log_close(void)
{
	if (g_error_log_fd >= 0 && close(g_error_log_fd) < 0)
		wsv_logging_error("[FATAL ERROR] wsv_error_close(): ");
}

void
wsv_error_log_open(const char* file)
{
	g_error_log_fd = open(file, O_WRONLY | O_CREAT | O_APPEND, 0600);
	if (g_error_log_fd < 0)
		wsv_logging_error("[FATAL ERROR] wsv_error_open(): ");
}
