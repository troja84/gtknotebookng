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

GtkNotebookNg*  gtk_notebook_ng_new           (void);

/* Notebook API */
void            gtk_notebook_ng_remove_page       (GtkNotebookNg   *notebook,
                                                   gint             position);
gint            gtk_notebook_ng_page_num          (GtkNotebookNg   *notebook,
                                                   GtkWidget       *child);
void            gtk_notebook_ng_next_page         (GtkNotebookNg   *notebook);
void            gtk_notebook_ng_prev_page         (GtkNotebookNg   *notebook);
void            gtk_notebook_ng_reorder_child     (GtkNotebookNg   *notebook,
                                                   GtkWidget       *child,
                                                   gint             position);

void            gtk_notebook_ng_set_tab_pos       (GtkNotebookNg   *notebook,
                                                   GtkPositionType  pos);
GtkPositionType gtk_notebook_ng_get_tab_pos       (GtkNotebookNg   *notebook);

void            gtk_notebook_ng_set_show_tabs     (GtkNotebookNg   *notebook,
                                                   gboolean         show_tabs);
gboolean        gtk_notebook_ng_get_show_tabs     (GtkNotebookNg   *notebook);

void            gtk_notebook_ng_set_show_border   (GtkNotebookNg   *notebook,
                                                   gboolean         show_border);
gboolean        gtk_notebook_ng_get_show_border   (GtkNotebookNg   *notebook);

void            gtk_notebook_ng_set_current_page  (GtkNotebookNg   *notebook,
                                                   gint             page_num);
gint            gtk_notebook_ng_get_current_page  (GtkNotebookNg   *notebook);

GtkWidget*      gtk_notebook_ng_get_nth_page      (GtkNotebookNg   *notebook,
                                                   gint             page_num);
gint            gtk_notebook_ng_get_n_pages       (GtkNotebookNg   *notebook);


G_END_DECLS

#endif /* _GTK_NOTEBOOK_NG */
