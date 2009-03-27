#include "gtknotebookbutton.h"

G_DEFINE_TYPE (GtkNotebookButton, gtk_notebook_button, GTK_TYPE_BUTTON)

#define GTK_NOTEBOOK_BUTTON_GET_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), GTK_TYPE_NOTEBOOK_BUTTON, GtkNotebookButtonPrivate))

/* DnD */
enum {
  TARGET_INT32
};

/* datatype (string), restrictions on DnD (GtkTargetFlags), datatype (int) */
static GtkTargetEntry target_list[] = {
  { "INTEGER",    0, TARGET_INT32 }
};

static guint n_targets = G_N_ELEMENTS (target_list);


typedef struct _GtkNotebookButtonPrivate GtkNotebookButtonPrivate;

struct _GtkNotebookButtonPrivate {
  GtkNotebookNg *notebook;  

  GtkDirectionType direction;

  guint loop_id;
};

/* DnD functions */
static gboolean   gtk_notebook_button_move            (gpointer data);
static gboolean   gtk_notebook_button_drag_motion_cb  (GtkWidget      *widget, 
                                                       GdkDragContext *context, 
                                                       gint           x, 
                                                       gint           y, 
                                                       guint          t,
                                                       gpointer       user_data);
static gboolean   gtk_notebook_button_drag_leave_cb   (GtkWidget      *widget, 
                                                       GdkDragContext *context, 
                                                       guint          time, 
                                                       gpointer       user_data);
static gboolean   gtk_notebook_button_drag_drop_cb    (GtkWidget      *widget, 
                                                       GdkDragContext *context, 
                                                       gint           x, 
                                                       gint           y, 
                                                       guint          time,
                                                       gpointer       user_data);


static void
gtk_notebook_button_class_init (GtkNotebookButtonClass *klass)
{
  //GObjectClass *object_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (GtkNotebookButtonPrivate));
}

static void
gtk_notebook_button_init (GtkNotebookButton *self)
{
}

GtkNotebookButton*
gtk_notebook_button_new (GtkDirectionType direction,
                         GtkNotebookNg    *notebook)
{
  GtkNotebookButton *nb_button;
  GtkNotebookButtonPrivate *priv;
  GtkWidget *img;
  gchar *stock_id;

  if (!notebook)
    g_warning ("You haven't past a GtkNotebook, GtkNotebookButton will "
               "not work as intended!");

  switch (direction)
    {
      case GTK_DIR_UP:
        stock_id = "gtk-go-up";
        break;
      case GTK_DIR_DOWN:
        stock_id = "gtk-go-down";
        break;
      case GTK_DIR_LEFT:
        stock_id = "gtk-go-back";
        break;
      case GTK_DIR_RIGHT:
        stock_id = "gtk-go-forward";
        break;
      default:
        g_warning ("GTK_DIR_TAB_FORWARD and GTK_DIR_TAB_BACKWARD are not "
                   "supported by GtkNotebookButton!");
        
        return NULL;
    }

  nb_button = g_object_new (GTK_TYPE_NOTEBOOK_BUTTON, NULL);

  priv = GTK_NOTEBOOK_BUTTON_GET_PRIVATE (nb_button);

  priv->notebook = notebook;
  priv->direction = direction;
  priv->loop_id = 0;

  img = gtk_image_new_from_stock (stock_id,
                                  GTK_ICON_SIZE_SMALL_TOOLBAR);

  gtk_button_set_image (GTK_BUTTON (nb_button), img);

  /* Set the button up as a DnD destination */
  gtk_drag_dest_set (GTK_WIDGET (nb_button),
                     GTK_DEST_DEFAULT_HIGHLIGHT,
                     target_list,
                     n_targets,
                     GDK_ACTION_DEFAULT);

  /* connect the signals */
  g_signal_connect (G_OBJECT (nb_button), "drag-motion",
                    G_CALLBACK (gtk_notebook_button_drag_motion_cb), NULL);
  g_signal_connect (G_OBJECT (nb_button), "drag-leave",
                    G_CALLBACK (gtk_notebook_button_drag_leave_cb), NULL);
  g_signal_connect (G_OBJECT (nb_button), "drag-drop",
                    G_CALLBACK (gtk_notebook_button_drag_drop_cb), NULL);

  return nb_button;
}

/* DnD functions */
static gboolean
gtk_notebook_button_move (gpointer data)
{
  GtkNotebookButton *nb_button;
  GtkNotebookButtonPrivate *priv;

  g_return_val_if_fail (GTK_IS_NOTEBOOK_BUTTON (data), FALSE);

  nb_button = GTK_NOTEBOOK_BUTTON (data);
  priv = GTK_NOTEBOOK_BUTTON_GET_PRIVATE (nb_button);

  if (GTK_WIDGET_SENSITIVE (GTK_WIDGET (data)))
    {
      gtk_button_clicked (GTK_BUTTON (data));
      return TRUE;
    }

  return FALSE;
}

/* Emitted when a drag is over the destination */
static gboolean
gtk_notebook_button_drag_motion_cb (GtkWidget      *widget, 
                                    GdkDragContext *context, 
                                    gint           x, 
                                    gint           y, 
                                    guint          t,
                                    gpointer       user_data)
{
  GtkNotebookButton *nb_button;
  GtkNotebookButtonPrivate *priv;

  g_return_val_if_fail (GTK_IS_NOTEBOOK_BUTTON (widget), FALSE);

  nb_button = GTK_NOTEBOOK_BUTTON (widget);
  priv = GTK_NOTEBOOK_BUTTON_GET_PRIVATE (nb_button);

  if (!priv->loop_id)
    priv->loop_id = gdk_threads_add_timeout (500,
                                             (GSourceFunc)gtk_notebook_button_move,
                                             (gpointer)widget);

  return  TRUE;
}

static void
gtk_notebook_button_remove_move_cb (GtkWidget *widget)
{
  GtkNotebookButton *nb_button;
  GtkNotebookButtonPrivate *priv;

  g_return_if_fail (GTK_IS_NOTEBOOK_BUTTON (widget));

  nb_button = GTK_NOTEBOOK_BUTTON (widget);
  priv = GTK_NOTEBOOK_BUTTON_GET_PRIVATE (nb_button);

  if (priv->loop_id)
    {
      g_source_remove (priv->loop_id);
      priv->loop_id = 0;
    }
}

/* Emitted when a drag leaves the destination */
static gboolean
gtk_notebook_button_drag_leave_cb (GtkWidget      *widget, 
                                   GdkDragContext *context, 
                                   guint          time, 
                                   gpointer       user_data)
{
  gtk_notebook_button_remove_move_cb (widget);

  return TRUE;
}

/* Emitted when the user releases (drops) the selection. It should check that
 * the drop is over a valid part of the widget (if its a complex widget), and
 * itself to return true if the operation should continue. Next choose the
 * target type it wishes to ask the source for. Finally call gtk_drag_get_data
 * which will emit "drag-data-get" on the source. */
static gboolean
gtk_notebook_button_drag_drop_cb (GtkWidget      *widget, 
                                  GdkDragContext *context, 
                                  gint           x, 
                                  gint           y, 
                                  guint          time,
                                  gpointer       user_data)
{
  gtk_notebook_button_remove_move_cb (widget);

  return FALSE;
}

