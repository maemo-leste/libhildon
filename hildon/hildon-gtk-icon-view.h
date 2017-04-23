#ifndef                                         __HILDON_GTK_ICON_VIEW_H__
#define                                         __HILDON_GTK_ICON_VIEW_H__

typedef struct _GtkIconViewItem GtkIconViewItem;
struct _GtkIconViewItem
{
  GtkTreeIter iter;
  gint index;

  gint row, col;

  /* Bounding box */
  gint x, y, width, height;

  /* Individual cells.
   * box[i] is the actual area occupied by cell i,
   * before, after are used to calculate the cell
   * area relative to the box.
   * See gtk_icon_view_get_cell_area().
   */
  gint n_cells;
  GdkRectangle *box;
  gint *before;
  gint *after;

  guint selected : 1;
  guint selected_before_rubberbanding : 1;

};

struct _GtkIconViewPrivate
{
  gint width, height;

  GtkSelectionMode selection_mode;

  GdkWindow *bin_window;

  GList *children;

  GtkTreeModel *model;

  GList *items;

  GtkAdjustment *hadjustment;
  GtkAdjustment *vadjustment;

  guint layout_idle_id;

  gboolean doing_rubberband;
  gint rubberband_x1, rubberband_y1;
  gint rubberband_x2, rubberband_y2;

  guint scroll_timeout_id;
  gint scroll_value_diff;
  gint event_last_x, event_last_y;

  GtkIconViewItem *anchor_item;
  GtkIconViewItem *cursor_item;
  GtkIconViewItem *edited_item;
  GtkCellEditable *editable;

  GtkIconViewItem *last_single_clicked;

  GList *cell_list;
  guint n_cells;

  gint cursor_cell;

  GtkOrientation item_orientation;

  gint columns;
  gint item_width;
  gint spacing;
  gint row_spacing;
  gint column_spacing;
  gint margin;
  gint item_padding;

  gint text_column;
  gint markup_column;
  gint pixbuf_column;

  gint pixbuf_cell;
  gint text_cell;

  gint tooltip_column;

  /* Drag-and-drop. */
  GdkModifierType start_button_mask;
  gint pressed_button;
  gint press_start_x;
  gint press_start_y;

  GdkDragAction source_actions;
  GdkDragAction dest_actions;

  GtkTreeRowReference *dest_item;
  GtkIconViewDropPosition dest_pos;

  /* scroll to */
  GtkTreeRowReference *scroll_to_path;
  gfloat scroll_to_row_align;
  gfloat scroll_to_col_align;
  guint scroll_to_use_align : 1;

  guint source_set : 1;
  guint dest_set : 1;
  guint reorderable : 1;
  guint empty_view_drop :1;

  guint modify_selection_pressed : 1;
  guint extend_selection_pressed : 1;

  guint draw_focus : 1;
};

void
hildon_subclass_gtk_icon_view(void);

#endif /* __HILDON_GTK_ICON_VIEW_H__ */
