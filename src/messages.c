/*
  GPL
  (c) 2016, thorsten.johannvorderbrueggen@t-online.de

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

*/

#include "common.h"

static void
message_common(int errno_flag, int log_level, const char *fmt, va_list va)
{
	int errno_save = 0;
	if (errno_flag)
		errno_save = errno;

	char buf[MAXLINE];
	memset(buf, 0, MAXLINE);
	vsnprintf(buf, MAXLINE, fmt, va);

	size_t n = strlen(buf);
	if (errno_flag)
		snprintf(buf + n, MAXLINE - n, ": %s", strerror(errno_save));

	strcat(buf, "\n");

	write_to_textfield(buf, log_level);

	if (log_level == LOG_ERR)
		baa_th_error_msg(errno_save, fmt, va);

	if (log_level == LOG_DEBUG)
		baa_debug_msg(fmt, va);
}

/* print error message */
void
write_error_msg(const char *fmt, ...)
{
	va_list	va;

	va_start(va, fmt);
	message_common(0, LOG_ERR, fmt, va);
	va_end(va);
}

/* print errno message */
void
write_errno_msg(const char *fmt, ...)
{
	va_list	va;

	va_start(va, fmt);
	message_common(1, LOG_ERR, fmt, va);
	va_end(va);
}

void
write_info_msg(const char *fmt, ...)
{
	va_list	va;

	va_start(va, fmt);
	message_common(0, LOG_INFO, fmt, va);
	va_end(va);
}

void
write_debug_msg(const char *fmt, ...)
{
	va_list	va;

	va_start(va, fmt);
	message_common(0, LOG_DEBUG, fmt, va);
	va_end(va);
}
