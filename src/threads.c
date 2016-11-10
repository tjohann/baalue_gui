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


void *
help(void *args)
{
	(void) args;

	write_info_msg(_("in help"));
	build_help_window();
	sleep(60);

	return NULL;
}

void *
search_node(void *args)
{
	(void) args;

	write_info_msg(_("in search"));

	printf("GEDOEEENS\n");

	sleep(60);
	return NULL;
}

void *
connect_node(void *args)
{
	(void) args;

	write_info_msg(_("in connect"));
	build_connect_window();
	sleep(60);

	return NULL;
}

void *
halt_node(void *args)
{
	(void) args;

	write_info_msg(_("in halt"));
	build_halt_window();
	sleep(60);

	return NULL;
}

void *
reboot_node(void *args)
{
	(void) args;

	write_info_msg(_("in reboot"));
	build_reboot_window();
	sleep(60);

	return NULL;
}
