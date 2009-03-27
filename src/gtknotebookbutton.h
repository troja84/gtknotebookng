#ifndef _GTK_NOTEBOOK_BUTTON
#define _GTK_NOTEBOOK_BUTTON

#include <glib-object.h>
#include <gtk/gtk.h>

#include "gtknotebookng.h"

G_BEGIN_DECLS

#define GTK_TYPE_NOTEBOOK_BUTTON gtk_notebook_button_get_type()

#define GTK_NOTEBOOK_BUTTON(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_TYPE_NOTEBOOK_BUTTON, GtkNotebookButton))

#define GTK_NOTEBOOK_BUTTON_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), GTK_TYPE_NOTEBOOK_BUTTON, GtkNotebookButtonClass))

#define GTK_IS_NOTEBOOK_BUTTON(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_TYPE_NOTEBOOK_BUTTON))

#define GTK_IS_NOTEBOOK_BUTTON_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_TYPE_NOTEBOOK_BUTTON))

#define GTK_NOTEBOOK_BUTTON_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_TYPE_NOTEBOOK_BUTTON, GtkNotebookButtonClass))

typedef struct {
  GtkButton parent;
} GtkNotebookButton;

typedef struct {
  GtkButtonClass parent_class;
} GtkNotebookButtonClass;

GType gtk_notebook_button_get_type (void);

GtkNotebookButton*      gtk_notebook_button_new     (GtkDirectionType direction,
                                                     GtkNotebookNg    *notebook);

G_END_DECLS

#endif /* _GTK_NOTEBOOK_BUTTON */
