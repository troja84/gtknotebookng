#include <gtk/gtk.h>

#include "gtknotebookng.h"

int main (int argc, char *argv[])
{
  GtkWidget *window, *bbox, *button, *image;
  GtkNotebookNg *notebook_ng;

  gtk_init (&argc, &argv);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

  notebook_ng = gtk_notebook_ng_new ();
  gtk_container_add (GTK_CONTAINER (window), GTK_WIDGET (notebook_ng));

  /* It's a good idea to do this for all windows. */
  g_signal_connect (G_OBJECT (window), "destroy",
                    G_CALLBACK (gtk_main_quit), NULL);

  g_signal_connect (G_OBJECT (window), "delete_event",
    		    G_CALLBACK (gtk_main_quit), NULL);

  /* Add widgets to the notebook */
  bbox = gtk_hbutton_box_new (); 
  button = gtk_button_new_from_stock (GTK_STOCK_APPLY);
  gtk_container_add (GTK_CONTAINER (bbox), button);
  gtk_container_add (GTK_CONTAINER (notebook_ng), bbox);

  image = NULL;
  image = gtk_image_new_from_file ("./tests/img/test_img1.jpg");
  gtk_container_add (GTK_CONTAINER (notebook_ng), image);
  image = gtk_image_new_from_file ("./tests/img/test_img2.jpg");
  gtk_container_add (GTK_CONTAINER (notebook_ng), image);
  image = gtk_image_new_from_file ("./tests/img/test_img3.jpg");
  gtk_container_add (GTK_CONTAINER (notebook_ng), image);
  image = gtk_image_new_from_file ("./tests/img/test_img4.jpg");
  gtk_container_add (GTK_CONTAINER (notebook_ng), image);

  gtk_widget_show_all (window);

  gtk_main ();

  return 0;
}
