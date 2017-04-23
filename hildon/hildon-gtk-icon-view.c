#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <gtk/gtk.h>
#include <gtk/gtkprivate.h>

#include "hildon-gtk.h"
#include "hildon-gtk-icon-view.h"
#include "hildon-enum-types.h"

#include <libintl.h>

#ifdef ENABLE_NLS
#define P_(String) g_dgettext("hildon-libs-properties",String)
#else
#define P_(String) (String)
#endif

#define HILDON_GTK_ICON_VIEW_PRIVATE(o) get_hildon_private (GTK_ICON_VIEW(o))

#define HILDON_TICK_MARK_SIZE 48
#define HILDON_ROW_HEADER_HEIGHT 35

static guint PROP_HILDON_UI_MODE = 0;

static void (*old_gtk_icon_view_set_property)(GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec);
static void (*old_gtk_icon_view_get_property)(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec);

typedef struct _HildonGtkIconViewPrivate HildonGtkIconViewPrivate;
struct _HildonGtkIconViewPrivate
{
  HildonUIMode hildon_ui_mode;

  GtkIconViewItem *queued_activate_item;
  GtkIconViewItem *queued_select_item;

  HildonIconViewRowHeaderFunc row_header_func;
  gpointer row_header_data;
  GDestroyNotify row_header_destroy;
  PangoLayout *row_header_layout;

  GdkPixbuf *tickmark_icon;
};

static inline HildonGtkIconViewPrivate *
get_hildon_private (GtkIconView *icon_view)
{
  HildonGtkIconViewPrivate *hpriv =
      g_object_get_data (G_OBJECT (icon_view), "hildon_gtk_icon_view_private");

  if (!hpriv)
  {
    hpriv = g_slice_new0 (HildonGtkIconViewPrivate);
    hpriv->hildon_ui_mode = HILDON_UI_MODE_NORMAL;
    hpriv->queued_activate_item = NULL;
    hpriv->queued_select_item = NULL;
    g_object_set_data (G_OBJECT (icon_view), "hildon_gtk_icon_view_private",
                       hpriv);
  }

  return hpriv;
}

static inline gboolean
item_is_header (GtkIconView     *icon_view,
                GtkIconViewItem *item)
{
  HildonGtkIconViewPrivate *hpriv = HILDON_GTK_ICON_VIEW_PRIVATE (icon_view);
  gboolean is_header = FALSE;

  if (hpriv->row_header_func)
    {
      GtkTreeIter iter;

      if (gtk_tree_model_get_flags (icon_view->priv->model) & GTK_TREE_MODEL_ITERS_PERSIST)
        {
          GtkTreePath *path;

          path = gtk_tree_path_new_from_indices (item->index, -1);
          if (!gtk_tree_model_get_iter (icon_view->priv->model, &iter, path))
            return is_header;
          gtk_tree_path_free (path);
        }
      else
        iter = item->iter;

      is_header = (* hpriv->row_header_func) (icon_view->priv->model, &iter,
                                              NULL, hpriv->row_header_data);
    }

  return is_header;
}

static gboolean
search_first_selectable_path (GtkIconView  *icon_view,
                              GtkTreePath **path,
                              gboolean      search_forward)
{
  int index;
  GList *list;

  if (!path || !*path)
    return FALSE;

  index = gtk_tree_path_get_indices (*path)[0];
  list = g_list_nth (icon_view->priv->items, index);

  while (list && item_is_header (icon_view, list->data))
    {
      if (search_forward)
        {
          index++;
          list = list->next;
        }
      else
        {
          index--;
          list = list->prev;
        }
    }

  if (!list)
    return FALSE;

  gtk_tree_path_up (*path);
  gtk_tree_path_append_index (*path, index);

  return TRUE;
}

void
hildon_icon_view_set_hildon_ui_mode (GtkIconView   *icon_view,
                                     HildonUIMode   hildon_ui_mode)
{
  HildonGtkIconViewPrivate *hpriv = HILDON_GTK_ICON_VIEW_PRIVATE (icon_view);
  HildonMode mode;

  g_return_if_fail (GTK_IS_ICON_VIEW (icon_view));

  hpriv->hildon_ui_mode = hildon_ui_mode;
  gtk_widget_style_get (GTK_WIDGET (icon_view), "hildon-mode", &mode, NULL);

  if (mode == HILDON_DIABLO)
    return;

  if (hildon_ui_mode == HILDON_UI_MODE_NORMAL)
      {
        gtk_icon_view_set_selection_mode (icon_view, GTK_SELECTION_NONE);
      }
    else if (hildon_ui_mode == HILDON_UI_MODE_EDIT)
      {
        int count = 0;
        GList *list;

        if (gtk_icon_view_get_selection_mode (icon_view) == GTK_SELECTION_NONE)
          {
            gtk_icon_view_set_selection_mode (icon_view, GTK_SELECTION_SINGLE);
          }

        if (icon_view->priv->selection_mode != GTK_SELECTION_MULTIPLE)
          {
            /* Instead of using gtk_icon_view_get_selected_items() we walk
             * over the list of items ourselves to save allocating/deallocating
             * all paths.
             */
            for (list = icon_view->priv->items; list; list = list->next)
              {
                GtkIconViewItem *item = list->data;

                if (item->selected)
                  {
                    count++;
                    break;
                  }
              }

            if (!count)
              {
                GtkTreePath *path;

                /* Select the first item */
                path = gtk_tree_path_new_first ();
                search_first_selectable_path (icon_view, &path, TRUE);
                gtk_icon_view_select_path (icon_view, path);
                gtk_tree_path_free (path);
              }
          }
      }
    else
      g_assert_not_reached ();
}

HildonUIMode
hildon_icon_view_get_hildon_ui_mode (GtkIconView *icon_view)
{
  g_return_val_if_fail (GTK_IS_ICON_VIEW (icon_view), 0);

  return HILDON_GTK_ICON_VIEW_PRIVATE (icon_view)->hildon_ui_mode;
}

static void
hildon_gtk_icon_view_set_property (GObject         *object,
                                   guint            prop_id,
                                   const GValue    *value,
                                   GParamSpec      *pspec)
{
  GtkIconView *icon_view = GTK_ICON_VIEW (object);

  if (prop_id == PROP_HILDON_UI_MODE)
    hildon_icon_view_set_hildon_ui_mode (icon_view, g_value_get_enum (value));
  else
    old_gtk_icon_view_set_property(object, prop_id, value, pspec);
}

static void
hildon_gtk_icon_view_get_property (GObject         *object,
                                   guint            prop_id,
                                   GValue          *value,
                                   GParamSpec      *pspec)
{
  GtkIconView *icon_view = GTK_ICON_VIEW (object);

  if (prop_id == PROP_HILDON_UI_MODE)
    g_value_set_enum (value, hildon_icon_view_get_hildon_ui_mode (icon_view));
  else
    old_gtk_icon_view_get_property (object, prop_id, value, pspec);
}

static void (*old_gtk_icon_view_style_set)(GtkWidget *widget, GtkStyle *previous_style);

static void
hildon_gtk_icon_view_style_set (GtkWidget *widget,
                                GtkStyle *previous_style)
{
  GtkIconView *icon_view = GTK_ICON_VIEW (widget);
  HildonGtkIconViewPrivate *hpriv = HILDON_GTK_ICON_VIEW_PRIVATE (widget);

  /* Reset the UI mode */
  hildon_icon_view_set_hildon_ui_mode (
        icon_view, hildon_icon_view_get_hildon_ui_mode (icon_view));

  if (hpriv->tickmark_icon)
    g_object_unref (hpriv->tickmark_icon);

  hpriv->tickmark_icon =
      gtk_icon_theme_load_icon (gtk_icon_theme_get_default (),
                                "widgets_tickmark_grid", HILDON_TICK_MARK_SIZE,
                                0, NULL);

  old_gtk_icon_view_style_set (widget, previous_style);
}

static void (*old_gtk_icon_view_destroy)(GtkObject *object);
static void
hildon_gtk_icon_view_destroy (GtkObject *object)
{
  HildonGtkIconViewPrivate *hpriv = HILDON_GTK_ICON_VIEW_PRIVATE (object);

  if (hpriv->row_header_destroy && hpriv->row_header_data)
    {
      (* hpriv->row_header_destroy) (hpriv->row_header_data);
      hpriv->row_header_data = NULL;
    }

  if (hpriv->row_header_layout != NULL)
    {
      g_object_unref (hpriv->row_header_layout);
      hpriv->row_header_layout = NULL;
    }

  if (hpriv->tickmark_icon)
    {
        g_object_unref (hpriv->tickmark_icon);
        hpriv->tickmark_icon = NULL;
    }

  g_slice_free (HildonGtkIconViewPrivate, hpriv);
  g_object_set_data (G_OBJECT (object), "hildon_gtk_icon_view_private", NULL);

  old_gtk_icon_view_destroy (object);
}

static gboolean (*old_gtk_icon_view_expose)(GtkWidget *widget, GdkEventExpose *expose);
static gboolean
hildon_gtk_icon_view_expose (GtkWidget *widget, GdkEventExpose *expose)
{
  GtkIconView *icon_view = GTK_ICON_VIEW (widget);
  HildonGtkIconViewPrivate *hpriv;
  HildonMode mode;
  GList *icons;
  gboolean ret;

  ret = old_gtk_icon_view_expose (widget, expose);

  if (expose->window != icon_view->priv->bin_window)
    return ret;

  gtk_widget_style_get (widget, "hildon-mode", &mode, NULL);
  hpriv = HILDON_GTK_ICON_VIEW_PRIVATE (widget);

  for (icons = icon_view->priv->items; icons; icons = icons->next)
    {
      GtkIconViewItem *item = icons->data;
      GdkRectangle area;

      area.x = item->x;
      area.y = item->y;
      area.width = item->width;
      area.height = item->height;

      if (gdk_region_rect_in (expose->region, &area) == GDK_OVERLAP_RECTANGLE_OUT)
        continue;

      if (icon_view->priv->selection_mode == GTK_SELECTION_MULTIPLE
          && mode == HILDON_FREMANTLE
          && hpriv->hildon_ui_mode == HILDON_UI_MODE_EDIT
          && item->selected)
        {
          gdk_draw_pixbuf (icon_view->priv->bin_window,
                           NULL,
                           hpriv->tickmark_icon,
                           0, 0,
                           item->x + (item->width - HILDON_TICK_MARK_SIZE) / 2,
                           item->y + (item->height - HILDON_TICK_MARK_SIZE) / 2,
                           HILDON_TICK_MARK_SIZE,
                           HILDON_TICK_MARK_SIZE,
                           GDK_RGB_DITHER_MAX,
                           0, 0);
        }
    }

  return ret;
}

void
hildon_subclass_gtk_icon_view(void)
{
  GObjectClass *gobject_class;
  GtkWidgetClass *widget_class;
  GtkObjectClass *object_class;
  GParamSpec **specs;

  widget_class = g_type_class_ref(GTK_TYPE_ICON_VIEW);
  gobject_class = G_OBJECT_CLASS(widget_class);
  object_class = GTK_OBJECT_CLASS(widget_class);

  specs = g_object_class_list_properties (gobject_class, &PROP_HILDON_UI_MODE);
  g_free (specs);

  old_gtk_icon_view_set_property = gobject_class->set_property;
  old_gtk_icon_view_get_property = gobject_class->get_property;

  gobject_class->set_property = hildon_gtk_icon_view_set_property;
  gobject_class->get_property = hildon_gtk_icon_view_get_property;

  old_gtk_icon_view_style_set = widget_class->style_set;
  widget_class->style_set = hildon_gtk_icon_view_style_set;
  old_gtk_icon_view_expose = widget_class->expose_event;
  widget_class->expose_event = hildon_gtk_icon_view_expose;

  old_gtk_icon_view_destroy = object_class->destroy;
  object_class->destroy = hildon_gtk_icon_view_destroy;

  /**
   * GtkIconView::hildon-ui-mode
   *
   * Specifies which UI mode to use.  A setting of #HILDON_UI_MODE_NORMAL
   * will cause the icon view to disable selections and emit item-activated
   * as soon as an item is pressed.  When #HILDON_UI_MODE_EDIT is set,
   * selections can be made according to the setting of the mode in
   * GtkIconView::selection-mode.
   *
   * Toggling this property will cause the icon view to select an
   * appropriate selection mode if not already done.
   *
   * Since: maemo 5.0
   * Stability: unstable.
   */
  g_object_class_install_property (gobject_class,
                                   PROP_HILDON_UI_MODE,
                                   g_param_spec_enum ("hildon-ui-mode",
                                                      P_("Hildon UI Mode"),
                                                      P_("The mode according to which the icon view should behave"),
                                                      HILDON_TYPE_UI_MODE,
                                                      HILDON_UI_MODE_NORMAL,
                                                      GTK_PARAM_READWRITE));
}
