#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include "pieces.h"

extern const guint8 pieces[];
static void destroy_fn(unsigned char* pixels, gpointer data);

int main(int argc, char** argv)
{
	int i;
	unsigned char *data = NULL;
    GtkWidget* window;

    gtk_init(&argc, &argv);
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_signal_connect(G_OBJECT(window), "destroy",
                        G_CALLBACK(gtk_main_quit), NULL);
	gtk_window_set_title(GTK_WINDOW(window), "Using Pixbuf");
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_container_set_border_width(GTK_CONTAINER(window), 50);

#if 1
	data = (unsigned char*)malloc(3 * 10000);
    printf("data的地址\t == %p\n", data);
    for(i=0; i<30000; i++)
    {
        data[i] = (i/300)%100 + 'A';
    }
    GdkPixbuf* pixbuf = gdk_pixbuf_new_from_data(data, GDK_COLORSPACE_RGB,
                		    		0,
		                		    8,
				                    100, 100, 300,
        				            destroy_fn,NULL);
#else
    GdkPixbuf* pixbuf;
    pixbuf = gdk_pixbuf_new_from_data(pieces+24, GDK_COLORSPACE_RGB,
                    0,
                    8,
                    102,68, 306,
                    destroy_fn,NULL);
#endif

	GtkWidget *image = gtk_image_new_from_pixbuf(pixbuf);
	gtk_container_add(GTK_CONTAINER(window), image);

    gtk_widget_show_all(window);
    gtk_main();

	return 0;
}

static void destroy_fn(unsigned char* pixels, gpointer data)
{
	free(data);
    printf("pixels 的地址\t == %p\n", pixels);
    printf("%s\n", pixels);
}

