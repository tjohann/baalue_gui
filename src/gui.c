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
static GtkWidget *disconnect_b;
static GtkWidget *reboot_b;
static GtkWidget *halt_b;
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

void
write_to_textfield(const char *message, int log_level)
{
	GtkTextIter iter;

	if (textfield_buffer != NULL) {
		switch(log_level) {
		case LOG_INFO:
			gdk_threads_enter();
			gtk_text_buffer_get_end_iter(textfield_buffer, &iter);
			gtk_text_buffer_insert(textfield_buffer, &iter, message, -1);
			gdk_threads_leave();
		case LOG_ERR:
			gdk_threads_enter();
			gtk_text_buffer_get_end_iter(textfield_buffer, &iter);
			gtk_text_buffer_insert_with_tags_by_name(textfield_buffer,
								 &iter,
								 "--- ERROR ---\n",
								 -1, "red", "bold",
								 NULL);
			gtk_text_buffer_insert(textfield_buffer, &iter, message, -1);
			gdk_threads_leave();
			break;
		case LOG_DEBUG:
			gdk_threads_enter();
			gtk_text_buffer_get_end_iter(textfield_buffer, &iter);
			gtk_text_buffer_insert_with_tags_by_name(textfield_buffer,
								 &iter,
								 "--- DEBUG ---\n",
								 -1, "italic",
								 NULL);
			gtk_text_buffer_insert(textfield_buffer, &iter, message, -1);
			gdk_threads_leave();
		default:
			baa_error_msg(_("ERROR: Message type not supported"));
		}
	} else {
		baa_error_msg(_("ERROR: textfield_buffer == NULL!"));
	}
}


static void
help_button(GtkWidget *widget, gpointer data)
{
	PRINT_LOCATION();

	(void) widget;
	(void) data;

	if (!g_thread_create(&help, NULL, FALSE, NULL) != 0)
		write_error_msg(_("Can't create the thread"));
}

static void
build_button_box(void)
{
	gtk_container_set_border_width(GTK_CONTAINER(buttonbox), 5);

	/* ---- search */
	search_b = gtk_button_new_with_label(_("Search for node"));
//	g_signal_connect(search_b, "clicked", G_CALLBACK(search_node), NULL);
	gtk_tooltips_set_tip(tooltips,
			     search_b,
			     _("Search for baalue cluster nodes"),
			     NULL);
	gtk_container_add(GTK_CONTAINER(buttonbox), search_b);

	/* ---- connect */
	connect_b = gtk_button_new_with_label(_("Connect to node"));
//	g_signal_connect(connect_b, "clicked", G_CALLBACK(clone_button), NULL);
	gtk_tooltips_set_tip(tooltips,
			     connect_b,
			     _("Connect to a baalue cluster node"),
			     NULL);
	gtk_container_add(GTK_CONTAINER(buttonbox), connect_b);

	/* ---- disconnect */
	disconnect_b = gtk_button_new_with_label(_("Disconnect node"));
//	g_signal_connect(disconnect_b, "clicked", G_CALLBACK(clone_button), NULL);
	gtk_tooltips_set_tip(tooltips,
			     disconnect_b,
			     _("Disconnect from a baalue cluster node"),
			     NULL);
	gtk_container_add(GTK_CONTAINER(buttonbox), disconnect_b);

	/* ---- reboot */
	reboot_b = gtk_button_new_with_label(_("Reboot node"));
//	g_signal_connect(reboot_b, "clicked", G_CALLBACK(update_button), NULL);
	gtk_tooltips_set_tip(tooltips,
			     reboot_b,
			     _("Reboot (and reconnect) baalue cluster node"),
			     NULL);
	gtk_container_add(GTK_CONTAINER(buttonbox), reboot_b);

	/* ---- halt */
	halt_b = gtk_button_new_with_label(_("Shutdown node"));
//	g_signal_connect(halt_b, "clicked", G_CALLBACK(download_button), NULL);
	gtk_tooltips_set_tip(tooltips,
			     halt_b,
			     _("Shutdown baalue cluster node"),
			     NULL);
	gtk_container_add(GTK_CONTAINER(buttonbox), halt_b);

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
