#ifndef _GTK_NOTEBOOK_NG
#define _GTK_NOTEBOOK_NG

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GTK_TYPE_NOTEBOOK_NG gtk_notebook_ng_get_type()

#define GTK_NOTEBOOK_NG(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_TYPE_NOTEBOOK_NG, GtkNotebookNg))

#define GTK_NOTEBOOK_NG_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), GTK_TYPE_NOTEBOOK_NG, GtkNotebookNgClass))

#define GTK_IS_NOTEBOOK_NG(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_TYPE_NOTEBOOK_NG))

#define GTK_IS_NOTEBOOK_NG_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_TYPE_NOTEBOOK_NG))

#define GTK_NOTEBOOK_NG_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_TYPE_NOTEBOOK_NG, GtkNotebookNgClass))

typedef struct {
  GtkNotebook parent;
} GtkNotebookNg;

typedef struct {
  GtkNotebookClass parent_class;
} GtkNotebookNgClass;

GType gtk_notebook_ng_get_type (void);

GtkNotebookNg* gtk_notebook_ng_new (void);

G_END_DECLS

#endif /* _GTK_NOTEBOOK_NG */
