#include "hildon-gtk.h"
#include "hildon-gtk-treeview.h"

/*static HildonUIMode
get_ui_mode(GtkTreeView *tree_view)
{
  return GPOINTER_TO_INT(g_object_get_data(G_OBJECT(tree_view),
                                           "hildon_ui_mode"));
}*/

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
