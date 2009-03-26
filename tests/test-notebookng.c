#include <gtk/gtk.h>
#include <glib/gprintf.h>

#include "gtknotebookng.h"

static gboolean
notebook_add_signal_cb (GtkNotebookNg *notebook,
                        GtkWidget     *widget,
                        gpointer       data)
{
  GtkLabel *label;
  gchar *res;
  gint n = 0;

  g_return_val_if_fail (GTK_IS_LABEL (data), FALSE);

  label = GTK_LABEL (data);

  n = gtk_notebook_ng_get_n_pages (notebook);

  res = g_strdup_printf ("Number of tabs: %d", n);

  gtk_label_set_text (label, res);

  g_free (res);

  return FALSE;
}

static gboolean
notebook_next_cb (GtkButton *button,
                  gpointer   data)
{
  GtkNotebookNg *notebook;

  g_return_val_if_fail (GTK_IS_NOTEBOOK_NG (data), FALSE);

  notebook = GTK_NOTEBOOK_NG (data);

  gtk_notebook_ng_next_page (notebook);

  return FALSE;
}

static gboolean
notebook_prev_cb (GtkButton *button,
                  gpointer   data)
{
  GtkNotebookNg *notebook;

  g_return_val_if_fail (GTK_IS_NOTEBOOK_NG (data), FALSE);

  notebook = GTK_NOTEBOOK_NG (data);

  gtk_notebook_ng_prev_page (notebook);

  return FALSE;
}

static gboolean
notebook_reorder_cb (GtkButton *button,
                     gpointer   data)
{
  GtkNotebookNg *notebook;
  GRand *rand;
  GtkWidget *child;
  gint old_pos, new_pos, range;

  g_return_val_if_fail (GTK_IS_NOTEBOOK_NG (data), FALSE);

  notebook = GTK_NOTEBOOK_NG (data);

  rand = g_rand_new ();

  range = gtk_notebook_ng_get_n_pages (notebook);

  old_pos = g_rand_int_range (rand, 0, range);
  new_pos = g_rand_int_range (rand, 0, range);

  child = gtk_notebook_ng_get_nth_page (notebook, old_pos);

  //g_printf (">> REORDER OLD: %d NEW: %d\n", old_pos, new_pos);
  gtk_notebook_ng_reorder_child (notebook, child, new_pos);

  return FALSE;
}

int main (int argc, char *argv[])
{
  GtkWidget *window, *box, *vbox, *bbox, *button, *image, *label;
  GtkNotebookNg *notebook_ng;

  gtk_init (&argc, &argv);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

  /* It's a good idea to do this for all windows. */
  g_signal_connect (G_OBJECT (window), "destroy",
                    G_CALLBACK (gtk_main_quit), NULL);

  g_signal_connect (G_OBJECT (window), "delete_event",
    		    G_CALLBACK (gtk_main_quit), NULL);

  box = gtk_hbox_new (FALSE, 0);
  vbox = gtk_vbox_new (FALSE, 0);
  label = gtk_label_new ("Number of tabs: 0");
  notebook_ng = gtk_notebook_ng_new ();
  g_signal_connect (G_OBJECT (notebook_ng), "add",
                    G_CALLBACK (notebook_add_signal_cb), label);
  gtk_container_add (GTK_CONTAINER (vbox), label);

  gtk_container_add (GTK_CONTAINER (box), GTK_WIDGET (notebook_ng));
  gtk_container_add (GTK_CONTAINER (window), box);

  /* Add widgets for the notebook */
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

  bbox = gtk_vbutton_box_new ();
  gtk_container_add (GTK_CONTAINER (vbox), bbox);
  gtk_container_add (GTK_CONTAINER (box), vbox);


  button = gtk_button_new_from_stock (GTK_STOCK_GO_FORWARD);
  g_signal_connect (G_OBJECT (button), "clicked",
                    G_CALLBACK (notebook_next_cb), (gpointer)notebook_ng);
  gtk_container_add (GTK_CONTAINER (bbox), button);
  button = gtk_button_new_from_stock (GTK_STOCK_GO_BACK);
  g_signal_connect (G_OBJECT (button), "clicked",
                    G_CALLBACK (notebook_prev_cb), (gpointer)notebook_ng);
  gtk_container_add (GTK_CONTAINER (bbox), button);
  button = gtk_button_new_from_stock (GTK_STOCK_REFRESH);
  g_signal_connect (G_OBJECT (button), "clicked",
                    G_CALLBACK (notebook_reorder_cb), (gpointer)notebook_ng);
  gtk_container_add (GTK_CONTAINER (bbox), button);

  gtk_widget_show_all (window);

  gtk_main ();

  return 0;
}
