/*
* wnd_gtk.c
*
*/

#include "gtk/gtk.h"
#include "gdk/gdkkeysyms.h"
#include <stdio.h>
#include "types.h"

void mvShowImage(const char *name, image_t *image)
{
	int i;
	unsigned char *data = NULL;
	mat_t *mat = (mat_t*)(image->Data);
	GtkWidget* window;

	gtk_init(&argc, &argv);
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	g_signal_connect(G_OBJECT(window), "destroy",
	G_CALLBACK(gtk_main_quit), NULL);
	gtk_window_set_title(GTK_WINDOW(window), name);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_container_set_border_width(GTK_CONTAINER(window), 50);

	GdkPixbuf* pixbuf = gdk_pixbuf_new_from_data(mat->data.ptr, GDK_COLORSPACE_RGB,
									0,
								    8,
								    mat->width, mat->height, mat->step,
								    NULL, NULL);

	GtkWidget *img = gtk_image_new_from_pixbuf(pixbuf);
	gtk_container_add(GTK_CONTAINER(window), img);

	gtk_widget_show_all(window);
	gtk_main();

	return 0;
}

