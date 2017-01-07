#include "hildon-gtk.h"
#include "hildon-gtk-treeview.h"
#include "hildon-gtk-marshalers.h"
#include "hildon-enum-types.h"

#include <gtk/gtkprivate.h>

#include <libintl.h>

#ifdef ENABLE_NLS
#define P_(String) g_dgettext("hildon-libs-properties",String)
#else
#define P_(String) (String)
#endif

enum {
    PROP_HILDON_UI_MODE = 20 /* PROP_TOOLTIP_COLUMN + 1 */,
    PROP_ACTION_AREA_VISIBLE,
    PROP_ACTION_AREA_ORIENTATION
};

/* Signals */
enum
{
  ROW_INSENSITIVE,
  HILDON_ROW_TAPPED,
  LAST_SIGNAL
};

static guint tree_view_signals [LAST_SIGNAL] = { 0 };

static HildonUIMode
get_ui_mode(GtkTreeView *tree_view)
{
  return GPOINTER_TO_INT(g_object_get_data(G_OBJECT(tree_view),
                                           "hildon_ui_mode"));
}

static void
set_ui_mode(GtkTreeView *tree_view, HildonUIMode hildon_ui_mode)
{
    g_object_set_data(G_OBJECT(tree_view), "hildon_ui_mode",
                      GINT_TO_POINTER(hildon_ui_mode));
}

static HildonMode
get_mode(GtkTreeView *tree_view)
{
  return GPOINTER_TO_INT(g_object_get_data(G_OBJECT(tree_view),
                                           "hildon_mode"));
}

void
hildon_tree_view_set_hildon_ui_mode (GtkTreeView   *tree_view,
                                     HildonUIMode   hildon_ui_mode)
{
  GtkTreeSelection *selection;
  g_return_if_fail (GTK_IS_TREE_VIEW (tree_view));

  /* Don't check if the new mode matches the old mode; always continue
   * so that the selection corrections below always happen.
   */
  set_ui_mode(tree_view, hildon_ui_mode);

  if (get_mode(tree_view) == HILDON_DIABLO)
    return;

  /* For both normal and edit mode a couple of things are disabled. */

  selection = gtk_tree_view_get_selection(tree_view);
  /* Mode-specific settings */
  if (hildon_ui_mode == HILDON_UI_MODE_NORMAL)
    {
      gtk_tree_selection_set_mode (selection, GTK_SELECTION_NONE);
    }
  else if (hildon_ui_mode == HILDON_UI_MODE_EDIT)
    {
      if (gtk_tree_selection_get_mode (selection) == GTK_SELECTION_NONE)
        {
          gtk_tree_selection_set_mode (selection, GTK_SELECTION_SINGLE);
        }

      if (selection->type != GTK_SELECTION_MULTIPLE
          && gtk_tree_selection_count_selected_rows (selection) < 1)
        {
          GtkTreePath *path;

          /* Select the first item */
          path = gtk_tree_path_new_first ();
/* Why is that needed ?
  maybe use gtk_tree_view_real_move_cursor if really needed
          search_first_focusable_path (tree_view, &path,
                                       TRUE, NULL, NULL); */
          gtk_tree_selection_select_path (selection, path);
          gtk_tree_path_free (path);
        }
    }
  else
    g_assert_not_reached ();
}

static void (*old_gtk_tree_view_set_property)(GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec);
static void (*old_gtk_tree_view_get_property)(GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec);

static void
hildon_gtk_tree_view_set_property (GObject         *object,
                                   guint            prop_id,
                                   const GValue    *value,
                                   GParamSpec      *pspec)
{
  GtkTreeView *tree_view;

  tree_view = GTK_TREE_VIEW (object);

  switch (prop_id)
    {
    case PROP_HILDON_UI_MODE:
      hildon_tree_view_set_hildon_ui_mode (tree_view, g_value_get_enum (value));
      break;
    /*case PROP_ACTION_AREA_VISIBLE:
      hildon_tree_view_set_action_area_visible (tree_view, g_value_get_boolean (value));
      break;
    case PROP_ACTION_AREA_ORIENTATION:
      hildon_tree_view_set_action_area_orientation (tree_view, g_value_get_enum (value));
      break;*/
    default:
      old_gtk_tree_view_set_property (object, prop_id, value, pspec);
      break;
    }
}

static void
hildon_gtk_tree_view_get_property (GObject    *object,
                                   guint       prop_id,
                                   GValue     *value,
                                   GParamSpec *pspec)
{
  GtkTreeView *tree_view;

  tree_view = GTK_TREE_VIEW (object);

  switch (prop_id)
    {
    case PROP_HILDON_UI_MODE:
      g_value_set_enum (value, get_ui_mode(tree_view));
      break;
    /*case PROP_ACTION_AREA_VISIBLE:
      g_value_set_boolean (value, tree_view->priv->action_area_visible);
      break;
    case PROP_ACTION_AREA_ORIENTATION:
      g_value_set_enum (value, tree_view->priv->action_area_orientation);
      break;*/
    default:
      old_gtk_tree_view_get_property (object, prop_id, value, pspec);
      break;
    }
}

void
hildon_subclass_gtk_treeview(void)
{
  GObjectClass *gobject_class;
  GtkTreeViewClass *klass;
  GtkWidgetClass *widget_class;

  g_warning("hildon_subclass_gtk_treeview");

  klass = g_type_class_ref(GTK_TYPE_TREE_VIEW);
  widget_class = GTK_WIDGET_CLASS(klass);
  gobject_class = G_OBJECT_CLASS(klass);

  old_gtk_tree_view_set_property = gobject_class->set_property;
  old_gtk_tree_view_get_property = gobject_class->get_property;
  gobject_class->set_property = hildon_gtk_tree_view_set_property;
  gobject_class->get_property = hildon_gtk_tree_view_get_property;

  widget_class->focus = hildon_gtk_tree_view_focus;

  /**
   * GtkTreeView::row-insensitive:
   * @tree_view: the object which received the signal.
   * @path: the path where the cursor is tried to be moved.
   *
   * Emitted when the user tries to move cursor to an insesitive row.
   *
   * Since: maemo 1.0
   * Stability: Unstable
   */
  tree_view_signals[ROW_INSENSITIVE] =
     g_signal_new ("row_insensitive",
                  G_TYPE_FROM_CLASS (gobject_class),
                  G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
                  0,
                  NULL, NULL,
                  _hildon_gtk_marshal_VOID__OBJECT,
                  G_TYPE_NONE, 1,
                  GTK_TYPE_TREE_PATH);

  tree_view_signals[HILDON_ROW_TAPPED] =
      g_signal_new ("hildon_row_tapped",
                    G_TYPE_FROM_CLASS (gobject_class),
                    G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
                    0,
                    NULL, NULL,
                    _hildon_gtk_marshal_VOID__BOXED,
                    G_TYPE_NONE, 1,
  GTK_TYPE_TREE_PATH);

  /**
    * GtkTreeView:hildon-ui-mode:
    *
    * Specifies which UI mode to use.  A setting of #HILDON_UI_MODE_NORMAL
    * will cause the tree view to disable selections and emit row-activated
    * as soon as a row is pressed.  When #HILDON_UI_MODE_EDIT is set,
    * selections can be made according to the setting of the mode on
    * GtkTreeSelection.
    *
    * Toggling this property will cause the tree view to select an
    * appropriate selection mode if not already done.
    *
    * Since: maemo 5.0
    * Stability: unstable
    */
  g_object_class_install_property (gobject_class,
                                   PROP_HILDON_UI_MODE,
                                   g_param_spec_enum ("hildon-ui-mode",
                                                      P_("Hildon UI Mode"),
                                                      P_("The Hildon UI mode according to which the tree view should behave"),
                                                      HILDON_TYPE_UI_MODE,
                                                      HILDON_UI_MODE_NORMAL,
                                                      GTK_PARAM_READWRITE));

  /**
    * GtkTreeView:action-area-visible:
    *
    * Makes the action area of the GtkTreeView visible or invisible.
    * Based on the value of the GtkTreeView:action-area-orientation
    * property a certain height will be allocated above the first row
    * for the action area.
    *
    * Since: maemo 5.0
    * Stability: unstable
    */
  g_object_class_install_property (gobject_class,
                                   PROP_ACTION_AREA_VISIBLE,
                                   g_param_spec_boolean ("action-area-visible",
                                                         P_("Action Area Visible"),
                                                         P_("Whether the action area above the first row is visible"),
                                                         FALSE,
                                                         GTK_PARAM_READWRITE));

  /**
    * GtkTreeView:action-area-orientation:
    *
    * Sets the orientation of the action area.  This is either
    * horizontal (landscape) or vertical (portrait).  The height of
    * the action area depends on this setting.
    *
    * Since: maemo 5.0
    * Stability: unstable
    */
  g_object_class_install_property (gobject_class,
                                   PROP_ACTION_AREA_ORIENTATION,
                                   g_param_spec_enum ("action-area-orientation",
                                                      P_("Action Area Orientation"),
                                                      P_("Determines the orientation of the action area."),
                                                      GTK_TYPE_ORIENTATION,
                                                      GTK_ORIENTATION_HORIZONTAL,
                                                      GTK_PARAM_READWRITE));
}

