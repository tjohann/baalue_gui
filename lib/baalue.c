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

#include "libservice.h"

baalue_nodes_t *
get_active_baalue_nodes(void)
{
	baalue_node_t *array_of_nodes = NULL;
	baalue_nodes_t *baalue_nodes = NULL;

	int n = -1;
	int num_nodes_max = 8;  /* range_max - range_min */
	int count_nodes = 0;
	do {
		array_of_nodes = realloc(array_of_nodes,
					 (count_nodes + 1) * sizeof(baalue_node_t));
		if (array_of_nodes == NULL)
			goto error;
		memset(&array_of_nodes[count_nodes],0 , sizeof(baalue_node_t));

		n = snprintf(array_of_nodes[count_nodes].hostname,
			     HOST_NAME_MAX + 1, "hostname.%d", count_nodes);
		if (n == -1)
			goto error;

		count_nodes++;
	} while (count_nodes < num_nodes_max);

	baalue_nodes = malloc(sizeof(baalue_nodes_t));
	baalue_nodes->array_of_nodes = array_of_nodes;
	baalue_nodes->num_nodes = count_nodes;

	return baalue_nodes;
error:
	baa_errno_msg(_("something went wrong"));

	if (array_of_nodes != NULL)
		free(array_of_nodes);

	return NULL;
}

void
free_baalue_nodes_instance(baalue_nodes_t *baalue_nodes)
{
	if (baalue_nodes != NULL) {
		if (baalue_nodes->array_of_nodes !=NULL)
			free(baalue_nodes->array_of_nodes);
		else
			baa_error_msg(_("baalue_nodes->array_of_nodes == NULL"));
	} else {
		baa_error_msg(_("baalue_nodes == NULL"));
	}
}

int
print_baalue_nodes_instance(baalue_nodes_t *baalue_nodes)
{
	if (baalue_nodes == NULL) {
		baa_error_msg(_("baalue_nodes == NULL"));
		return -1;
	}

	int i = 0;
	for(i = 0; i < baalue_nodes->num_nodes; i++)
		baa_error_msg(_("array_of_nodes[%d].hostname: %s"), i,
			      baalue_nodes->array_of_nodes[i].hostname);

	return 0;
}
