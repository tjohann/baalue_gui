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


/*
 * main window
 */
static GtkWidget *window;
static GtkWidget *vbox;
static GtkWidget *hbox;
static GtkWidget *statusbar;

/*
 * buttonbox on the left of main window
 */
static GtkWidget *buttonbox;
static GtkWidget *search_b;
static GtkWidget *connect_b;
static GtkWidget *reboot_b;
static GtkWidget *halt_b;
static GtkWidget *about_b;
static GtkWidget *help_b;
static GtkWidget *exit_b;

/*
 * textfield
 */
static GtkWidget *textfield;
static GtkWidget *textfield_scrolled;
static GtkWidget *textfield_entry;
static GtkWidget *textfield_table;
static GtkTextBuffer *textfield_buffer;

/*
 * the "rest"
 */
static GtkTooltips *tooltips;


static gboolean
on_delete_event(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	(void) widget;
        (void) event;
	(void) data;

	/*
	  - return FALSE -> "destroy" signal will be sent
	  - return TRUE  -> NO "destroy" signal will be sent
	*/

	GtkWidget *dialog;
	dialog = gtk_message_dialog_new(GTK_WINDOW(window),
					GTK_DIALOG_DESTROY_WITH_PARENT,
					GTK_MESSAGE_QUESTION,
					GTK_BUTTONS_YES_NO,
					_("Are you sure to quit?"));
	gtk_window_set_title(GTK_WINDOW(dialog), "Question");

	gint result = gtk_dialog_run(GTK_DIALOG (dialog));
	gtk_widget_destroy(dialog);

	if (result == GTK_RESPONSE_YES)
		return FALSE;
	else
		return TRUE;
}

static void
exit_function(GtkWidget *widget, gpointer data)
{
	/*
	  For (possible) autosave ...

	  Quit-Button and Quit-Function(s) do autosave
	  via "X" of the wm-window will send "delete-event" which will be handeld
	  dialog box (see on_delete_event@gui.c)
	 */

	BAA_PRINT_LOCATION();

	(void) widget;
	(void) data;
}

static void
toggle_connect_window(GtkWidget *widget, gpointer data)
{
	baalue_node_t *i = data;

	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
		baa_info_msg(_("data: %s is active"), i->hostname);

		if (!g_thread_new("connect", &connect_node, i) != 0)
			write_error_msg(_("could not create the thread"));
	} else {
		baa_info_msg(_("data: %s is passiv"), i->hostname);

		if (!g_thread_new("disconnect", &disconnect_node, i) != 0)
			write_error_msg(_("could not create the thread"));
	}
}

static void
toggle_halt_window(GtkWidget *widget, gpointer data)
{
	baalue_node_t *i = data;

	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
		baa_info_msg(_("data: %s is active"), i->hostname);

		if (!g_thread_new("halt", &halt_node, i) != 0)
			write_error_msg(_("could not create the thread"));
	} else {
		baa_info_msg(_("data: %s is passiv"), i->hostname);
	}
}

static void
toggle_reboot_window(GtkWidget *widget, gpointer data)
{
	baalue_node_t *i = data;

	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
		baa_info_msg(_("data: %s is active"), i->hostname);

		if (!g_thread_new("reboot", &reboot_node, i) != 0)
			write_error_msg(_("could not create the thread"));
	} else {
		baa_info_msg(_("data: %s is passiv"), i->hostname);
	}
}

void
write_to_textfield(const char *message, int log_level)
{
	GtkTextIter iter;

	if (textfield_buffer != NULL) {
		switch(log_level) {
		case LOG_INFO:
			gtk_text_buffer_get_end_iter(textfield_buffer, &iter);
			gtk_text_buffer_insert(textfield_buffer, &iter, message, -1);
			break;
		case LOG_ERR:
			gtk_text_buffer_get_end_iter(textfield_buffer, &iter);
			gtk_text_buffer_insert_with_tags_by_name(textfield_buffer,
								 &iter,
								 "--- ERROR ---\n",
								 -1, "red", "bold",
								 NULL);
			gtk_text_buffer_insert(textfield_buffer, &iter, message, -1);
			break;
		case LOG_DEBUG:
			gtk_text_buffer_get_end_iter(textfield_buffer, &iter);
			gtk_text_buffer_insert_with_tags_by_name(textfield_buffer,
								 &iter,
								 "--- DEBUG ---\n",
								 -1, "italic",
								 NULL);
			gtk_text_buffer_insert(textfield_buffer, &iter, message, -1);
		default:
			baa_error_msg(_("ERROR: Message type not supported"));
		}
	} else {
		baa_error_msg(_("ERROR: textfield_buffer == NULL!"));
	}
}

static void
build_help_window(void)
{
	/* the help window */
}

static void
build_connect_window(void)
{
	gtk_widget_set_sensitive(search_b, FALSE);

	GtkWidget *connect_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	GtkWidget *array_of_cb[baalue_nodes->num_nodes];

	GtkWidget *vbox_connect = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(connect_window), vbox_connect);
	gtk_container_set_border_width(GTK_CONTAINER(connect_window), 15);
	gtk_window_set_title(GTK_WINDOW(connect_window), "Connect nodes");
	gtk_window_set_default_size(GTK_WINDOW(connect_window), 250, 200);

	int i = 0;
	for (i = 0; i < baalue_nodes->num_nodes; i++) {
		array_of_cb[i] = gtk_check_button_new_with_label(
			baalue_nodes->array_of_nodes[i].hostname);

		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(array_of_cb[i]), FALSE);

		GTK_WIDGET_UNSET_FLAGS(array_of_cb[i], GTK_CAN_FOCUS);
		gtk_box_pack_start(GTK_BOX(vbox_connect), array_of_cb[i], TRUE, TRUE, 5);

		g_signal_connect(array_of_cb[i], "clicked",
				 G_CALLBACK(toggle_connect_window),
				 (gpointer) &baalue_nodes->array_of_nodes[i]);
	}

	gtk_widget_show_all(connect_window);
}

static void
build_halt_window(void)
{
	gtk_widget_set_sensitive(search_b, FALSE);

	GtkWidget *halt_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	GtkWidget *array_of_cb[baalue_nodes->num_nodes];

	GtkWidget *vbox_halt = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(halt_window), vbox_halt);
	gtk_container_set_border_width(GTK_CONTAINER(halt_window), 15);
	gtk_window_set_title(GTK_WINDOW(halt_window), "Halt nodes");
	gtk_window_set_default_size(GTK_WINDOW(halt_window), 250, 200);

	int i = 0;
	for (i = 0; i < baalue_nodes->num_nodes; i++) {
		array_of_cb[i] = gtk_check_button_new_with_label(
			baalue_nodes->array_of_nodes[i].hostname);

		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(array_of_cb[i]), FALSE);

		GTK_WIDGET_UNSET_FLAGS(array_of_cb[i], GTK_CAN_FOCUS);
		gtk_box_pack_start(GTK_BOX(vbox_halt), array_of_cb[i], TRUE, TRUE, 5);

		g_signal_connect(array_of_cb[i], "clicked",
				 G_CALLBACK(toggle_halt_window),
				 (gpointer) &baalue_nodes->array_of_nodes[i]);
	}

	gtk_widget_show_all(halt_window);
}

static void
build_reboot_window(void)
{
	gtk_widget_set_sensitive(search_b, FALSE);

	GtkWidget *reboot_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	GtkWidget *array_of_cb[baalue_nodes->num_nodes];

	GtkWidget *vbox_reboot = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(reboot_window), vbox_reboot);
	gtk_container_set_border_width(GTK_CONTAINER(reboot_window), 15);
	gtk_window_set_title(GTK_WINDOW(reboot_window), "Reboot nodes");
	gtk_window_set_default_size(GTK_WINDOW(reboot_window), 250, 200);

	int i = 0;
	for (i = 0; i < baalue_nodes->num_nodes; i++) {
		array_of_cb[i] = gtk_check_button_new_with_label(
			baalue_nodes->array_of_nodes[i].hostname);

		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(array_of_cb[i]), FALSE);

		GTK_WIDGET_UNSET_FLAGS(array_of_cb[i], GTK_CAN_FOCUS);
		gtk_box_pack_start(GTK_BOX(vbox_reboot), array_of_cb[i], TRUE, TRUE, 5);

		g_signal_connect(array_of_cb[i], "clicked",
				 G_CALLBACK(toggle_reboot_window),
				 (gpointer) &baalue_nodes->array_of_nodes[i]);
	}

	gtk_widget_show_all(reboot_window);
}

static void
help_button(GtkWidget *widget, gpointer data)
{
	(void) widget;
	(void) data;

	build_help_window();
}

static void
search_button(GtkWidget *widget, gpointer data)
{
	(void) widget;
	(void) data;

	/* broadcast to get active nodes */
	if (baalue_nodes != NULL) {
		write_info_msg(_("clear old node config"));
		free_baalue_nodes_instance(baalue_nodes);
	}

	baalue_nodes = get_active_baalue_nodes(); // TODO: search range
	if (baalue_nodes == NULL) {
		write_error_msg(_("baalue_nodes == NULL"));
	} else {
		gtk_widget_set_sensitive(connect_b, TRUE);
		gtk_widget_set_sensitive(halt_b, TRUE);
		gtk_widget_set_sensitive(reboot_b, TRUE);
	}
}

static void
connect_button(GtkWidget *widget, gpointer data)
{
	(void) widget;
	(void) data;

	if (baalue_nodes != NULL) {
		build_connect_window();
	} else {
		write_error_msg(_("baalue_nodes == NULL"));
		gtk_widget_set_sensitive(connect_b, FALSE);
	}
}

static void
about_button(GtkWidget *widget, gpointer data)
{
	(void) widget;
	(void) data;

	/* about dialog -> http://zetcode.com/gui/gtk2/gtkdialogs/*/
}

static void
halt_button(GtkWidget *widget, gpointer data)
{
	(void) widget;
	(void) data;

	if (baalue_nodes != NULL) {
		build_halt_window();
	} else {
		write_error_msg(_("baalue_nodes == NULL"));
		gtk_widget_set_sensitive(halt_b, FALSE);
	}
}

static void
reboot_button(GtkWidget *widget, gpointer data)
{
	(void) widget;
	(void) data;

	if (baalue_nodes != NULL) {
		build_reboot_window();
	} else {
		write_error_msg(_("baalue_nodes == NULL"));
		gtk_widget_set_sensitive(reboot_b, FALSE);
	}
}

static void
build_button_box(void)
{
	gtk_container_set_border_width(GTK_CONTAINER(buttonbox), 5);

	/* ---- search */
	search_b = gtk_button_new_with_label(_("Search for nodes"));
	g_signal_connect(search_b, "clicked", G_CALLBACK(search_button), NULL);
	gtk_tooltips_set_tip(tooltips,
			     search_b,
			     _("Search for baalue cluster nodes"),
			     NULL);
	gtk_container_add(GTK_CONTAINER(buttonbox), search_b);

	/* ---- connect */
	connect_b = gtk_button_new_with_label(_("Connect node(s)"));
	g_signal_connect(connect_b, "clicked", G_CALLBACK(connect_button), NULL);
	gtk_tooltips_set_tip(tooltips,
			     connect_b,
			     _("Connect to a baalue cluster node(s)"),
			     NULL);
	gtk_container_add(GTK_CONTAINER(buttonbox), connect_b);

	/* ---- reboot */
	reboot_b = gtk_button_new_with_label(_("Reboot node(s)"));
	g_signal_connect(reboot_b, "clicked", G_CALLBACK(reboot_button), NULL);
	gtk_tooltips_set_tip(tooltips,
			     reboot_b,
			     _("Reboot (and reconnect) baalue cluster node(s)"),
			     NULL);
	gtk_container_add(GTK_CONTAINER(buttonbox), reboot_b);

	/* ---- halt */
	halt_b = gtk_button_new_with_label(_("Halt node(s)"));
	g_signal_connect(halt_b, "clicked", G_CALLBACK(halt_button), NULL);
	gtk_tooltips_set_tip(tooltips,
			     halt_b,
			     _("Halt baalue cluster node(s)"),
			     NULL);
	gtk_container_add(GTK_CONTAINER(buttonbox), halt_b);

	/* ---- about */
	about_b = gtk_button_new_with_label(_("About"));
	g_signal_connect(about_b, "clicked", G_CALLBACK(about_button), NULL);
	gtk_tooltips_set_tip(tooltips,
			     about_b,
			     _("About"),
			     NULL);
	gtk_container_add(GTK_CONTAINER(buttonbox), about_b);


	/* ---- help */
	help_b = gtk_button_new_with_label(_("Help"));
	g_signal_connect(help_b, "clicked", G_CALLBACK(help_button), NULL);
	gtk_tooltips_set_tip(tooltips,
			     help_b,
			     _("Open help window"),
			     NULL);
	gtk_container_add(GTK_CONTAINER(buttonbox), help_b);

	/* ---- exit */
	exit_b = gtk_button_new_with_label(_("Quit"));
	g_signal_connect(exit_b, "clicked", G_CALLBACK(exit_function), NULL);
	g_signal_connect_swapped(exit_b,
				 "clicked",
				 G_CALLBACK(gtk_widget_destroy),
				 window);
	gtk_tooltips_set_tip(tooltips,
			     exit_b,
			     _("Quit/Exit"),
			     NULL);
	gtk_container_add(GTK_CONTAINER(buttonbox), exit_b);
}

static void
setup_textfield_entry(void)
{
	/*
	 * TODO:
	 * Read value of textfield_entry which should represent some command
	 * to control the sdk and more
	 *
	 * See https://developer.gnome.org/gtk2/stable/GtkEntry.html
	 */

	gtk_entry_set_editable(GTK_ENTRY(textfield_entry), FALSE);
}


static void
setup_textfield(void)
{
	gtk_widget_set_size_request(textfield, 600, 240);

	gtk_text_view_set_editable(GTK_TEXT_VIEW(textfield), FALSE);
	gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(textfield), FALSE);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(textfield), GTK_WRAP_WORD);

	textfield_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textfield));

	gtk_text_buffer_create_tag(textfield_buffer, "bold", "weight",
				   PANGO_WEIGHT_BOLD, NULL);
	gtk_text_buffer_create_tag(textfield_buffer, "italic", "style",
				   PANGO_STYLE_ITALIC, NULL);
	gtk_text_buffer_create_tag(textfield_buffer, "red", "foreground",
				   "red",  NULL);
}


void
build_main_window(void)
{
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	gtk_window_set_title(GTK_WINDOW(window), program_name);

	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_window_set_icon_name(GTK_WINDOW (window), program_name);

	/*
	 * delete-event -> event coming from the window manager
	 */
	g_signal_connect(window, "delete-event",
			 G_CALLBACK(on_delete_event), NULL);
	g_signal_connect(window, "destroy",
			 G_CALLBACK(gtk_main_quit), NULL);


	/*
	   Layout:
	      +-------------------------------+
	      + ------------- vbox -----------+
	      |           |                   |
	      |           |                   |
	      | buttonbox |    textarea       |   <- hbox for buttonbox + area
	      |           |   (see below)     |
	      |           |                   |
	      +-----------+-------------------+
	      |      ..... statusbar ....     |   <- statusbar
	      +-------------------------------+
	*/

        // ------------------ tooltips --------------------
	tooltips = gtk_tooltips_new();


        // ------------------ vbox ------------------------
	vbox = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(window), vbox);

	// ------------------ hbox ------------------------
	hbox = gtk_hbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(vbox), hbox);


	// ------------------ buttonbox -------------------
	buttonbox = gtk_vbutton_box_new();
	build_button_box();
	gtk_box_pack_start(GTK_BOX(hbox), buttonbox, FALSE, FALSE, 1);

	// ------------------ textarea --------------------

        /*
	   Layout:
	      + -------- table --------+
	      |                      | |
	      |        texfield      | |
	      |                      | |
	      +----------------------+-+
	      |        textentry       |
	      +------------------------+
	*/

	textfield_table = gtk_table_new(2, 1, FALSE);
	textfield_scrolled = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(textfield_scrolled),
		GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);

	textfield = gtk_text_view_new();
	setup_textfield();

	// entry line below the textfield
	textfield_entry = gtk_entry_new();
	setup_textfield_entry();

	gtk_container_add(GTK_CONTAINER(textfield_scrolled), textfield);
	gtk_table_attach_defaults(GTK_TABLE(textfield_table),
				  textfield_scrolled, 0, 1, 0, 1);
	gtk_table_attach_defaults(GTK_TABLE(textfield_table),
				  textfield_entry, 0, 1, 1, 2);

	gtk_box_pack_end(GTK_BOX(hbox), textfield_table, TRUE, TRUE, 1);

	// ------------------ statusbar -------------------
	statusbar = gtk_statusbar_new();
	gtk_box_pack_start(GTK_BOX(vbox), statusbar, FALSE, TRUE, 1);

	gtk_widget_show_all(window);



	gtk_widget_show_all(window);
}
