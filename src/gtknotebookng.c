#include <glib/gprintf.h>

#include "gtknotebookng.h"

typedef struct _GtkNotebookNgPrivate GtkNotebookNgPrivate;

struct _GtkNotebookNgPrivate {
  /* data structures */
  GQueue *tabs; /* A queue of buttons representing the order*/
  GHashTable *tabs_to_widgets; /* Connection between tabs and widgets */
  GHashTable *widgets_to_tabs; /* Connection between widgets and tabs*/
  GtkWidget *active; /* The currently shown child */
  GtkWidget *first_tab;
  GtkWidget *last_tab;

  /* scroll buttons */
  GtkWidget *previous;
  GtkWidget *next;
  guint offset;

  /* configuration */
  gboolean show_tabs;
  gboolean homogeneous;
  gboolean show_border;
  GtkPositionType position;

  /* allocation */
  GtkAllocation child_allocation;
};

/* GObject functions */
static void     gtk_notebook_ng_get_property       (GObject *object, 
                                                    guint property_id,
                                                    GValue *value,
                                                    GParamSpec *pspec);
static void     gtk_notebook_ng_set_property       (GObject *object,
                                                    guint property_id,
                                                    const GValue *value,
                                                    GParamSpec *pspec);

/* GtkWidget functions */
static void     gtk_notebook_ng_map                (GtkWidget        *widget);
static void     gtk_notebook_ng_size_request       (GtkWidget        *widget,
					                                          GtkRequisition   *requisition);
static void     gtk_notebook_ng_size_allocate      (GtkWidget        *widget,
					                                          GtkAllocation    *allocation);
static gint     gtk_notebook_ng_expose             (GtkWidget        *widget,
					                                          GdkEventExpose   *event);

/* GtkContainer functions */
static void     gtk_notebook_ng_add                 (GtkContainer     *container,
					                                           GtkWidget        *widget);
static void     gtk_notebook_ng_remove              (GtkContainer     *container,
					                                           GtkWidget        *widget);
static void     gtk_notebook_ng_forall              (GtkContainer     *container,
					                                           gboolean		       include_internals,
					                                           GtkCallback       callback,
					                                           gpointer          callback_data);

/* PRIVATE: GtkNotebookNg convenience functions */
static void     gtk_notebook_ng_select_widget      (GtkNotebookNg *notebook,
                                                    GtkWidget     *widget);

/* PRIVATE: GtkNotebookNg and notebook tabs connection */
static void     gtk_notebook_ng_select_cb          (GtkButton     *button,
                                                    GtkNotebookNg *notebook);
static void     gtk_notebook_ng_previous_cb        (GtkButton     *button,
                                                    GtkNotebookNg *notebook);
static void     gtk_notebook_ng_next_cb            (GtkButton     *button,
                                                    GtkNotebookNg *notebook);

G_DEFINE_TYPE (GtkNotebookNg, gtk_notebook_ng, GTK_TYPE_NOTEBOOK)

#define GTK_NOTEBOOK_NG_GET_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), GTK_TYPE_NOTEBOOK_NG, GtkNotebookNgPrivate))

static void
gtk_notebook_ng_class_init (GtkNotebookNgClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);
  GtkContainerClass *container_class = GTK_CONTAINER_CLASS (klass);

  g_type_class_add_private (klass, sizeof (GtkNotebookNgPrivate));

  object_class->get_property = gtk_notebook_ng_get_property;
  object_class->set_property = gtk_notebook_ng_set_property;

  widget_class->map = gtk_notebook_ng_map;
  widget_class->size_request = gtk_notebook_ng_size_request;
  widget_class->size_allocate = gtk_notebook_ng_size_allocate;
  widget_class->expose_event = gtk_notebook_ng_expose;

  container_class->add = gtk_notebook_ng_add;
  container_class->remove = gtk_notebook_ng_remove;
  container_class->forall = gtk_notebook_ng_forall;
}

static void
gtk_notebook_ng_init (GtkNotebookNg *self)
{
  GtkNotebookNgPrivate *priv;

  priv = GTK_NOTEBOOK_NG_GET_PRIVATE (self);

  /* data structures */
  priv->tabs = g_queue_new ();
  priv->tabs_to_widgets = g_hash_table_new (g_direct_hash,
                                            g_direct_equal);
  priv->widgets_to_tabs = g_hash_table_new (g_direct_hash,
                                            g_direct_equal);

  priv->active = NULL;

  /* scroll buttons */
  GtkWidget *prev_img = gtk_image_new_from_stock ("gtk-go-back",
                                                  GTK_ICON_SIZE_BUTTON);
  GtkWidget *next_img = gtk_image_new_from_stock ("gtk-go-forward",
                                                  GTK_ICON_SIZE_BUTTON);

  priv->previous = gtk_button_new ();
  g_signal_connect (G_OBJECT (priv->previous), "clicked",
                    G_CALLBACK (gtk_notebook_ng_previous_cb), self);
  gtk_button_set_image (GTK_BUTTON (priv->previous), prev_img);

  priv->next = gtk_button_new ();
  g_signal_connect (G_OBJECT (priv->next), "clicked",
                    G_CALLBACK (gtk_notebook_ng_next_cb), self);
  gtk_button_set_image (GTK_BUTTON (priv->next), next_img);

  gtk_widget_set_parent (priv->previous, GTK_WIDGET (self));
  gtk_widget_set_parent (priv->next, GTK_WIDGET (self));

  gtk_widget_show_all (priv->previous);
  gtk_widget_show_all (priv->next);

  priv->offset = 0;

  /* configuration */
  priv->show_tabs = TRUE;
  priv->homogeneous = FALSE;
  priv->show_border = TRUE;
  priv->position = GTK_POS_TOP;
}

GtkNotebookNg*
gtk_notebook_ng_new (void)
{
  return g_object_new (GTK_TYPE_NOTEBOOK_NG, NULL);
}

static void
gtk_notebook_ng_get_property (GObject *object, guint property_id,
                              GValue *value, GParamSpec *pspec)
{
  switch (property_id) {
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
  }
}

static void
gtk_notebook_ng_set_property (GObject *object, guint property_id,
                              const GValue *value, GParamSpec *pspec)
{
  switch (property_id) {
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
  }
}

/* GtkWidget functions */
static void
gtk_notebook_ng_map (GtkWidget *widget)
{
  GtkNotebookNg *notebook;
  GtkNotebookNgPrivate *priv;
  GList *tabs;
  GtkWidget *tab;

  g_return_if_fail (GTK_IS_NOTEBOOK_NG (widget));

  notebook = GTK_NOTEBOOK_NG (widget);
  priv = GTK_NOTEBOOK_NG_GET_PRIVATE (notebook);

  GTK_WIDGET_SET_FLAGS (widget, GTK_MAPPED);

  tabs = g_queue_peek_head_link (priv->tabs);

  while (tabs)
    {
      tab = GTK_WIDGET (tabs->data);

      if (tab && GTK_WIDGET_VISIBLE (tab)
          && !GTK_WIDGET_DRAWABLE (tab))
        gtk_widget_map (tab);

      tabs = tabs->next;
    }

  if (priv->previous && GTK_WIDGET_VISIBLE (priv->previous)
      && !GTK_WIDGET_DRAWABLE (priv->previous))
    gtk_widget_map (priv->previous);
  
  if (priv->next && GTK_WIDGET_VISIBLE (priv->next)
      && !GTK_WIDGET_DRAWABLE (priv->next))
    gtk_widget_map (priv->next);

  if (priv->active && GTK_WIDGET_VISIBLE (priv->active)
      && !GTK_WIDGET_DRAWABLE (priv->active))
      gtk_widget_map (priv->active);
}

/* - START - PRIVATE to gtk_notebook_ng_size_request */
static void
gtk_notebook_ng_size_request_children (GtkWidget *widget,
                                       GtkRequisition *requisition)
{
  GtkNotebookNg *notebook;
  GtkNotebookNgPrivate *priv;
  GList *children;
  GtkWidget *child_widget;
  GtkRequisition child_requisition;

  g_return_if_fail (GTK_IS_NOTEBOOK_NG (widget));

  notebook = GTK_NOTEBOOK_NG (widget);
  priv = GTK_NOTEBOOK_NG_GET_PRIVATE (notebook);

  children = g_hash_table_get_values (priv->tabs_to_widgets);

  while (children)
    {
      child_widget = GTK_WIDGET (children->data);

      if (GTK_WIDGET_VISIBLE (child_widget))
        {
          gtk_widget_size_request (child_widget, &child_requisition);

          requisition->width = MAX (requisition->width,
                                    child_requisition.width);
          requisition->height = MAX (requisition->height,
                                     child_requisition.height);

          /*
          g_printf ("> REQUEST CHILD NEW: W: %d H: %d\n",
                    requisition->width, requisition->height);
          */
        }

      children = children->next;
    }
}

static void
gtk_notebook_ng_size_request_tabs (GtkWidget *widget,
                                   GtkRequisition *requisition)
{
  GtkNotebookNg *notebook;
  GtkNotebookNgPrivate *priv;
  GList *children;
  GtkWidget *child_widget;
  GtkRequisition child_requisition, tabs_requisition;
  GtkRequisition max_requisition, offset_requisition;
  gint /*max_size,*/ counter;
  gint max_height = 0;

  g_return_if_fail (GTK_IS_NOTEBOOK_NG (widget));

  notebook = GTK_NOTEBOOK_NG (widget);
  priv = GTK_NOTEBOOK_NG_GET_PRIVATE (notebook);

  tabs_requisition.width = 0;
  tabs_requisition.height = 0;

  max_requisition.width = 0;
  max_requisition.height = 0;

  offset_requisition.width = 0;
  offset_requisition.height = 0;

  children = g_queue_peek_head_link (priv->tabs);
  counter = 0;

  while (children)
    {
      child_widget = GTK_WIDGET (children->data);

      if (!GTK_WIDGET_VISIBLE (child_widget))
        gtk_widget_show_all (child_widget);

      gtk_widget_size_request (child_widget, &child_requisition);

      tabs_requisition.width += child_requisition.width;
      tabs_requisition.height += child_requisition.height;

      max_requisition.width = MAX (max_requisition.width,
                                    child_requisition.width);
      max_requisition.height = MAX (max_requisition.height,
                                     child_requisition.height);

      if (counter >= priv->offset)
        {
          offset_requisition.width += child_requisition.width;
          offset_requisition.height += child_requisition.height;
        }

      counter++;

      children = children->next;
    }

  switch (priv->position)
    {
      case GTK_POS_TOP:
      case GTK_POS_BOTTOM:
        requisition->height += max_requisition.height;

        if (GTK_WIDGET_DRAWABLE (priv->previous))
          {
            gtk_widget_size_request (priv->previous, &child_requisition);

            max_height = child_requisition.height;
          }

        if (GTK_WIDGET_DRAWABLE (priv->next))
          {
            gtk_widget_size_request (priv->next, &child_requisition);

            max_height = MAX (max_height, child_requisition.height);
          }

        if (max_height > max_requisition.height)
            requisition->height += max_height - max_requisition.height;

        break;
      case GTK_POS_LEFT:
      case GTK_POS_RIGHT:
        requisition->width += max_requisition.width;

        /* TODO */

        break;
    }
}
/* - END - */

static void
gtk_notebook_ng_size_request (GtkWidget      *widget,
					                    GtkRequisition *requisition)
{
  GtkNotebookNg *notebook;
  GtkNotebookNgPrivate *priv;

  g_return_if_fail (GTK_IS_NOTEBOOK_NG (widget));

  notebook = GTK_NOTEBOOK_NG (widget);
  priv = GTK_NOTEBOOK_NG_GET_PRIVATE (notebook);

  widget->requisition.width = widget->requisition.height = 0;

  gtk_notebook_ng_size_request_children (widget, &(widget->requisition));

  if (priv->show_tabs)
    gtk_notebook_ng_size_request_tabs (widget, &(widget->requisition));

  widget->requisition.width += GTK_CONTAINER (widget)->border_width * 2;
  widget->requisition.height += GTK_CONTAINER (widget)->border_width * 2;

  /*
  g_printf ("> REQUEST NOTEBOOK: W: %d H: %d\n",
            requisition->width, requisition->height);
  */
}

/* - START - PRIVATE to gtk_notebook_ng_size_request */
static void
gtk_notebook_ng_max_tab_size (GtkNotebookNg *notebook,
                              gint          *width,
                              gint          *height)
{
  GtkNotebookNgPrivate *priv;
  GList *children;
  GtkWidget *tab;
  GtkRequisition child_requisition;
  gint res;

  g_return_if_fail (notebook);

  priv = GTK_NOTEBOOK_NG_GET_PRIVATE (notebook);

  children = g_queue_peek_head_link (priv->tabs);
  res  = 0;

  while (children)
    {
      tab = GTK_WIDGET (children->data);

      gtk_widget_size_request (tab, &child_requisition);

      *width = MAX (*width, child_requisition.width);
      *height = MAX (*height, child_requisition.height);
      
      children = children->next;
    }

  if (GTK_WIDGET_DRAWABLE (priv->previous))
    {
      gtk_widget_size_request (priv->previous, &child_requisition);
      *width = MAX (*width, child_requisition.width);
      *height = MAX (*height, child_requisition.height);
    }

  if (GTK_WIDGET_DRAWABLE (priv->next))
    {
      gtk_widget_size_request (priv->next, &child_requisition);
      *width = MAX (*width, child_requisition.width);
      *height = MAX (*height, child_requisition.height);
    }

  /*
  g_printf ("> MAX TAB SIZE: W: %d H: %d\n",
            *width, *height);
  */
}

static void
gtk_notebook_ng_size_allocate_tabs (GtkWidget     *widget,
                                    GtkAllocation *allocation,
                                    gint           max_tab_width,
                                    gint           max_tab_height)
{
  GtkNotebookNg *notebook;
  GtkNotebookNgPrivate *priv;
  GtkRequisition child_requisition;
  GtkAllocation child_allocation;
  GList *tabs;
  GtkWidget *tab;
  gint counter = 0;
  gint start_point_tabs = 0;
  gboolean need_button_next = FALSE;

  g_return_if_fail (GTK_IS_NOTEBOOK_NG (widget));

  //g_printf ("> ALLOCATE TABS\n");

  notebook = GTK_NOTEBOOK_NG (widget);
  priv = GTK_NOTEBOOK_NG_GET_PRIVATE (notebook);

  /* Start with the >previous< button */
  if (priv->offset > 0)
    {
      if (!GTK_WIDGET_DRAWABLE (priv->previous))
        gtk_widget_map (priv->previous);

      gtk_widget_size_request (priv->previous, &child_requisition);

      child_allocation.x = allocation->x + allocation->width
                           - child_requisition.width;
      child_allocation.y = allocation->y + allocation->height
                           - child_requisition.height;
      child_allocation.width = child_requisition.width;
      child_allocation.height = child_requisition.height;

      switch (priv->position)
        {
          case GTK_POS_BOTTOM:
            child_allocation.y = allocation->y + 
                                 ABS (allocation->height 
                                      - child_requisition.height);
            /* fall through */
          case GTK_POS_TOP:
            child_allocation.x = allocation->x;
            start_point_tabs = child_allocation.x + child_allocation.width;
            break;
          case GTK_POS_RIGHT:
            child_allocation.x = allocation->x + 
                                 ABS (allocation->width
                                      - child_requisition.width);
            /* fall through */
          case GTK_POS_LEFT:
            child_allocation.y = allocation->y;
            start_point_tabs = child_allocation.y + child_allocation.height;
            break;
        }

      gtk_widget_size_allocate (priv->previous, &child_allocation);
    }
  else if (GTK_WIDGET_DRAWABLE (priv->previous))
    {
      gtk_widget_unmap (priv->previous);
      gtk_widget_queue_resize (widget);
    }

  tabs = g_queue_peek_head_link (priv->tabs);

  for (counter = 0; counter < priv->offset; tabs = tabs->next, counter++)
    gtk_widget_unmap (GTK_WIDGET (tabs->data));

  while (tabs && !need_button_next)
    {
      tab = GTK_WIDGET (tabs->data);

      if (!GTK_WIDGET_DRAWABLE (tab))
        gtk_widget_map (tab);

      gtk_widget_size_request (tab, &child_requisition);

      child_allocation.x = allocation->x + allocation->width
                           - child_requisition.width;
      child_allocation.y = allocation->y + allocation->height
                           - child_requisition.height;
      child_allocation.width = (priv->homogeneous) 
                               ? max_tab_width : child_requisition.width;
      child_allocation.height = (priv->homogeneous)
                                ? max_tab_height : child_requisition.height;

      switch (priv->position)
        {
          case GTK_POS_TOP:
            child_allocation.y = allocation->y + max_tab_height
                                 - child_requisition.height;
            /* fall through */
          case GTK_POS_BOTTOM:
            child_allocation.x = start_point_tabs;
            start_point_tabs += child_requisition.width;

            if (child_allocation.x + child_allocation.width
                >= allocation->x + allocation->width)
              need_button_next = TRUE;

            if (child_allocation.y < 0)
              gtk_widget_queue_resize (widget);

            break;
          case GTK_POS_RIGHT:
            child_allocation.x = allocation->x + max_tab_width
                                 - child_requisition.width;
            /* fall through */
          case GTK_POS_LEFT:
            child_allocation.y = start_point_tabs;
            start_point_tabs += child_requisition.height;

            if (child_allocation.y + child_allocation.height
                >= allocation->y + allocation->height)
              need_button_next = TRUE;

            if (child_allocation.x < 0)
              gtk_widget_queue_resize (widget);

            break;
        }

      /*
      g_printf ("> ALLOC TAB: X: %d Y: %d W: %d H: %d\n",
                child_allocation.x, child_allocation.y,
                child_allocation.width, child_allocation.height);
      */

      gtk_widget_size_allocate (tab, &child_allocation);

      tabs = tabs->next;
    }

  if (need_button_next)
    {
      if (!GTK_WIDGET_DRAWABLE (priv->next))
        gtk_widget_map (priv->next);

      gtk_widget_size_request (priv->next, &child_requisition);

      child_allocation.x = allocation->x + allocation->width
                           - child_requisition.width;
      child_allocation.y = allocation->y + allocation->height
                           - child_requisition.height;
      child_allocation.width = child_requisition.width;
      child_allocation.height = child_requisition.height;

      switch (priv->position)
        {
          case GTK_POS_BOTTOM:
            child_allocation.y = allocation->y + allocation->height 
                                 - child_requisition.height;
            /* fall through */
          case GTK_POS_TOP:
            if (child_allocation.y < 0)
              gtk_widget_queue_resize (widget);
            break;
          case GTK_POS_RIGHT:
            child_allocation.x = allocation->x + allocation->width
                                 - child_requisition.width;
            /* fall through */
          case GTK_POS_LEFT:
            if (child_allocation.x < 0)
              gtk_widget_queue_resize (widget);
            break;
        }

      gtk_widget_size_allocate (priv->next, &child_allocation);
    }
  else if (GTK_WIDGET_DRAWABLE (priv->next))
    {
      gtk_widget_unmap (priv->next);
      gtk_widget_queue_resize (widget);
    }
}
/* - END - */

static void
gtk_notebook_ng_size_allocate (GtkWidget     *widget,
					                     GtkAllocation *allocation)
{
  GtkNotebookNg *notebook;
  GtkNotebookNgPrivate *priv;
  GtkAllocation tab_area, child_area;
  gint max_tab_width, max_tab_height, border_width;

  g_return_if_fail (GTK_IS_NOTEBOOK_NG (widget));

  widget->allocation = *allocation;

  /*
  g_printf ("> ALLOC: X: %d Y: %d W: %d H: %d\n",
            allocation->x, allocation->y,
            allocation->width, allocation->height);
  */

  notebook = GTK_NOTEBOOK_NG (widget);
  priv = GTK_NOTEBOOK_NG_GET_PRIVATE (notebook);

  max_tab_width = 0; 
  max_tab_height = 0;

  if (priv->show_tabs)
    gtk_notebook_ng_max_tab_size (notebook,
                                  &max_tab_width,
                                  &max_tab_height);

  tab_area.x = allocation->x;
  tab_area.y = allocation->y;
  tab_area.width = allocation->width;
  tab_area.height = allocation->height;

  child_area.x = allocation->x + max_tab_width;
  child_area.y = allocation->y + max_tab_height;
  child_area.width = allocation->width - tab_area.width;
  child_area.height = allocation->height - tab_area.height;

  switch (priv->position)
    {
      case GTK_POS_BOTTOM:
        child_area.y = allocation->y;
        tab_area.y = allocation->y + allocation->height - max_tab_height;
        /* fall through */
      case GTK_POS_TOP:
        tab_area.height = max_tab_height;
        child_area.x = allocation->x;
        child_area.width = allocation->width;
        child_area.height = allocation->height - tab_area.height;
        break;
      case GTK_POS_RIGHT:
        child_area.x = allocation->x;
        tab_area.x = allocation->x + allocation->width - max_tab_width;
        /* fall through */
      case GTK_POS_LEFT:
        tab_area.width = max_tab_width;
        child_area.y = allocation->y;
        child_area.height = allocation->height;
        child_area.width = allocation->width - tab_area.width;
        break;
    }

  /*
  g_printf ("> ALLOC TABS: X: %d Y: %d W: %d H: %d\n",
            tab_area.x, tab_area.y,
            tab_area.width, tab_area.height);
  */

  gtk_notebook_ng_size_allocate_tabs (widget, &tab_area,
                                      max_tab_width, max_tab_height);

  border_width = GTK_CONTAINER (widget)->border_width;

  child_area.x += border_width;
  child_area.y += border_width;
  child_area.width -= border_width;
  child_area.height -= border_width;

  /*
  g_printf ("> ALLOC CHILD: X: %d Y: %d W: %d H: %d\n",
            child_area.x, child_area.y,
            child_area.width, child_area.height);
  */

  priv->child_allocation = child_area;

  if (priv->active)
    gtk_widget_size_allocate (priv->active, &child_area);
}

static gint
gtk_notebook_ng_expose (GtkWidget      *widget,
					              GdkEventExpose *event)
{
  GtkNotebookNg *notebook;
  GtkNotebookNgPrivate *priv;

  g_return_val_if_fail (GTK_IS_NOTEBOOK_NG (widget), FALSE);

  notebook = GTK_NOTEBOOK_NG (widget);
  priv = GTK_NOTEBOOK_NG_GET_PRIVATE (notebook);

  if (GTK_WIDGET_DRAWABLE (widget))
    {
      //g_printf ("> DRAW\n");

      if (priv->show_tabs)
        {
          GList *tabs;
          GtkWidget *tab;
          
          tabs = g_queue_peek_head_link (priv->tabs);

          while (tabs)
            {
              tab = GTK_WIDGET (tabs->data);
              
              gtk_container_propagate_expose (GTK_CONTAINER (notebook),
                                              tab, event);

              tabs = tabs->next;
            }

          if (GTK_WIDGET_DRAWABLE (priv->previous))
            gtk_container_propagate_expose (GTK_CONTAINER (notebook),
                                            priv->previous, event);

          if (GTK_WIDGET_DRAWABLE (priv->next))
            gtk_container_propagate_expose (GTK_CONTAINER (notebook),
                                            priv->next, event);
        }

      if (priv->show_border)
        {
          GtkAllocation *child_area = &(priv->child_allocation);

          gtk_paint_box (widget->style, widget->window,
                         GTK_STATE_NORMAL, GTK_SHADOW_OUT,
                         &(event->area), widget, "notebook",
                         child_area->x, child_area->y,
                         child_area->width, child_area->height);
        }

      if (priv->active && GTK_WIDGET_DRAWABLE (priv->active))
        gtk_container_propagate_expose (GTK_CONTAINER (notebook),
                                        priv->active, event);
    }

  return FALSE;
}

/*** GtkContainer Methods ***/
static gchar*
gtk_notebook_ng_get_label (GtkNotebookNg *notebook)
{
  GtkNotebookNgPrivate *priv;
  gchar *label;

  g_return_val_if_fail (notebook, NULL);

  priv = GTK_NOTEBOOK_NG_GET_PRIVATE (notebook);

  label = g_malloc (10);

  g_sprintf (label, "Page %d", g_queue_get_length (priv->tabs));

  return label;
}

static void
gtk_notebook_ng_add (GtkContainer *container,
                     GtkWidget    *widget)
{
  GtkNotebookNg *notebook;
  GtkNotebookNgPrivate *priv;
  GtkWidget *tab;
  gchar *label;

  g_return_if_fail (GTK_IS_NOTEBOOK_NG (container));
  g_return_if_fail (widget && GTK_IS_WIDGET (widget));

  notebook = GTK_NOTEBOOK_NG (container);

  priv = GTK_NOTEBOOK_NG_GET_PRIVATE (notebook);

  label = gtk_notebook_ng_get_label (notebook);
  if (g_queue_get_length (priv->tabs) == 0)
    tab = gtk_radio_button_new_with_label_from_widget (NULL, label);
  else
    {
      GtkRadioButton *active_button = g_queue_peek_head (priv->tabs);

      tab = gtk_radio_button_new_with_label_from_widget (active_button, label);
    }
  //g_free (label);

  gtk_widget_set_parent (tab, GTK_WIDGET (notebook));
  gtk_widget_set_parent (widget, GTK_WIDGET (notebook));

  g_queue_push_tail (priv->tabs, tab);
  g_hash_table_insert (priv->tabs_to_widgets,
                       tab, widget);
  g_hash_table_insert (priv->widgets_to_tabs,
                       widget, tab);

  g_signal_connect (G_OBJECT (tab), "toggled",
                    G_CALLBACK (gtk_notebook_ng_select_cb), notebook);

  if (priv->show_tabs)
    {
      gtk_widget_show_all (tab);

      if (GTK_WIDGET_VISIBLE (tab)
          && !GTK_WIDGET_DRAWABLE (tab))
        gtk_widget_map (tab);
    }

  if (g_queue_get_length (priv->tabs) == 1)
    {
      priv->active = widget;

      if (!GTK_WIDGET_VISIBLE (priv->active))
        gtk_widget_show_all (priv->active);

      //g_printf ("> FIRST CHILD ");
    }
  else if (GTK_WIDGET_VISIBLE (widget))
    gtk_widget_hide_all (widget);

  //g_printf ("> CHILDREN: %s\n", label);
  g_free (label);

  if (GTK_WIDGET_VISIBLE (notebook))
    gtk_widget_queue_resize (GTK_WIDGET (notebook));
}

static void
gtk_notebook_ng_remove (GtkContainer *container,
                        GtkWidget    *widget)
{  
  GtkNotebookNg *notebook;
  GtkNotebookNgPrivate *priv;
  GList* tabs;
  GtkWidget *tab;

  g_return_if_fail (GTK_IS_NOTEBOOK_NG (container));
  g_return_if_fail (widget && GTK_IS_WIDGET (widget));

  notebook = GTK_NOTEBOOK_NG (container);

  priv = GTK_NOTEBOOK_NG_GET_PRIVATE (notebook);

  tab = g_hash_table_lookup (priv->widgets_to_tabs, widget);

  tabs = g_queue_find (priv->tabs, tab);

  if (tabs->next)
    gtk_notebook_ng_select_widget (notebook,
                                   GTK_WIDGET (tabs->next->data));
  else if (tabs->prev)
    gtk_notebook_ng_select_widget (notebook,
                                   GTK_WIDGET (tabs->prev->data));
  else
    gtk_notebook_ng_select_widget (notebook, NULL);

  gtk_widget_unparent (tab);
  gtk_widget_unparent (widget);

  g_queue_remove (priv->tabs, tab);
  g_hash_table_remove (priv->tabs_to_widgets, tab);
  g_hash_table_remove (priv->widgets_to_tabs, widget);

  gtk_widget_queue_resize (GTK_WIDGET (notebook));
}

static void
gtk_notebook_ng_forall (GtkContainer *container,
                        gboolean		  include_internals,
                        GtkCallback   callback,
                        gpointer      callback_data)
{
  GtkNotebookNgPrivate *priv;
  GList *tabs;
  GtkWidget *child_widget;

  g_return_if_fail (GTK_IS_NOTEBOOK_NG (container));

  priv = GTK_NOTEBOOK_NG_GET_PRIVATE (GTK_NOTEBOOK_NG (container));

  tabs = g_queue_peek_head_link (priv->tabs);

  while (tabs)
    {
      child_widget = g_hash_table_lookup (priv->tabs_to_widgets, 
                                          tabs->data);

      (* callback) (child_widget, callback_data);

      tabs = tabs->next;
    }

  if (include_internals)
    {
      (* callback) (priv->previous, callback_data);
      (* callback) (priv->next, callback_data);
    }
}

/* PRIVATE: GtkNotebookNg convenience functions */
static void
gtk_notebook_ng_select_widget (GtkNotebookNg *notebook,
                               GtkWidget     *widget)
{
  GtkNotebookNgPrivate *priv;

  g_return_if_fail (notebook);

  priv = GTK_NOTEBOOK_NG_GET_PRIVATE (notebook);

  if (priv->active)
    gtk_widget_hide_all (priv->active);

  if (widget)
    {
      priv->active = widget;

      gtk_widget_show_all (widget);
  
      if (GTK_WIDGET_VISIBLE (widget)
          && !GTK_WIDGET_DRAWABLE (widget))
          gtk_widget_map (widget);
    }
}

/* PRIVATE: GtkNotebookNg and notebook tabs connection */
static void
gtk_notebook_ng_select_cb (GtkButton     *button,
                           GtkNotebookNg *notebook)
{
  GtkNotebookNgPrivate *priv;
  GtkWidget *selected;

  g_return_if_fail (notebook);

  priv = GTK_NOTEBOOK_NG_GET_PRIVATE (notebook);

  selected = GTK_WIDGET (g_hash_table_lookup (priv->tabs_to_widgets,
                                              button));

  gtk_notebook_ng_select_widget (notebook, selected);
}

static void
gtk_notebook_ng_previous_cb (GtkButton     *button,
                             GtkNotebookNg *notebook)
{
  GtkNotebookNgPrivate *priv;

  g_return_if_fail (notebook);

  priv = GTK_NOTEBOOK_NG_GET_PRIVATE (notebook);

  priv->offset--;
  gtk_widget_queue_resize (GTK_WIDGET (notebook));
}

static void
gtk_notebook_ng_next_cb (GtkButton     *button,
                         GtkNotebookNg *notebook)
{
  GtkNotebookNgPrivate *priv;

  g_return_if_fail (notebook);

  priv = GTK_NOTEBOOK_NG_GET_PRIVATE (notebook);

  priv->offset++;

  gtk_widget_queue_resize (GTK_WIDGET (notebook));
}
