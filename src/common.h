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

#ifndef _BAALUE_COMMON_H_
#define _BAALUE_COMMON_H_

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>


#include "libservice.h"

char *program_name;

/*
 * gui.c
 * =====
 */

void
build_main_window(void);

void
write_to_textfield(const char *message, int log_level);

void
build_help_window(void);

void
build_search_window(void);

void
build_connect_window(void);

void
build_halt_window(void);

void
build_reboot_window(void);


/*
 * threads.c
 * =========
 */

void *
help(void *args);

void *
search_node(void *args);

void *
connect_node(void *args);

void *
halt_node(void *args);

void *
reboot_node(void *args);


/*
 * message_handler.c
 * =================
 */

/* print error message */
void
write_error_msg(const char *fmt, ...);

/* print errno message */
void
write_errno_msg(const char *fmt, ...);

void
write_info_msg(const char *fmt, ...);

void
write_debug_msg(const char *fmt, ...);


#endif
